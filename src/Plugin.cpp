// MediaPlayer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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
    hstring type_name = RmReadString(rm, L"PlayerType", L"");
    PlayerType t = PlayerType::UNKNOWN;

    if (type_name == L"")
        RmLog(rm, LOG_ERROR, L"[MediaPlayer.dll] PlayerType measure option not found");
    else if (type_name == L"Artist")
        t = PlayerType::ARTIST;
    else if (type_name == L"Album")
        t = PlayerType::ALBUM;
    else if (type_name == L"Title")
        t = PlayerType::TITLE;
    else if (type_name == L"Number")
        t = PlayerType::NUMBER;
    else if (type_name == L"Year")
        t = PlayerType::YEAR;
    else if (type_name == L"Genre")
        t = PlayerType::GENRE;
    else if (type_name == L"Cover")
        t = PlayerType::COVER;
    else if (type_name == L"File")
        t = PlayerType::FILE;
    else if (type_name == L"Duration")
        t = PlayerType::DURATION;
    else if (type_name == L"Position")
        t = PlayerType::POSITION;
    else if (type_name == L"Progress")
        t = PlayerType::PROGRESS;
    else if (type_name == L"Rating")
        t = PlayerType::RATING;
    else if (type_name == L"Repeat")
        t = PlayerType::REPEAT;
    else if (type_name == L"Shuffle")
        t = PlayerType::SHUFFLE;
    else if (type_name == L"State")
        t = PlayerType::STATE;
    else if (type_name == L"Status")
        t = PlayerType::STATUS;
    else if (type_name == L"Volume")
        t = PlayerType::VOLUME;
    else
        RmLogF(rm, LOG_ERROR, L"[MediaPlayer.dll] Unknown player action: %ls", t);

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