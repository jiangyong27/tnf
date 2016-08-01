#ifndef CONVERTER_H
#define CONVERTER_H
#include <iconv.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
using namespace std;

class Convert
{
public:
    static int GBKToUtf8(const string& src,string *result);
    static int Utf8ToGBK(const string& src,string *result);
private:
    static int Utf8ToUnicode(char *utfStr,
                       const char *srcStr,
                       int maxUtfStrlen,
                       const char* fromLocale,
                       const char* toLocale,
                       char toAlter[],
                       int toAlter_len);
};

#endif
