/* Copyright (C) 2021 Tudor Butufei
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#pragma once

// std imports
#include <sstream>
#include <tuple>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

// windows imports
#include <Windows.h>
#include <gdiplus.h>

// WinRT imports
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>

// self imports
#include "PluginEnums.h"
#include "ImageUtils.h"
#include "../API/RainmeterAPI.h"

// namespaces
using namespace std;
using namespace Gdiplus;

// WinRT namespaces
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Media;
using namespace winrt::Windows::Media::Control;

// alias longer names
typedef GlobalSystemMediaTransportControlsSessionManager MediaSessionManager;
typedef GlobalSystemMediaTransportControlsSession MediaSession;
typedef GlobalSystemMediaTransportControlsSessionMediaProperties MediaProperties;
typedef GlobalSystemMediaTransportControlsSessionPlaybackInfo PlaybackInfo;
typedef GlobalSystemMediaTransportControlsSessionPlaybackStatus PlaybackStatus;
typedef GlobalSystemMediaTransportControlsSessionTimelineProperties TimelineProperties;

class MediaPlayer
{
	void* rm;
	MediaSessionManager m = nullptr;
	PlayerType type = PlayerType::UNKNOWN;

	// multithreading
	queue<tuple <PlayerActions, int, bool>> messageQueue;
	thread workerThread;
	mutex queueMutex;
	mutex dataMutex;
	mutex typeMutex;
	condition_variable messagesAvailable;
	condition_variable valueUpdated;
	hstring updateError = L"";

	// data
	hstring playerName = L"";
	hstring artist = L"";
	hstring album = L"";
	hstring title = L"";
	int number = 0;
	int year = 0;
	hstring genre = L"";
	hstring cover = L"";
	hstring file = L"";
	int duration = 0;
	hstring durationStr = L"";
	int position = 0;
	hstring positionStr = L"";
	int progress = 0;
	int rating = 0;
	int repeat = 0;
	int shuffle = 0;
	int state = 0;
	int status = 0;
	int volume = 0;

public:
	MediaPlayer(void*);
	~MediaPlayer();

	void Reload(PlayerType);
	double Update();
	LPCWSTR GetString();
	void ExecuteBang(LPCWSTR);

protected:
	void PushMessage(PlayerActions, int = 0, bool = false);
	void Runtime();
	void Initialize();
	void Deinitialize();
	bool ProcessMessages();
	void HandlePlayerAction(PlayerActions, int, bool);
	void UpdateData();
	void UpdateCover(hstring, MediaProperties);
	void UpdateValues(hstring, MediaProperties, PlaybackInfo, TimelineProperties);
	void ResetValues();

	int GetDuration(TimeSpan, TimeSpan);
	int GetPosition(TimeSpan);
	int GetState(PlaybackStatus);
	int GetStatus(PlaybackStatus);
	hstring JoinGenres(IVectorView<hstring>);
	hstring FormatTime(int);

	int ParseIntArg(wstring, int = 0);
};

