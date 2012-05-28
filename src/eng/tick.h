#pragma once
#ifndef __ENG_TICK_H
#define __ENG_TICK_H
#include "../com/barnacle.h"


/* GAME LOOP TICK
``````````````````````````````````````````````````````````````````````````````*/
/*
 * When the game is running, the value of TICKER will be incremented by one
 * each time the render_cb() is called by the event loop.
 */
uint32_t TICKER;

/* tick -- increment the value of TICKER */
static inline void tick(void)
{
        TICKER++;
}
/* get_tick -- return the current value of TICKER */
static inline uint32_t get_tick(void)
{
        return (TICKER);
}



/* PAUSE LOOP TICK
``````````````````````````````````````````````````````````````````````````````*/
bool game_is_paused; /* Defines the game as 'paused' */
/*
 * When the game is paused, the value of PAUSE_TICKER will be incremented by 
 * one each time the pause_loop() is called by the event loop. In order for
 * the game to be paused, the boolean value game_is_paused must be set to
 * 'true'.
 */
uint32_t PAUSE_TICKER;

/* pause -- increment the value of PAUSE_TICKER instead of TICKER */
static inline void pause(void)
{
        PAUSE_TICKER++;
}

/* get_pause -- return the value of PAUSE_TICKER */
static inline uint32_t get_pause(void)
{
        return (PAUSE_TICKER);
}



#endif

