#pragma once
#ifndef __NOUN_STATS_H
#define __NOUN_STATS_H

#include "../com/arawak.h"
#include "../eng/bytes.h"

typedef uint32_t STATS;

enum statbytes { HP, SP, AP };

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


#endif
