#pragma once
#ifndef __MYMATH_H
#define __MYMATH_H


//static inline float norm(int val, int min, int max)
//{
        //return (float)((float)val / (float)(max - min));
//}


inline float fnorm(float val, float min, float max)
{
        return val / (max - min);
}


#endif
