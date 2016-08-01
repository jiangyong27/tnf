// Copyright (c) 2014, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-10-31
//
#include "string.h"

inline bool StringUtility::IsGbk(uint8_t high, uint8_t low) {
    return (high >= 0x81 && high <= 0xfe) &&
           ((low >= 0x40 && low <= 0x7e) || (low >= 0x80 && low <= 0xfe));
}

inline bool StringUtility::IsGbk(const char* word) {
    unsigned char c1 = static_cast<unsigned char>(*word);
    unsigned char c2 = static_cast<unsigned char>(*(word + 1));
    return IsGbk(c1, c2);
}


/** 把字符转为小写, 不用系统函数tolower是因为它有bug */
char StringUtility::CharToLower(char c)
{
	if (c >= 'A' && c <= 'Z')
		return c + 32;
	else
		return c;
}

/** 把字符转为大写, 不用系统函数toupper是因为它有bug */
char StringUtility::CharToUpper(char c)
{
	if (c >= 'a' && c <= 'z')
		return c - 32;
	else
		return c;
}

std::string StringUtility::FormatString(const char *fmt, ...)
{
    char buf[1024 * 10];
    uint32_t buf_len = 1024 * 10;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, buf_len, fmt, ap);
    va_end(ap);
    return buf;
}

std::string StringUtility::TrimBothSides(const std::string& strValue, const std::string& strTarget)
{
	if (strValue.size() <= 0)
		return "";

	size_t dwBeginPos, dwEndPos;
	size_t dwStrLen = strValue.length();

	for (dwBeginPos = 0 ; dwBeginPos < dwStrLen; dwBeginPos++)
	{
		// 如果该字符不在要删除的字符串中
		if (std::string::npos == strTarget.find(strValue.at(dwBeginPos)))
			break;
	}

	for (dwEndPos = dwStrLen - 1 ; dwEndPos > 0 ; dwEndPos--)
	{
		// 如果该字符不在要删除的字符串中
		if (std::string::npos == strTarget.find(strValue.at(dwEndPos)))
			break;
	}

	if (dwBeginPos > dwEndPos)
		return "";

	return strValue.substr(dwBeginPos, dwEndPos - dwBeginPos + 1);
}

std::string StringUtility::TrimLeft(const std::string& strValue, const std::string& strTarget)
{
	if (strValue.size() <= 0)
		return "";

	size_t dwBeginPos;
	size_t dwStrLen = strValue.length();
	for (dwBeginPos = 0 ; dwBeginPos < dwStrLen; dwBeginPos++)
	{
		// 如果该字符不在要删除的字符串中
		if (std::string::npos == strTarget.find(strValue.at(dwBeginPos)))
			break;
	}

	if (dwBeginPos == dwStrLen - 1)
		return "";

	return strValue.substr(dwBeginPos);
}

std::string StringUtility::TrimRight(const std::string& strValue, const std::string& strTarget)
{
	if (strValue.size() <= 0)
		return "";

	size_t dwEndPos;
	size_t dwStrLen = strValue.length();
	for (dwEndPos = dwStrLen - 1 ; dwEndPos > 0 ; dwEndPos--)
	{
		// 如果该字符不在要删除的字符串中
		if (std::string::npos == strTarget.find(strValue.at(dwEndPos)))
			break;
	}

	if (dwEndPos == 0)
		return "";

	return strValue.substr(0, dwEndPos + 1);
}


void StringUtility::SplitString(const std::string& strMain, const  std::string& strSpliter, std::vector<std::string>* strList)
{
	strList->clear();

	if (strMain.empty() || strSpliter.empty())
		return;

	// 从下面开始都使用处理后的string
    std::string strMainTemp = TrimBothSides(strMain, " \r\n\t");

	// 要划分的字符串或分界符为空，说明调用该函数不正确
	if (strMainTemp.empty() || strSpliter.empty())
		return;

	size_t nPos = strMainTemp.find(strSpliter);

    while (std::string::npos != nPos)
	{
		strList->push_back(strMainTemp.substr(0,nPos));
		strMainTemp.erase(0, nPos+strSpliter.length());
		strMainTemp = TrimBothSides(strMainTemp, " \r\n\t");
		nPos = strMainTemp.find(strSpliter);
	}

	strMainTemp = TrimBothSides(strMainTemp, " \r\n\t");

	// 最后一项要求不为空才加入集合
	if(!strMainTemp.empty())
		strList->push_back(strMainTemp);
}

/** 功能:  简单的分词函数：把一个字符串按分隔符号划分成多个字符串 */
void StringUtility::SplitString(const std::string& strMain, std::vector<std::string>* strList)
{
	strList->clear();
	if (strMain.empty())
		return;

    std::string strTemp = strMain;
    size_t   dwBeginPos;
    size_t    dwEndPos;
	do
	{
        dwBeginPos = 0;
        while(dwBeginPos < strTemp.size() && IsSeparator(strTemp[dwBeginPos]))
            dwBeginPos++;

        dwEndPos = dwBeginPos;
        while(dwEndPos < strTemp.size() && !IsSeparator(strTemp[dwEndPos]))
            dwEndPos++;

        if (dwEndPos > dwBeginPos)
        {
            strList->push_back(strTemp.substr(dwBeginPos, dwEndPos - dwBeginPos));
            strTemp = strTemp.substr(dwEndPos);
        }
        else
        {
            break;
        }
	}
    while ( strTemp.size() > 0);
}

/** 把字符串A中的子串B替换为C */
std::string StringUtility::ReplaceStr(const std::string& strValue, const std::string& oldStr, const std::string& newStr)
{
    std::string strRet = strValue;

	// 找到第一个Target
	size_t dwPos = strValue.find(oldStr);
	size_t dwNextPos;

    while (std::string::npos != dwPos)
	{
		dwNextPos = dwPos + oldStr.size();
		if (strRet.size() >= dwNextPos)
		{
			strRet.erase(dwPos, oldStr.size());
			strRet.insert(dwPos, newStr);
		}

		dwPos = strRet.find(oldStr, dwPos+newStr.size());
	}
	return strRet;
}

/** 把一个集合中的字符转换为对应的另一个集合中字符
 *  例如：把 <变为[, > 变为]
 *        strNew = ReplaceCharSet(strOld, "<>", "[]");
 */
std::string StringUtility::ReplaceCharSet(const std::string& strOld, const std::string& strFromSet, const std::string& strToSet)
{
	std::string strResult;
	for (unsigned int i = 0; i < strOld.size(); i++)
	{
		char ch = strOld[i];

		size_t dwPos = strFromSet.find(ch);
		if (dwPos == std::string::npos)
		{
			// 没有找到，说明不需要替换，直接复制过去
			strResult += ch;
		}
		else if (dwPos < strToSet.size())
		{
			// 找到，替换
			strResult += strToSet[dwPos];
		}
	}
	return strResult;

}

/** 替换字符串中的某些子字符串 */
std::string StringUtility::ReplaceStrSet(const std::string& strRawData, const std::map<std::string, std::string>& mSet)
{
    std::string strRet = strRawData;
	std::map<std::string, std::string>::const_iterator it;
	for (it = mSet.begin(); it != mSet.end(); it++)
	{
		strRet = ReplaceStr(strRet, it->first, it->second);
	}
	return strRet;
}

/** 把字符串A中的子符删除 */
std::string StringUtility::RemoveChar(const std::string& strValue, char ch)
{
    std::string strTmp = strValue;

	// 删除所有的字符ch
	strTmp.erase(std::remove(strTmp.begin(), strTmp.end(), ch), strTmp.end());

	return strTmp;
}

/** 把字符串A中的某个子字符串删除 */
std::string StringUtility::RemoveStr(const std::string& strRawData, const std::string& strSubStr, bool bNeedSeparator)
{
	int dwPos;
    std::string strTemp = strRawData;
    std::string strRet;

	do
	{
		dwPos = strTemp.find(strSubStr);
		if (dwPos == -1)
		{
			strRet += strTemp;
			break;
		}
        else
		{
            if (bNeedSeparator)
            {
                bool bHasPreSeparator = false;
                bool bHasPostSeparator = false;

                if (0 == dwPos)
                    bHasPreSeparator = true;
                else if (dwPos > 0 && IsSeparator(strTemp[dwPos-1]))
                    bHasPreSeparator = true;

                if (dwPos+strSubStr.size() >= strTemp.size())
                    bHasPostSeparator = true;
                else if (IsSeparator(strTemp[dwPos+strSubStr.size()]))
                    bHasPostSeparator = true;

                if (bHasPreSeparator && bHasPostSeparator)
                {
                    // 前后都有分界符，那么认为是可以删除的
			        strRet += strTemp.substr(0, dwPos);
			        strTemp = strTemp.substr(dwPos+strSubStr.size());
                }
                else
                {
                    // 如果前或后没有分界符，那么认为是不可以删除的
			        strRet += strTemp.substr(0, dwPos+strSubStr.size());
			        strTemp = strTemp.substr(dwPos+strSubStr.size());
                }
            }
            else
            {
			    strRet += strTemp.substr(0, dwPos);
			    strTemp = strTemp.substr(dwPos+strSubStr.size());
            }
		}
	}
	while (!strTemp.empty());
	return strRet;

}

/** 删除字符串中的某些子字符串 */
std::string StringUtility::RemoveStrSet(const std::string& strRawData, const std::set<std::string>& sStrSet, bool bNeedSeparator)
{
    std::set<std::string>::iterator it;
    std::string strTmp = strRawData;

    it = sStrSet.end();
    for(it--; it != sStrSet.begin(); it--)
        strTmp = RemoveStr(strTmp, *it, bNeedSeparator);

    if (it == sStrSet.begin())
        strTmp = RemoveStr(strTmp, *it, bNeedSeparator);
    return strTmp;
}

/** 删除一个字符串中的一段子字符串，包括strBegin和strEnd也会被删除 */
std::string StringUtility::DeleteSubstr(const std::string& strRawData, const std::string& strBegin, const std::string& strEnd)
{
	int dwPosBegin;
	int dwPosEnd;
    std::string strTemp = strRawData;
    std::string strRet;

	do
	{
		dwPosBegin = strTemp.find(strBegin);
		dwPosEnd = strTemp.find(strEnd);

        if ((dwPosBegin != -1) && (dwPosEnd != -1) && (dwPosEnd > dwPosBegin))
		{
            // 开始和结束字符串都找到的情况
			strRet += strTemp.substr(0, dwPosBegin);
			strTemp = strTemp.substr(dwPosEnd+strEnd.size());
			continue;
		}
        else
        {
            strRet += strTemp;
            break;
        }
	}
	while (!strTemp.empty());
	return strRet;
}

/** 从一个字符串中提取出关注的部分，例如"[*]" */
std::string StringUtility::ExtractSubstr(const std::string& strInput, const std::string& strPattern)
{
    // 如果参数为空，或者参数为'*'，或者在参数中没有找到*这个子串，则返回源串
    if ((strPattern.empty())  || ("*" == strPattern))
        return strInput;

    // 找到 strInput 中的*号所在的位置
    std::string::size_type pos = strPattern.find("*");
    if (std::string::npos == pos)
        return strInput;

    std::string strLeft  = strPattern.substr(0, pos);
    std::string strRight = strPattern.substr(pos + 1, strPattern.length() - pos + 1);

    // 在输入串中找*左边子串的位置
    std::string::size_type leftPos = 0;
    std::string::size_type rightPos =  strInput.length() - 1;

    if (!strLeft.empty())
    {
        leftPos = strInput.find(strLeft);
        if (std::string::npos != leftPos)
        {
            leftPos += strLeft.length();
        }
        else
        {
            leftPos = 0;
        }
    }

    if (!strRight.empty())
    {
        rightPos = strInput.rfind(strRight);
        if (std::string::npos != rightPos)
        {
            rightPos -= 1;
        }
        else
        {
            rightPos =  strInput.length() - 1;
        }
    }

    return strInput.substr(leftPos, rightPos - leftPos + 1);
}

/** 把字符串中一些需要转义的字符做转义处理 */
std::string StringUtility::QuoteWild(const std::string& strRaw)
{
	if (0 == strRaw.size())
		return "";

	char* pcDest = new char[(strRaw.size())*2];
  	int i = 0;
	int j = 0;

	while (strRaw[i])
	{
		switch((unsigned char)strRaw[i])
		{
			case '\n':
				pcDest[j++] = '\\';
				pcDest[j++] = 'n';
				break;
			case '\t':
				pcDest[j++] = '\\';
				pcDest[j++] = 't';
				break;
			case '\r':
				pcDest[j++] = '\\';
				pcDest[j++] = 'r';
				break;
			case '\b':
				pcDest[j++] = '\\';
				pcDest[j++] = 'b';
				break;
			case '\'':
				pcDest[j++] = '\\';
				pcDest[j++] = '\'';
				break;
			case '\"':
				pcDest[j++] = '\\';
				pcDest[j++] = '\"';
				break;
			case '\\':
				pcDest[j++] = '\\';
				pcDest[j++] = '\\';
				break;
			default:
				// 小于32是控制字符，都转换为空格(32)
				if ((unsigned char)strRaw[i] >= 32)
					pcDest[j++] = strRaw[i];
				else
					pcDest[j++] =32;
		}
		i++;
	}
	pcDest[j] = 0;

    std::string strRet;
	strRet.assign(pcDest);
	delete [] pcDest;

	return strRet;
}


/** 把字符串中一些特殊字符转换成空格, 去掉两边的无效字符，并转换为小写 */
std::string StringUtility::ConvertToSpace(const std::string& str)
{
	static char pc[] = "1111111111111111111111111111111111100011111111110000000000111010"
					   "100000000000000000000000000111011000000000000000000000000000100";
	int len = str.size() * 2;
    std::string strRet;
	if ( len == 0 ) return strRet;

	char * pDest = new char[len * 2];
	size_t i = 0, j = 0;
	while (i < str.size())
	{
		unsigned char a = (unsigned char)str[i++];
		if (a<128)
		{
			if (pc[a] == '1')   //转换为空格，如果是连续个空格，则不管
			{
				if (j>0 && pDest[j-1] != ' ') pDest[j++] = ' ';
			}
			else pDest[j++] = CharToLower( a );
			continue;
		}

		//multibyte char
		if (i >= str.size()) break;

		unsigned char b = (unsigned char)str[i++];
		if ((a==0xa1 && b>=0xa1) ||				//、。·‘’“”《》【】′
			(a==0xa3 && b>=0xa1 && b<=0xc0) ||	//！＂＃￥％＆＇（）＊＋，－．／０１２３４５６８９：；＜＝＞？＠
			(a==0xa9 && b>=0x6f))				//；﹒
		{
			if (j>0 && pDest[j-1] != ' ')  pDest[j++] = ' ';
			continue;
		}
		pDest[j++] = a;
		pDest[j++] = b;
	}
	if (j > 0 && pDest[j-1] == ' ') j--;  //如果最后一个字符为空格，则去掉
	strRet.assign( pDest, j);
	delete pDest;
	return strRet;
}

// 字符串内容判断

bool StringUtility::CmpNoCase(char c1, char c2)
{
	return toupper(c1) == toupper(c2);
}

/** 在字符串中查找子符串，不区分大小写 */
std::string::size_type StringUtility::FindNoCase(const std::string& strMain, const std::string& strSub, int nFrom)
{
    std::string::const_iterator pos;
	pos = search(strMain.begin()+nFrom, strMain.end(),
			strSub.begin(),strSub.end(), CmpNoCase);

	if( pos == strMain.end())
	{
		return std::string::npos;
	}
	else
	{
		return (pos - strMain.begin());
	}
}

/** 检测字符串中是否包含了某些子字符串 */
bool StringUtility::IsSubStrContained(const std::string& strRawData, const std::set<std::string>& sSubStr)
{
    std::set<std::string>::const_iterator it;
	for (it = sSubStr.begin(); it != sSubStr.end(); it++)
	{
		if (std::string::npos != strRawData.find(*it))
			return true;
	}

	return false;
}

/** 返回当前位置开始的下一个字符 */
std::string StringUtility::GetNextChar(const std::string& str, size_t dwCurPos)
{
	if(str.size() < dwCurPos+1)
		return "";
	if(str.at(dwCurPos) < 0)
	{
		return str.substr(dwCurPos, 2);
	}
	else
	{
		return str.substr(dwCurPos, 1);
	}
}

/** 获得一个字符串的字符个数, 中文等双字节字符只算1个 */
int StringUtility::GetCharCount(const std::string& strWord)
{
	int dwCharCount = 0;
	for (size_t i = 0; i < strWord.size(); ++i, ++dwCharCount)
	{
		if (strWord[i] < 0)
		{
			++i;
		}
	}
	return dwCharCount;
}

/** 是否是数字IP */
bool StringUtility::IsDigitIp(const std::string& strIp)
{
    std::vector<std::string> strList;
	int dwTmp;

	SplitString(strIp, ".", &strList);
    if (4 != strList.size())
    	return false;

    for(int i = 0; i < 4; i++)
    {
    	if (strList[i].size() > 3 || strList[i].size() == 0 )
    		return false;

		for (size_t j = 0; j < strList[i].size(); j++)
		{
			if (!(strList[i][j] >= '0' && strList[i][j] <= '9'))
				return false;
    	}
    	dwTmp = Str2Int(strList[i]);
    	if (dwTmp > 255)
    		return false;
    }
	return true;
}

/**
* 判断一个字符串是不是只是包含空格字符,检查的空格字符包括' ', '\t'
* @param   pStr 要检查的字符串
* @return  bool 如果被检查字符串只是包含空格字符,就返回PORT_TRUE,
*          否则返回PORT_FALSE
*/
bool StringUtility::IsWhiteString(const std::string& pStr)
{
    size_t i = 0;
    while((pStr[i] == ' ')|| (pStr[i] == '\t')) {
        i++;
        if (i >= pStr.size())
            break;
    }
    if(i >= pStr.size())
        return true;
    else
        return false;
}

/** 判断一个字符串是否是完全由英文字符串组成 */
bool StringUtility::IsEnglishString(const std::string& pStr, bool bLowLineAsAlpha)
{
	char* p = (char*)pStr.c_str();
    while (*p != '\0')
	{
        if (bLowLineAsAlpha)
        {
            if( !IsAlpha(*p) && ('_' != *p))
                return false;
        }
        else
        {
            if( !IsAlpha(*p))
                return false;
        }
		p++;
	}

    return true;
}

/** 判断一个字符串是否全是数字组成 */
bool StringUtility::IsDigitString(const std::string& pStr)
{
	char* p = (char*)pStr.c_str();
    while (*p != '\0')
	{
        if( !IsDigit(*p))
            return false;
		p++;
	}

    return true;
}

bool StringUtility::IsEnglishOrDigitString(const std::string& str)
{
	char* p = (char*)str.c_str();
    while (*p != '\0')
	{
        if( !IsDigit(*p) && !IsAlnum(*p))
            return false;
		p++;
	}

    return true;
}


/** 计算GB2312汉字的序号,返回值是0到6767之间的数 */
int StringUtility::GetGB2312HanziIndex(const std::string& str)
{
	return ((unsigned char)str[0] - 176)*94 + (unsigned char)str[1] - 161;
}

/** 判断一个字符串的前两个字节是否是GB2312汉字 */
bool StringUtility::IsGB2312Hanzi(const std::string& str)
{
	if(str.size() < 2)
		return false;
	int dwIndex = GetGB2312HanziIndex(str);
	if (dwIndex >=0 && dwIndex < 6768)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/** 判断一个字符串是否完全由GB2312的汉字组成 */
bool StringUtility::IsOnlyComposedOfGB2312Hanzi(const std::string& strWord)
{
	//如果字符串的长度不是2的倍数,则返回false
	if (strWord.size()%2 != 0)
	{
		return false;
	}

    std::string strTmp;

	//逐个判断每两个字节是否组成一个汉字
	for (size_t i = 0; i+1 < strWord.size(); i=i+2)
	{
        strTmp = strWord.substr(i);
		if (!IsGB2312Hanzi(strTmp))
		{
			return false;
		}
	}
	return true;
}

/** 判断字符是否是空格，windows下的isspace()有bug */
bool StringUtility::IsSpace(char c)
{
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        return true;
    else
        return false;
}

/** 判断字符是否是数字或字母，windows下的isalnum()有bug */
bool StringUtility::IsAlnum(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
        return true;
    else
        return false;
}

/** 判断字符是否是数字，windows下的isdigit()有bug */
bool StringUtility::IsDigit(char c)
{
    if (c >= '0' && c <= '9')
        return true;
    else
        return false;
}

/** 判断字符是否是字母，windows下的isalpha()有bug */
bool StringUtility::IsAlpha(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;
    else
        return false;
}

/** 判断字符是否是分界符 */
bool StringUtility::IsSeparator(char c)
{
    std::string strSeparator(" \t\r\n-_[](){}:.,=*&^%$#@!~?<>/|'\"");
    if (std::string::npos == strSeparator.find(c))
        return false;
    else
        return true;
}


/** 把字符串转为小写 */
std::string StringUtility::ToLower(const std::string& pszValue)
{
    std::string  strRet;
    size_t dwLen = pszValue.size();
    for (unsigned long i = 0 ; i < dwLen; i ++)
    {
        if ((i+1) < dwLen && IsGbk(pszValue[i], pszValue[i+1])) {
            strRet += pszValue[i];
            strRet += pszValue[i+1];
            i++;
            continue;
        }
        strRet += CharToLower(pszValue[i]);
    }

    return strRet;
}

/** 把字符串转为大写 */
std::string StringUtility::ToUpper(const std::string& pszValue)
{
    std::string  strRet;
    size_t dwLen = pszValue.size();
    for (unsigned long i = 0 ; i < dwLen; i ++)
    {
        if ((i+1) < dwLen && IsGbk(pszValue[i], pszValue[i+1])) {
            strRet += pszValue[i];
            strRet += pszValue[i+1];
            i++;
            continue;
        }
        strRet += CharToUpper(pszValue[i]);
    }

    return strRet;
}

/** 把一个int值转换成为字符串*/
std::string StringUtility::SignedInt2Str(int dwValue)
{
    char szBuf[16];
    sprintf(szBuf,"%d",dwValue);
    return std::string(szBuf);
}

/** 把一个unsigned int值转换成为字符串*/
std::string StringUtility::Int2Str(unsigned int  dwValue)
{
    char szBuf[16];
    sprintf(szBuf,"%u",dwValue);
    return std::string(szBuf);
}

/** 把浮点数转为字符串 */
std::string StringUtility::Double2Str(double lfValue)
{
    char szBuf[80];
    sprintf(szBuf, "%lf", lfValue);
    return std::string(szBuf);
}

/** 把一个字符转换成16进制的形式 */
std::string StringUtility::Char2Hex(unsigned char ch)
{
	std::string str("%");
	char high;
	char low;

	high = ch / 16;
	if(high > 9)
		high = 'A' + high - 10;
	else
		high = '0' + high;

	low  = ch % 16;
	if(low > 9)
		low = 'A' + low - 10;
	else
		low = '0' + low;

	str += high;
	str += low;

	return str;
}


/** 把字符串转为整型数 */
int StringUtility::Str2Int(const std::string& str)
{
	int dwRet = 0;

	for (size_t i = 0; i < str.size(); ++i)
		dwRet = dwRet*10 + str[i] - '0';

	return dwRet;
}

/** 把字符串转为整型数 */
unsigned int StringUtility::Str2UInt(const std::string& str)
{
	unsigned int dwRet = 0;

	for (size_t i = 0; i < str.size(); ++i)
		dwRet = dwRet*10 + str[i] - '0';

	return dwRet;
}

//to long long
unsigned long long StringUtility::Str2ULongLong(const std::string& str)
{
    unsigned long long dwRet = 0;

    for (size_t i = 0; i < str.size(); ++i)
        dwRet = dwRet*10 + str[i] - '0';

    return dwRet;
}

/** 把字符串转为浮点数 */
double StringUtility::Str2Double(const std::string& str)
{
	// atof在windows和linux上定义相同
	return atof(str.c_str());
}

/** 把16进制的字符串转为整型数 */
int StringUtility::HexStr2long(const std::string& pcHex)
{
	int dwRet = 0;
	int dwTmp;
	size_t dwLen = pcHex.size();

	for (size_t i = 0; i < dwLen; ++i)
	{
		if (pcHex[i] >= 'A' && pcHex[i] <= 'F')
			dwTmp = pcHex[i] - 'A' + 10;
		else if (pcHex[i] >= 'a' && pcHex[i] <= 'f')
			dwTmp = pcHex[i] - 'a' + 10;
		else
			dwTmp = pcHex[i] - '0';

		dwRet = dwRet*16 + dwTmp;
	}
	return dwRet;
}

/** 把一个字符串中的连续空格转为单个空格 */
std::string StringUtility::SingleBlank(const std::string &strRaw)
{
	unsigned int dwCount = 0;
	bool bIsFirstSpace = true;
	const char *ptr = strRaw.c_str();

    std::string strRet(strRaw.length(), ' ');

	// 跳过字符串开始的空格
	while ((*ptr > 0) && isspace(*ptr)) ++ptr;

	while (*ptr)
	{
		if ((*ptr > 0) && isspace(*ptr))
		{
			if (bIsFirstSpace)
			{
				bIsFirstSpace = false;
				strRet[dwCount++] = ' ';
			}
		}
		else
		{
			bIsFirstSpace = true;
			strRet[dwCount++] = *ptr;
		}

		++ptr;
	}

	// 删除字符串结束的空格
    std::string::size_type a;
	a = strRet.find_last_not_of(' ', dwCount);
	if (a != std::string::npos)
		strRet.erase(a+1);
	else
	{
		a = 0;
		strRet.erase(a);
	}

	return strRet;
}

/** 删除html或xml格式的注释 <!-- --> */
std::string StringUtility::StripComments(const std::string& strRawFile)
{
    std::string strNewFile;
	strNewFile.reserve(strRawFile.size());

	const char *ptr = strRawFile.c_str();
	const char *end = ptr + strRawFile.length();

	bool bIsInsideComment = false;
	while(1)
	{
		if(!bIsInsideComment)
		{
			if(ptr  + 4 < end)
			{
				if(*ptr == '<' && *(ptr+1) == '!' && *(ptr+2) =='-' && *(ptr + 3) == '-' && isspace(*(ptr + 4)))
				{
					bIsInsideComment = true;
				}
			}
		}
		else
		{
			if(ptr + 2 < end)
			{
				if(*ptr == '-' && *(ptr+1) == '-' && *(ptr+2) == '>' )
				{
					bIsInsideComment = false;
					ptr += 3;
				}
			}
		}
		if(ptr == end)
			break;
		if(!bIsInsideComment)
			strNewFile += *ptr;
		ptr++;
	}

	strNewFile.resize(strNewFile.size());

	return strNewFile;
}

/** 计算一个字符串的hash值 */
unsigned int StringUtility::GetStrHashValue(const std::string& pcStr)
{
    unsigned int dwMagic = 31;
    const unsigned char* p = (const unsigned char*) pcStr.c_str();
    unsigned int h = 0;
    for (; *p != '\0'; ++p)
    {
        h = h * dwMagic + *p;
    }
    return h;
}

// 字符串模糊匹配实现（就是简单的正则表达式以及范围匹配功能）

/** 检查一个字符串是否能匹配到一个通配符号，外部使用接口
 *  MatchWildcard("he*o","hello"): true
 *  MatchWildcard("he*p","hello"): false
 *  MatchWildcard("he??o","hello"): true
 *  MatchWildcard("he?o","hello"): false
 *  MatchWildcard("[a-z][0-9]","h7"): true
 *  MatchWildcard("172.16.*","172.16.68.29"): true
 */
bool StringUtility::MatchWildcard(const std::string& strWild, const std::string& strMatch)
{
  return _MatchRemainder(strWild, strWild.begin(), strMatch, strMatch.begin());
}

// 计算一个字符是否在一个集合中，例如'8'属于"0-9"， 内部函数
bool StringUtility::_MatchSet(const std::string& strPattern, char strMatch)
{
	// 实际的字符集，例如:strPattern为"a-z", 那么strRealCharSet是"abcd...z"
    std::string strRealCharSet;
    std::string::const_iterator i;
	for (i = strPattern.begin(); i != strPattern.end(); ++i)
	{
		switch(*i)
		{
		case '-':
		{
			if (i == strPattern.begin())
				strRealCharSet += *i;
			else if (i+1 == strPattern.end())
		  		return false;
		    else
			{
				// 第一个字符已经在字符集中，所以首先删除(集合可能是空的)
				strRealCharSet.erase(strRealCharSet.end()-1);
				char last = *++i;
				for (char ch = *(i-2); ch <= last; ch++)
				{
				  strRealCharSet += ch;
				}
			}
			break;
		}
		case '\\':
			if (i+1 == strPattern.end())
				return false;
			strRealCharSet += *++i;
			break;
		default:
			strRealCharSet += *i;
			break;
		}
	}
    std::string::size_type dwResult = strRealCharSet.find(strMatch);
	return dwResult != std::string::npos;
}

// 匹配剩下的通配符，递归调用, 内部函数
bool StringUtility::_MatchRemainder(const std::string& strWild,
							std::string::const_iterator itWild,
							const std::string& strMatch,
							std::string::const_iterator itMatch)
{
#ifdef _DEBUG_
    std::cout << "MatchRemainder called at " << *itMatch << " with wildcard " << *itWild << std::endl;
#endif
	while (itWild != strWild.end() && itMatch != strMatch.end())
	{
#ifdef _DEBUG_
        std::cout << "trying to strMatch " << *itMatch << " with wildcard " << *itWild << std::endl;
#endif
		switch(*itWild)
		{
		case '*':
		{
			++itWild;
			++itMatch;
			for (std::string::const_iterator i = itMatch; i != strMatch.end(); ++i)
			{
				// 处理"*"在通配符的结束处的情况，这种情况下就没有剩下的了
				if (itWild == strWild.end())
				{
				  if (i == strMatch.end()-1)
					return true;
				}
				else if (_MatchRemainder(strWild, itWild, strMatch, i))
				{
				  return true;
				}
			} // end for
			return false;
		} // end case '*'
		case '[':
		{
			// 找到集合的结束处
			bool bFound = false;
            std::string::const_iterator it = itWild + 1;
			for (; !bFound && it != strWild.end(); ++it)
			{
				switch(*it)
				{
				case ']':
				{
					// 拿字符*itMatch到集合"[...]"中去匹配（括号被去掉）
					if (!_MatchSet(strWild.substr(itWild - strWild.begin() + 1, it - itWild - 1), *itMatch))
						return false;
					bFound = true;
					break;
				}
				case '\\':

					// 转义字符不能在结尾
					if (it == strWild.end()-1)
						return false;
					++it;
					break;
				default:
					break;
				}
			} // end for
			if (!bFound)
				return false;
			++itMatch;
			itWild = it;
			break;
		} // end case '['
		case '?':
			++itWild;
			++itMatch;
			break;
		case '\\':
			if (itWild == strWild.end()-1)
				return false;
			++itWild;
			if (*itWild != *itMatch)
				return false;
			++itWild;
			++itMatch;
			break;
		default:
			if (*itWild != *itMatch)
				return false;
			++itWild;
			++itMatch;
			break;
		} // end switch
	} // end while

	return (itWild == strWild.end()) && (itMatch == strMatch.end());
}

/** 字符串摘要：取前面若干个字符，主要是防止出现半个汉字和英文以及数字的截断　*/
std::string StringUtility::AbstractString(const std::string& strOldStr, int dwLen)
{
    if (dwLen <= 0)
    {
        return "";
    }
    else if (strOldStr.empty())
    {
        return "";
    }
    else if ((std::string::size_type)dwLen >= strOldStr.size())
    {
        return strOldStr;
    }

    int dwOffset = 0;
    while(dwOffset <= dwLen)
    {
        if(strOldStr[dwOffset] < 0)
        {
            // 处理中文
            if (dwOffset+2 <= dwLen)
                dwOffset += 2;
            else  // 容纳不小
                break;
        }
        else // 下面是ascii字符处理
        {
            if (IsAlnum(strOldStr[dwOffset]))
            {
                // 检查该行是否可以显示下该数字或字母
                int dwWordLen = 1;
                while(IsAlnum(strOldStr[dwOffset+dwWordLen]))
                    dwWordLen++;

                // 该词的长度大于摘要的长度，那么不得不截断处理
                if (dwWordLen > dwLen)
                {
                    dwOffset = dwLen;
                    break;
                }

                if (dwOffset+dwWordLen <= dwLen)
                    dwOffset += dwWordLen;
                else
                    break;
            }
            else
            {
                // 非控制字符\数字\字母之外的可以分开
                if (dwOffset+1 <= dwLen)
                    dwOffset++;
                else
                    break;
            } // end else
        }
    } // end while

    // 集中在这里返回是为了避免多个出口
    //strLeftStr = strOldStr.substr(dwOffset);
    return strOldStr.substr(0, dwOffset);
}

