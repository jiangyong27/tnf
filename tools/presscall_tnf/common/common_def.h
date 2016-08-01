#ifndef COMMON_UTIL_DEF_H
#define COMMON_UTIL_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include <sstream>
#include <map>
#include <set>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

/** 指针删除*/
#ifndef _DELETE
#define _DELETE(x)  if (x) {delete x;x=NULL;}
#endif


#ifndef _DELETE_ARR
#define _DELETE_ARR(x) if (x) {delete [] x; x=NULL;}
#endif

#ifndef TIME_DIFF_MS
#define TIME_DIFF_MS(a, b) (((a).tv_sec - (b).tv_sec) * 1000 + ((a).tv_usec - (b).tv_usec) / 1000)
#endif

#ifndef HHMap
#define HHMap std::tr1::unordered_map
#endif

#ifndef HHSet
#define HHSet std::tr1::unordered_set
#endif

template<typename T> std::string to_str(const T& t) {
    std::ostringstream s;
    s << t;
    return s.str();
}

template<typename T> T from_str(const std::string& s) {
    std::istringstream is(s);
    T t;
    is >> t;
    return t;
}


#endif
