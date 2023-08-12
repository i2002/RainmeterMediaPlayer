/* Copyright (C) 2021 Tudor Butufei
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include "MediaPlayer.h"

MediaPlayer::MediaPlayer(void* rm)
{
	this->rm = rm;
	workerThread = thread(&MediaPlayer::Runtime, this);
}

MediaPlayer::~MediaPlayer()
{
	PushMessage(PlayerActions::FINALIZE);
	workerThread.join();
}

void MediaPlayer::Reload(PlayerType t)
{
	lock_guard<mutex> guard(typeMutex);
	type = t;
}

double MediaPlayer::Update()
{
	unique_lock<mutex> uLock(dataMutex);
	PushMessage(PlayerActions::UPDATE_DATA);
	valueUpdated.wait(uLock);

	typeMutex.lock();
	PlayerType t = type;
	typeMutex.unlock();

	if(updateError != L"")
		RmLogF(rm, LOG_WARNING, L"[MediaPlayer.dll] Get media properties failed: %s", updateError.c_str());

	switch (t)
	{
	case PlayerType::NUMBER:
		return number;

	case PlayerType::YEAR:
		return year;

	case PlayerType::DURATION:
		return duration;

	case PlayerType::POSITION:
		return position;

	case PlayerType::PROGRESS:
		return progress;

	case PlayerType::RATING:
		return rating;

	case PlayerType::REPEAT:
		return repeat;
		
	case PlayerType::SHUFFLE:
		return shuffle;

	case PlayerType::STATE:
		return state;

	case PlayerType::STATUS:
		return status;

	case PlayerType::VOLUME:
		return volume;

#ifdef _DEBUG
	case PlayerType::UPDATE_TICK:
		return updateTick;
#endif
	}

	return 0.0;
}

LPCWSTR MediaPlayer::GetString()
{
	lock_guard guard(dataMutex);
	
	typeMutex.lock();
	PlayerType t = type;
	typeMutex.unlock();

	switch (t)
	{
	case PlayerType::ARTIST:
		return artist.c_str();

	case PlayerType::ALBUM:
		return album.c_str();

	case PlayerType::TITLE:
		return title.c_str();

	case PlayerType::GENRE:
		return genre.c_str();

	case PlayerType::COVER:
		return cover.c_str();

	case PlayerType::FILE:
		return file.c_str();

	case PlayerType::DURATION:
		return durationStr.c_str();

	case PlayerType::POSITION:
		return positionStr.c_str();
	}

	return nullptr;
}

void MediaPlayer::ExecuteBang(LPCWSTR args)
{
	wstringstream sstr(args);
	wstring command_name, val;
	sstr >> command_name >> val;
	transform(command_name.begin(), command_name.end(), command_name.begin(), towlower);

	if (command_name == L"pause")
		PushMessage(PlayerActions::PAUSE);
	else if (command_name == L"play")
		PushMessage(PlayerActions::PLAY);
	else if (command_name == L"playpause")
		PushMessage(PlayerActions::PLAY_PAUSE);
	else if (command_name == L"stop")
		PushMessage(PlayerActions::STOP);
	else if (command_name == L"next")
		PushMessage(PlayerActions::NEXT);
	else if (command_name == L"previous")
		PushMessage(PlayerActions::PREVIOUS);
	else if (command_name == L"openplayer")
		PushMessage(PlayerActions::OPEN_PLAYER);
	else if (command_name == L"closeplayer")
		PushMessage(PlayerActions::CLOSE_PLAYER);
	else if (command_name == L"toggleplayer")
		PushMessage(PlayerActions::TOGGLE_PLAYER);
	else if (command_name == L"setposition")
	{
		int position = ParseIntArg(val, 0);
		bool abs = val[0] != '+' && val[0] != '-';
		PushMessage(PlayerActions::SET_POSITION, position, abs);
	}
	else if (command_name == L"setrating")
	{
		PushMessage(PlayerActions::SET_RATING, ParseIntArg(val, 5));
	}
	else if (command_name == L"setshuffle")
	{
		PushMessage(PlayerActions::SET_SHUFFLE, ParseIntArg(val));
	}
	else if (command_name == L"setrepeat")
	{
		PushMessage(PlayerActions::SET_REPEAT, ParseIntArg(val));
	}
	else if (command_name == L"setvolume")
	{
		int position = ParseIntArg(val);
		bool abs = val[0] != '+' && val[0] != '-';
		PushMessage(PlayerActions::SET_VOLUME, position, abs);
	}
	else
		RmLogF(rm, LOG_ERROR, L"[MediaPlayer.dll] Unknown command: %ls", command_name.c_str());
}

//
void MediaPlayer::PushMessage(PlayerActions msg, int arg, bool abs)
{
	{
		lock_guard<mutex> guard(queueMutex);
		messageQueue.push({ msg, arg, abs });
	}
	messagesAvailable.notify_one();
}

void MediaPlayer::Runtime()
{
	Initialize();

	unique_lock<mutex> uLock(queueMutex);
	bool done = ProcessMessages();
	while (!done)
	{
		messagesAvailable.wait(uLock);
		done = ProcessMessages();
	}

	Deinitialize();
}

void MediaPlayer::Initialize()
{
	init_apartment();
	m = MediaSessionManager::RequestAsync().get();
}

void MediaPlayer::Deinitialize()
{
	while (messageQueue.size() > 0)
	{
		messageQueue.pop();
	}
	uninit_apartment();
}

bool MediaPlayer::ProcessMessages()
{
	bool done = false;
	while (!messageQueue.empty())
	{
		auto [action, arg, abs] = messageQueue.front();
		if (action == PlayerActions::UPDATE_DATA)
			UpdateData();
		else if (action == PlayerActions::FINALIZE)
			done = true;
		else
			HandlePlayerAction(action, arg, abs);

		messageQueue.pop();
	}

	return done;
}

void MediaPlayer::HandlePlayerAction(PlayerActions action, int arg, bool abs)
{
	auto session = m.GetCurrentSession();

	if (session == nullptr)
		return;

	switch (action)
	{
	case PlayerActions::PAUSE:
		session.TryPauseAsync().get();
		break;

	case PlayerActions::PLAY:
		session.TryPlayAsync().get();
		break;

	case PlayerActions::PLAY_PAUSE:
		session.TryTogglePlayPauseAsync().get();
		break;

	case PlayerActions::STOP:
		session.TryStopAsync().get();
		break;

	case PlayerActions::NEXT:
		session.TrySkipNextAsync().get();
		break;

	case PlayerActions::PREVIOUS:
		session.TrySkipPreviousAsync().get();
		break;

	case PlayerActions::SET_POSITION:
	{
		int new_pos = (abs ? 0 : position) + arg * duration / 100;
		long long new_time = TimeSpan(chrono::seconds(new_pos)).count();;
		session.TryChangePlaybackPositionAsync(new_time).get();
		break;
	}

	case PlayerActions::SET_RATING:
		rating = arg;
		break;

	case PlayerActions::SET_REPEAT:
	{
		auto repeatMode = (MediaPlaybackAutoRepeatMode)(arg != -1 ? arg : !repeat);
		session.TryChangeAutoRepeatModeAsync(repeatMode).get();
		break;
	}

	case PlayerActions::SET_SHUFFLE:
	{
		bool shuffleMode = arg != -1 ? arg : !shuffle;
		session.TryChangeShuffleActiveAsync(shuffleMode).get();
		break;
	}

	case PlayerActions::SET_VOLUME:
		volume = (abs ? 0 : volume) + arg;
		break;
	}
}

void MediaPlayer::UpdateData()
{
	lock_guard guard(dataMutex);
	auto session = m.GetCurrentSession();
	if (session == nullptr)
	{
#ifdef _DEBUG
		updateTick = updateTick == 1 ? 2 : 1;
#endif
		ResetValues();
	}
	else
	{
		MediaProperties props = nullptr;
		try
		{
			props = session.TryGetMediaPropertiesAsync().get();
		}
		catch (hresult_error const &ex)
		{
#ifdef _DEBUG
			updateTick = updateTick == 3 ? 4 : 3;
#endif
			ResetValues();
			updateError = ex.message();
			valueUpdated.notify_one();
			return;
		}

		auto playback = session.GetPlaybackInfo();
		auto timeline = session.GetTimelineProperties();
		auto newPlayerName = session.SourceAppUserModelId();

#ifdef _DEBUG
		updateTick = updateTick == 5 ? 6 : 5;
#endif
		UpdateCover(newPlayerName, props);
		UpdateValues(newPlayerName, props, playback, timeline);
	}
	updateError = L"";
	valueUpdated.notify_one();
}

void MediaPlayer::UpdateCover(hstring newPlayerName, MediaProperties props)
{
	hstring path = cover;
	hstring player = L"";

	typeMutex.lock();
	bool cover_type = type == PlayerType::COVER;
	typeMutex.unlock();

	if (!cover_type || props.Thumbnail() == nullptr)
		path = L"";

	else if (path == L"" || props.Title() != title || props.Artist() != artist || props.AlbumTitle() != album || newPlayerName != playerName)
	{
		path = ImageUtils::SaveCover(props.Thumbnail());

		if (newPlayerName == L"Spotify.exe")
			path = ImageUtils::CropCover(path);
	}
	else
		return;

	cover = path;
}


void MediaPlayer::UpdateValues(hstring newPlayerName, MediaProperties props, PlaybackInfo playback, TimelineProperties timeline)
{
	playerName = newPlayerName;
	artist = props.Artist();
	album = props.AlbumTitle();
	title = props.Title();
	number = props.TrackNumber();
	year = 0;
	genre = JoinGenres(props.Genres());
	file = L"";
	duration = timeline != nullptr ? GetDuration(timeline.StartTime(), timeline.EndTime()) : 0;
	durationStr = FormatTime(duration);
	position = timeline != nullptr ? GetPosition(timeline.Position()) : 0;
	positionStr = FormatTime(position);
	progress = duration != 0 ? position * 100 / duration : 0;
	rating = 5;
	repeat = (playback != nullptr && playback.AutoRepeatMode() != nullptr) ? (playback.AutoRepeatMode().Value() != MediaPlaybackAutoRepeatMode::None) : 0;
	shuffle = (playback != nullptr && playback.IsShuffleActive() != nullptr) ? playback.IsShuffleActive().Value() : 0;
	state = playback != nullptr ? GetState(playback.PlaybackStatus()) : 0;
	status = playback != nullptr ? GetStatus(playback.PlaybackStatus()) : 0;
	volume = 0;
}

void MediaPlayer::ResetValues()
{
	playerName = L"";
	artist = L"";
	album = L"";
	title = L"";
	number = 0;
	year = 0;
	genre = L"";
	cover = L"";
	file = L"";
	duration = 0;
	durationStr = L"";
	position = 0;
	positionStr = L"";
	progress = 0;
	rating = 5;
	repeat = 0;
	shuffle = 0;
	state = 0;
	status = 0;
	volume = 0;
}

int MediaPlayer::GetDuration(TimeSpan start, TimeSpan end)
{
	long long start_time = chrono::duration_cast<chrono::seconds>(start).count();
	long long end_time = chrono::duration_cast<chrono::seconds>(end).count();
	return (int)(end_time - start_time);
}

int MediaPlayer::GetPosition(TimeSpan position)
{
	return (int)chrono::duration_cast<chrono::seconds>(position).count();
}

int MediaPlayer::GetState(PlaybackStatus s)
{
	switch (s)
	{
	case PlaybackStatus::Stopped:
		return 0;

	case PlaybackStatus::Playing:
		return 1;

	case PlaybackStatus::Paused:
		return 2;

	default:
		return 0;
	}
}

int MediaPlayer::GetStatus(PlaybackStatus s)
{
	switch (s)
	{
	case PlaybackStatus::Closed:
		return 0;

	case PlaybackStatus::Opened:
		return 1;

	default:
		return 1;
	}
}

hstring MediaPlayer::JoinGenres(IVectorView<hstring> genres)
{
	hstring result = L"";
	for (hstring s : genres)
		result = result + L" " + s;

	return result;
}

hstring MediaPlayer::FormatTime(int time)
{
	WCHAR c[20];
    swprintf(c, 20, L"%02d:%02d", time / 60, time % 60);
	return c;
}

int MediaPlayer::ParseIntArg(wstring str, int def)
{
	try
	{
		return stoi(str);
	}
	catch (const invalid_argument)
	{
		RmLogF(rm, LOG_ERROR, L"[MediaPlayer.dll] Expected integer command argument: %ls", str.c_str());
		return def;
	}
}