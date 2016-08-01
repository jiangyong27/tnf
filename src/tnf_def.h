#ifndef TNF_DEF_H
#define TNF_DEF_H
#pragma once

#include <stdint.h>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#ifndef _DELETE
#define _DELETE(x)  if (x) {delete x;x=NULL;}
#endif


#ifndef _DELETE_ARR
#define _DELETE_ARR(x) if (x) {delete [] x; x=NULL;}
#endif

#ifndef TIME_DIFF_MS
#define TIME_DIFF_MS(a, b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_usec - (b).tv_usec) / 1000)
#endif

#ifndef TIME_DIFF_US
#define TIME_DIFF_US(a, b) (((a).tv_sec - (b).tv_sec) * (uint64_t)1000000 + ((a).tv_usec - (b).tv_usec))
#endif


#ifndef HHMap
#define HHMap std::tr1::unordered_map
#endif

#ifndef HHSet
#define HHSet std::tr1::unordered_set
#endif


#endif // TNF_DEF_H
