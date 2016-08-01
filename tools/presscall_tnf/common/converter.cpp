#include "converter.h"
/*
Convert::Convert()
{
    m_utf8_to_gbk = iconv_open("GBK","UTF-8");
    m_gbk_to_utf8 = iconv_open("UTF-8","GBK");
}

Convert::~Convert()
{
    iconv_close(m_utf8_to_gbk);
    iconv_close(m_gbk_to_utf8);
}

*/
int Convert::Utf8ToUnicode(char *utfStr,
                       const char *srcStr,
                       int maxUtfStrlen,
                       const char* fromLocale,
                       const char* toLocale,
                       char toAlter[],
                       int toAlter_len)
{
	if(NULL==srcStr)
	{
		//printf("NULL Bad Parameter\n");
		return -1;
	}

	//首先先将gbk编码转换为unicode编码
	if(NULL==setlocale(LC_ALL,fromLocale))//设置转换为unicode前的码,当前为gbk编码
	{
		//printf("setlocale fromLocale Bad Parameter\n");
		snprintf(toAlter,toAlter_len,"%s",srcStr);//保留原字符
		//cout << toAlter << endl;
		return -2;
	}

	int unicodeLen=mbstowcs(NULL,srcStr,0);//计算转换后的长度
	if(unicodeLen<=0)
	{
		//printf("mbstowcs Can not Transfer!\n");
		snprintf(toAlter,toAlter_len,"%s",srcStr);//保留原字符
		//cout << toAlter << endl;
		return -3;
	}
	wchar_t *unicodeStr=(wchar_t *)calloc(sizeof(wchar_t),unicodeLen+1);
	mbstowcs(unicodeStr,srcStr,strlen(srcStr));//将utf8转换为unicode
	snprintf(toAlter,toAlter_len,"&#%d;",unicodeStr[0]);

	//将unicode编码转换为utf8编码
	if(NULL==setlocale(LC_ALL,toLocale))//设置unicode转换后的码,当前为gbk
	{
		//printf("setlocale 2 Bad Parameter\n");
		return -4;
	}
	int utfLen=wcstombs(NULL,unicodeStr,0);//计算转换后的长度
	if(utfLen<=0)
	{
		//printf("wcstombs Can not Transfer!!!\n");
		return -5;
	}
	else if(utfLen>=maxUtfStrlen)//判断空间是否足够
	{
		//printf("Dst Str memory not enough\n");
		return -6;
	}
	wcstombs(utfStr,unicodeStr,utfLen);
	utfStr[utfLen]=0;//添加结束符
	free(unicodeStr);
	return utfLen;
}

int Convert::Utf8ToGBK(const string &strUtf8In,string *strGbkOut)
{
	char toAlter[50] = {0};
	string strHandle = "";
	*strGbkOut = "";
	const char* fromLocale = "zh_CN.utf8";
	const char* toLocale = "zh_CN.gbk";

	char *des = (char*)malloc(10);

	for(size_t i=0; i<strUtf8In.size();)
	{
		if((strUtf8In[i] & 0xf0) == 0xe0)//three-byte char
		{
			strHandle = strUtf8In.substr(i,3);
			int r = Utf8ToUnicode(des,(char*)strHandle.c_str(),20,fromLocale,toLocale,toAlter,sizeof(toAlter));
			if(r<=0)
			{
				*strGbkOut+=toAlter;
				//cout << toAlter <<endl;
			}
			else
			{
				//cout << r << endl;
				//cout << des << endl;
				*strGbkOut+=des;
			}
			i = i+3;
		}
		else
		{
			if(0 == (strUtf8In[i] & 0x80))//single-byte char
			{
				strHandle = strUtf8In.substr(i,1);
				int r = Utf8ToUnicode(des,(char*)strHandle.c_str(),20,fromLocale,toLocale,toAlter,sizeof(toAlter));
				if(r<=0)
				{
					*strGbkOut+=toAlter;
					//cout << toAlter <<endl;
				}
				else
				{
					//cout << des << endl;
					*strGbkOut+=des;
				}
				i=i+1;
			}
			else
			{
				if(0xc0 == (strUtf8In[i] & 0xe0))//two-byte char
				{
					strHandle = strUtf8In.substr(i,2);
					int r = Utf8ToUnicode(des,(char*)strHandle.c_str(),20,fromLocale,toLocale,toAlter,sizeof(toAlter));
					if(r<=0)
					{
						*strGbkOut+=toAlter;
						//cout << toAlter <<endl;
					}
					else
					{
						//cout << des << endl;
						*strGbkOut+=des;
					}
					i=i+2;
				}
				else
				{
					if((strUtf8In[i] & 0xf0) == 0xf0 &&(strUtf8In[i] & 0x08) == 0)//four-byte char
					{
						strHandle = strUtf8In.substr(i,4);
						int r = Utf8ToUnicode(des,(char*)strHandle.c_str(),20,fromLocale,toLocale,toAlter,sizeof(toAlter));
						if(r<=0)
						{
							*strGbkOut+=toAlter;
							//cout << toAlter <<endl;
						}
						else
						{
							//cout << des << endl;
							*strGbkOut+=des;
						}
						i=i+4;
					}
					else
					{
						//printf("error:{invalid utf8 char:[%x]}",strUtf8In[i]);
						i=i+1;
					}
				}
			}
		}
	}
	free(des);
	return 0;
}


int Convert::GBKToUtf8(const string& src,string *result)
{
    return 0;
}


