#include "other.h"
#include<string>

using namespace std;

int string_to_int(string str)
{
	int res = 0;
	int l = str.length();
	int i = 0;
	bool flag = false;
	while (i < l && (str[i] < '0' || str[i]>'9'))
	{
		flag = str[i] == '-';
		++i;
	}
	while (i < l && (str[i] >= '0'&&str[i] <= '9'))
	{
		res *= 10;
		res += str[i++] - '0';
	}
	if (flag)
	{
		res = -res;
	}
	return res;
}

int string_to_reg(string str)
{
	if (str.empty())
	{
		return -1;
	}//!string.back()can not be used on empty strings
	 /*can be delete */
	if (str.back() == ',')
	{
		str.pop_back();
	}
	for (int i = 0; i < 34; ++i)
	{
		if ((str == reg_num[i]) || (str == reg_name[i]))
		{
			return i;
		}
	}
	return -1;
}

int power_2(int n)//移位
{
	int res = 1;
	while (n--)
	{
		res <<= 1;
	}
	return res;
}

string get_phrase(char *str, int &i, int l) //l means length of the string
{
	string res = "";
	while (i < l&&str[i] != ' ')
	{
		res += str[i++];
	}//按空格隔开
	return res;
}

string get_str(char *str, int &i, int l)
{
	string res = "";
	while (i < l)
	{
		res += str[i++];
		//!!!!!
		if (res.back() == '\\')
		{
			char ch = str[i++];
			char before = '\0';
			switch (ch)
			{
			case 'n':
				before = '\n'; break;
			case 't':
				before = '\t'; break;
			case 'r':
				before = '\r'; break;
			case '0':
				before = '\0'; break;
			case '\'':
				before = '\''; break;
			case '\\':
				before = '\\'; break;
			case'\"':
				before = '\"'; break;
			}
			res.back() = before;
		}
	}
	return res;
}