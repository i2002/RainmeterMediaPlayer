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
enum class PlayerActions
{
    FINALIZE,
    UPDATE_DATA,
    PAUSE,
    PLAY,
    PLAY_PAUSE,
    STOP,
    NEXT,
    PREVIOUS,
    OPEN_PLAYER,
    CLOSE_PLAYER,
    TOGGLE_PLAYER,
    SET_POSITION,
    SET_RATING,
    SET_SHUFFLE,
    SET_REPEAT,
    SET_VOLUME
};

enum class PlayerType
{
    UNKNOWN = -1,
    ARTIST,
    ALBUM,
    TITLE,
    NUMBER,
    YEAR,
    GENRE,
    COVER,
    FILE,
    DURATION,
    POSITION,
    PROGRESS,
    RATING,
    REPEAT,
    SHUFFLE,
    STATE,
    STATUS,
    VOLUME,
    UPDATE_TICK
};
