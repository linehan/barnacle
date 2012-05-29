#pragma once
#ifndef __NOUN_STATS_H
#define __NOUN_STATS_H

#include "../com/barnacle.h"
#include "../eng/bytes.h"

typedef uint32_t STATS;

enum statbyte { HP, SP, AP };

#define STAT_SET(stat, byte, x) (set_byte((stat), (byte), (x)))
#define STAT_ADD(stat, byte, x) (add_byte((stat), (byte), (x)))
#define STAT_SUB(stat, byte, x) (sub_byte((stat), (byte), (x)))
#define STAT_INC(stat, byte)    (inc_byte((stat), (byte), (x)))
#define STAT_DEC(stat, byte)    (dec_byte((stat), (byte), (x)))

#define HP(stat)       (get_byte(stat, HP))
#define HP_SET(stat,x) (STAT_SET(stat, HP, (x)))
#define HP_SUB(stat,x) (STAT_SUB(stat, HP, (x)))
#define HP_ADD(stat,x) (STAT_ADD(stat, HP, (x)))
#define HP_INC(stat)   (STAT_INC(stat, HP))
#define HP_DEC(stat)   (STAT_DEC(stat, HP))

#define SP(stat)       (get_byte(stat, SP))
#define SP_SET(stat,x) (STAT_SET(stat, SP, (x)))
#define SP_SUB(stat,x) (STAT_SUB(stat, SP, (x)))
#define SP_ADD(stat,x) (STAT_ADD(stat, SP, (x)))
#define SP_INC(stat)   (STAT_INC(stat, SP))
#define SP_DEC(stat)   (STAT_DEC(stat, SP))

#define AP(stat)       (get_byte(stat, AP))
#define AP_SET(stat,x) (STAT_SET(stat, AP, (x)))
#define AP_SUB(stat,x) (STAT_SUB(stat, AP, (x)))
#define AP_ADD(stat,x) (STAT_ADD(stat, AP, (x)))
#define AP_INC(stat)   (STAT_INC(stat, AP))
#define AP_DEC(stat)   (STAT_DEC(stat, AP))

#define STAT_SET_ALL(stat, x, y, z) \
        HP_SET(stat, x);            \
        SP_SET(stat, y);            \
        AP_SET(stat, z)

#define AP_COLOR PUR_METALYELLOW
#define SP_COLOR PUR_DGREEN
#define HP_COLOR PUR_BRZ
#define AP_HI    PUR_YELLOW
#define SP_HI    PUR_GREY
#define HP_HI    PUR_RED


enum stat_trend { STAT_SAME, STAT_DOWN, STAT_UP };


static inline int stat_trend(uint32_t new, uint32_t old)
{
        enum stat_trend trend;
        
        if (new < old)
                trend = STAT_DOWN;
        else if (new > old)
                trend = STAT_UP;
        else
                trend = STAT_SAME;

        return (int)trend;
}

/**
 * stat_seek -- increment stat values up or down and indicate direction
 * @new : pointer to the new (i.e. target) stat word 
 * @old : pointer to the old (i.e. modified) stat word
 * @byte: the individual byte of the stat word (e.g. HP) to examine
 * 
 * Note that this function expects 'old' to be _retained_, such that
 * the value at *old will be modified through successive calls to this
 * function, until the trend becomes STAT_SAME. *new is the target that
 * *old is "chasing".
 */
static inline int stat_seek(uint32_t *new, uint32_t *old, enum statbyte byte)
{
        enum stat_trend trend;

        trend = stat_trend(get_byte(*new, byte), get_byte(*old, byte));

        if (trend == STAT_UP)
                inc_byte(old, byte);
        else if (trend == STAT_DOWN)
                dec_byte(old, byte);

        return trend;
}


#endif
