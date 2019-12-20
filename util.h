#ifndef _UTIL_H
#define _UTIL_H


inline float lerp(float lhs, float rhs, float percent)
{
   return lhs + (rhs - lhs) * percent;
}


#endif // _UTIL_H