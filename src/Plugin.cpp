/* Copyright (C) 2021 Tudor Butufei
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include "Windows.h"
#include "../API/RainmeterAPI.h"
#include "MediaPlayer.h"

PLUGIN_EXPORT void Initialize(void** data, void* rm)
{
    MediaPlayer* player = new MediaPlayer(rm);
    *data = player;
}

PLUGIN_EXPORT void Reload(void* data, void* rm, double* maxValue)
{
    MediaPlayer* player = (MediaPlayer*)data;
    wstring type_name = wstring(RmReadString(rm, L"PlayerType", L""));
    transform(type_name.begin(), type_name.end(), type_name.begin(), towlower);
    PlayerType t = PlayerType::UNKNOWN;

    if (type_name == L"")
        RmLog(rm, LOG_ERROR, L"[MediaPlayer.dll] PlayerType measure option not found");
    else if (type_name == L"artist")
        t = PlayerType::ARTIST;
    else if (type_name == L"album")
        t = PlayerType::ALBUM;
    else if (type_name == L"title")
        t = PlayerType::TITLE;
    else if (type_name == L"number")
        t = PlayerType::NUMBER;
    else if (type_name == L"year")
        t = PlayerType::YEAR;
    else if (type_name == L"genre")
        t = PlayerType::GENRE;
    else if (type_name == L"cover")
        t = PlayerType::COVER;
    else if (type_name == L"file")
        t = PlayerType::FILE;
    else if (type_name == L"duration")
        t = PlayerType::DURATION;
    else if (type_name == L"position")
        t = PlayerType::POSITION;
    else if (type_name == L"progress")
        t = PlayerType::PROGRESS;
    else if (type_name == L"rating")
        t = PlayerType::RATING;
    else if (type_name == L"repeat")
        t = PlayerType::REPEAT;
    else if (type_name == L"shuffle")
        t = PlayerType::SHUFFLE;
    else if (type_name == L"state")
        t = PlayerType::STATE;
    else if (type_name == L"status")
        t = PlayerType::STATUS;
    else if (type_name == L"volume")
        t = PlayerType::VOLUME;
#ifdef _DEBUG
    else if (type_name == L"updatetick")
        t = PlayerType::UPDATE_TICK;
#endif
    else
        RmLogF(rm, LOG_ERROR, L"[MediaPlayer.dll] Unknown player action: %ls", type_name.c_str());

    if(t == PlayerType::PROGRESS || t == PlayerType::VOLUME)
        *maxValue = 100.0;

    player->Reload(t);
}

PLUGIN_EXPORT double Update(void* data)
{
    MediaPlayer* player = (MediaPlayer*)data;
    return player->Update();
}

PLUGIN_EXPORT LPCWSTR GetString(void* data)
{
    MediaPlayer* player = (MediaPlayer*)data;
    return player->GetString();
}

PLUGIN_EXPORT void ExecuteBang(void* data, LPCWSTR args)
{
    MediaPlayer* player = (MediaPlayer*)data;
    player->ExecuteBang(args);
}

PLUGIN_EXPORT void Finalize(void* data)
{
    MediaPlayer* player = (MediaPlayer*)data;
    delete player;
}