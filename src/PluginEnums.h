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
    VOLUME
};
