// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-10-31
//
#ifndef _DATA_MTX_TEST_STR_H
#define _DATA_MTX_TEST_STR_H
#pragma once

#include <stdio.h>
#include <string.h>

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdarg.h>


class StringUtility
{
public:
    static inline bool IsGbk(uint8_t high, uint8_t low);
    static inline bool IsGbk(const char* word);

    static std::string FormatString(const char *fmt, ...);
    static std::string TrimBothSides(const std::string& strValue, const std::string& strTarget = " \t\r\n");
    static std::string TrimLeft(const std::string& strValue, const std::string& strTarget = " \t\r\n");
    static std::string TrimRight(const std::string& strValue, const std::string& strTarget = " \t\r\n");

    static void SplitString(const std::string& strMain, const  std::string& strSpliter, std::vector<std::string>* strList);
    static void SplitString(const std::string& strMain, std::vector<std::string>* strList);

    static std::string ReplaceStr(const std::string& strValue, const std::string& oldStr, const std::string& newStr);
	static std::string ReplaceCharSet(const std::string& strOld, const std::string& strFromSet, const std::string& strToSet);
	static std::string ReplaceStrSet(const std::string& strRawData, const std::map<std::string, std::string>& mSet);

    static std::string RemoveChar(const std::string& strRawData, char ch);
    static std::string RemoveStr(const std::string& strRawData, const std::string& strSubStr, bool bNeedSeparator = false);
	static std::string RemoveStrSet(const std::string& strRawData, const std::set<std::string>& sStrSet, bool bNeedSeparator = false);

	static std::string DeleteSubstr(const std::string& strRawData, const std::string& strBegin, const std::string& strEnd);
    static std::string ExtractSubstr(const std::string& strInput, const std::string& strPattern);
	static std::string QuoteWild(const std::string& strRaw);

    static std::string AbstractString(const std::string& strOldStr, int dwLen);
	static std::string ConvertToSpace(const std::string& strInput);
	static bool CmpNoCase(char c1, char c2);
    static std::string::size_type FindNoCase(const std::string& strMain, const std::string& strSub, int dwFrom);
    inline static int GetCharCount(const std::string& strWord);
    static std::string GetNextChar(const std::string& str, size_t dwCurPos);

    static bool IsSubStrContained(const std::string& strRawData, const std::set<std::string>& sSubStr);
    static bool IsDigitIp(const std::string& strIp);
    static bool IsWhiteString(const std::string& pStr);
    static bool IsEnglishString(const std::string& pStr, bool bLowLineAsAlpha = false);
    static bool IsDigitString(const std::string& pStr);
    static int GetGB2312HanziIndex(const std::string& str);
    static bool IsGB2312Hanzi(const std::string& str);
    static bool IsEnglishOrDigitString(const std::string& str);
    static bool IsOnlyComposedOfGB2312Hanzi(const std::string& strWord);

    inline static bool IsSpace(char c);
    inline static bool IsAlnum(char c);
    inline static bool IsDigit(char c);
    inline static bool IsAlpha(char c);
    inline static bool IsSeparator(char c);
    inline static char CharToLower(char c);
    inline static char CharToUpper(char c);

    static std::string ToLower(const std::string& pszValue);
    static std::string ToUpper(const std::string& pszValue);
    static std::string SignedInt2Str(int dwValue);
    static std::string Int2Str(unsigned int  dwValue);
    static int Str2Int(const std::string& str);
    static unsigned int Str2UInt(const std::string& str);
    static unsigned long long Str2ULongLong(const std::string& str);
    static double Str2Double(const std::string& str);
    static std::string Double2Str(double);
    static int HexStr2long(const std::string& pcHex);
    static void Str2HexStr(char* pcHexStr, unsigned char * p,int len);
    static std::string Char2Hex(unsigned char ch);

    static std::string SingleBlank(const std::string& strRaw);
	static std::string StripComments(const std::string& strRawFile);
    static unsigned int GetStrHashValue(const std::string& pcStr);
    static bool MatchWildcard(const std::string& strWild, const std::string& strMatch);

private:
    static bool _MatchSet(const std::string& strPattern, char strMatch);
    static bool _MatchRemainder(const std::string& strWild,
							    std::string::const_iterator itWild,
							    const std::string& strMatch,
							    std::string::const_iterator itMatch);
};



#endif // _DATA_MTX_TEST_STR_H
