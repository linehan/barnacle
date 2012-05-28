#pragma once
#ifndef __FSM_STATES_H
#define __FSM_STATES_H

enum sm_tag {
        SM_Default,
        SM_Ready,

        SM_GoUp,
        SM_GoDown,
        SM_GoLeft,
        SM_GoRight,

        SM_RunUp,
        SM_RunDown,
        SM_RunLeft,
        SM_RunRight,

        SM_Attack,
        SM_GetHit,

        SM_Die,
        SM_Destroy,

        SM_DigUp,
        SM_DigDown,
        SM_DigLeft,
        SM_DigRight,

        SM_Seek,
        SM_Reset,

        SM_PickUp,

        SM_PokeUp,
        SM_PokeDown,
        SM_PokeLeft,
        SM_PokeRight,

        SM_DodgeUp,
        SM_DodgeDown,
        SM_DodgeLeft,
        SM_DodgeRight,

        SM_JumpUpLeft,
        SM_JumpUpRight
};

#endif
