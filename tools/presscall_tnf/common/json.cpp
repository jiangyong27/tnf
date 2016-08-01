// Copyright (c) 2013, Tencent Inc.
// All rights reserved.
//
// Author:jason  <jasonyjiang@tencent.com>
// Created: 2014-01-05
//

#include <cstdio>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "json.h"
namespace tx{
namespace util{

#define C buffer[index]

//namespace start

Json::Json() : m_type(TYPE_UNKNOWN)
{

}

Json::Json(char value) : m_type(TYPE_INTEGER), m_i_value(value)
{

}

Json::Json(short value) : m_type(TYPE_INTEGER), m_i_value(value)
{

}

Json::Json(int value) : m_type(TYPE_INTEGER), m_i_value(value)
{

}

Json::Json(long value) : m_type(TYPE_INTEGER), m_i_value(value)
{

}

Json::Json(double value) : m_type(TYPE_REAL), m_d_value(value)
{

}

Json::Json(const char *value) : m_type(TYPE_STRING), m_str_value(value)
{

}

Json::Json(const std::string& value) : m_type(TYPE_STRING), m_str_value(value)
{

}

Json::Json(bool value) : m_type(TYPE_BOOLEAN), m_b_value(value)
{

}

Json::Json(json_type_t t) : m_type(t)
{
	if(t != TYPE_ARRAY && t != TYPE_OBJECT)
	 throw std::string("Must be type: Array or Object");
}

const Json& Json::operator [](const char *name) const
{
	if (m_type != TYPE_OBJECT)
		throw std::string("Must be type: Object");
	json_map_t::const_iterator it = m_object.find(name);
	if (it != m_object.end())
		return it -> second;
	throw std::string("Key not found: " + std::string(name));
}

const Json& Json::operator [](const std::string& name)const
{
	if(m_type != TYPE_OBJECT)
		throw std::string("Must be Type:Object");
	json_map_t::const_iterator it = m_object.find(name);
	if(it != m_object.end())
		return it->second;
	throw std::string("Not found key:" + name);
}

Json& Json::operator [](const char *name)
{
	if (m_type == TYPE_UNKNOWN)
		m_type = TYPE_OBJECT;
	if (m_type != TYPE_OBJECT)
		throw std::string("Must be type: Object");
	return m_object[name];
}

Json& Json::operator [](const std::string& name)
{
	if(m_type == TYPE_UNKNOWN)
		m_type = TYPE_OBJECT;
	if(m_type != TYPE_OBJECT)
		throw std::string("Must be Type:Object");
	return m_object[name];
}

Json::operator char() const
{
	return (char)m_i_value;
}

Json::operator short() const
{
	return (short)m_i_value;
}

Json::operator int() const
{
	return (int)m_i_value;
}
Json::operator long() const
{
	return m_i_value;
}

Json::operator double() const
{
	return m_d_value;
}

Json::operator std::string() const
{
	return m_str_value;
}

Json::operator bool() const
{
	return m_b_value;
}



Json::json_type_t Json::Type() const
{
	return m_type;
}

bool Json::HasValue(const std::string& name) const
{
	if(m_type != TYPE_OBJECT)
		return false;
	return m_object.find(name) != m_object.end();
}

void Json::Add(Json data)
{
	if(m_type == TYPE_UNKNOWN)
		m_type = TYPE_ARRAY;
	if(m_type != TYPE_ARRAY)
		throw std::string("HasValue must be a List");
	m_array.push_back(data);
}

const std::string& Json::GetString()const
{
	return m_str_value;
}

const Json::json_list_t& Json::GetArray()const
{
	if(m_type != TYPE_ARRAY)
		throw std::string("Must be Type:Array");
	return m_array;
}

const Json::json_map_t& Json::GetObject() const
{
	if(m_type != TYPE_OBJECT)
		throw std::string("Must be Type:Object");
	return m_object;
}

Json::json_list_t& Json::GetArray()
{
	if(m_type == TYPE_UNKNOWN)
		m_type = TYPE_ARRAY;
	if(m_type != TYPE_ARRAY)
		throw std::string("Must be Type:Array");
	return m_array;
}

Json::json_map_t& Json::GetObject()
{
	if(m_type == TYPE_UNKNOWN)
		m_type = TYPE_OBJECT;
	if(m_type != TYPE_OBJECT)
		throw std::string("Must be Type:Object");
	return m_object;
}


std::string Json::ToString(bool quote) const
{
	switch (m_type)
	{
	case TYPE_UNKNOWN:
		break;
	case TYPE_INTEGER:
	{
		std::string str;
		char tmp[100];
		snprintf(tmp,sizeof(tmp),"%ld",m_i_value);
		str = tmp;
		return str;
	}
	case TYPE_REAL:
	{
		char slask[100];
		sprintf(slask, "%f", m_d_value);
		return slask;
	}
	case TYPE_STRING:
	{
		std::ostringstream tmp;
		tmp << "\"" << encode(m_str_value) << "\"";
		return tmp.str();
	}
	case TYPE_BOOLEAN:
		return m_b_value ? "true" : "false";
	case TYPE_ARRAY:
	{
		std::ostringstream tmp;
		bool first = true;
		tmp << "[";
		for (json_list_t::const_iterator it = m_array.begin(); it != m_array.end(); it++)
		{
			const Json& ref = *it;
			if (!first)
				tmp << ",";
			tmp << ref.ToString(quote);
			first = false;
		}
		tmp << "]";
		return tmp.str();
	}
	case TYPE_OBJECT:
	{
		std::ostringstream tmp;
		bool first = true;
		tmp << "{";
		for (json_map_t::const_iterator it = m_object.begin(); it != m_object.end(); it++)
		{
			const std::pair<std::string, Json>& ref = *it;
			if (!first)
				tmp << ",";
			if (quote)
				tmp << "\"" << encode(ref.first) << "\":" << ref.second.ToString(quote);
			else
				tmp << ref.first << ":" << ref.second.ToString(quote);
			first = false;
		}
		tmp << "}";
		return tmp.str();
	}
	}
	return "null";
}

void Json::encode(std::string& src) const
{
    std::string new_src;
    for (size_t i = 0; i < src.size();) {
        if (src.compare(i, 1, "\"") == 0) {
            new_src.append("\\\"");
            i += 1;
        } else if (src.compare(i, 1, "\r") == 0) {
            new_src.append("\\r");
            i += 1;
        } else if (src.compare(i, 1, "\n") == 0) {
            new_src.append("\\n");
            i += 1;
        } else if (src.compare(i, 1, "\\") == 0) {
            new_src.append("\\\\");
            i += 1;
        } else {
            new_src.append(1, src[i++]);
        }
    }
    src.assign(new_src);
    //printf("encode[%s][%s]\n", src.c_str(),new_src.c_str());
    /*
	size_t pos = src.find("\\");
	while (pos != std::string::npos)
	{
		src.replace(pos, 1, "\\\\");
		pos = src.find("\\", pos + 2);
	}
	pos = src.find("\r");
	while (pos != std::string::npos)
	{
		src.replace(pos, 1, "\\r");
		pos = src.find("\r", pos + 2);
	}
	pos = src.find("\n");
	while (pos != std::string::npos)
	{
		src.replace(pos, 1, "\\n");
		pos = src.find("\n", pos + 2);
	}
	pos = src.find("\"");
	while (pos != std::string::npos)
	{
		src.replace(pos, 1, "\\\"");
		pos = src.find("\"", pos + 2);
	}
    */
}


// --------------------------------------------------------------------------------
void Json::decode(std::string& src) const
{
    std::string new_src;
    for (size_t i = 0; i < src.size();) {
        if (src.compare(i, 2, "\\\"") == 0) {
            new_src.append("\"");
            i += 2;
        } else if (src.compare(i, 2, "\\r") == 0) {
            new_src.append("\r");
            i += 2;
        } else if (src.compare(i, 2, "\\n") == 0) {
            new_src.append("\n");
            i += 2;
        } else if (src.compare(i, 2, "\\\\") == 0) {
            new_src.append("\\");
            i += 2;
        } else {
            new_src.append(1, src[i++]);
        }
    }
    //printf("decode[%s][%s]\n", src.c_str(), new_src.c_str());

    src.assign(new_src);
    /*
	size_t pos = src.find("\\\"");
	while (pos != std::string::npos)
	{
		src.replace(pos, 2, "\"");
		pos = src.find("\\\"", pos + 1);
	}
	pos = src.find("\\r");
	while (pos != std::string::npos)
	{
		src.replace(pos, 2, "\r");
		pos = src.find("\\r", pos + 1);
	}
	pos = src.find("\\n");
	while (pos != std::string::npos)
	{
		src.replace(pos, 2, "\n");
		pos = src.find("\\n", pos + 1);
	}
	pos = src.find("\\\\");
	while (pos != std::string::npos)
	{
		src.replace(pos, 2, "\\");
		pos = src.find("\\\\", pos + 1);
	}
    */
}


// --------------------------------------------------------------------------------
std::string Json::encode(const std::string& src) const
{
	std::string tmp(src);
	encode(tmp);
	return tmp;
}

// --------------------------------------------------------------------------------
Json Json::Parse(const std::string& data)
{
	size_t i = 0;
	Json obj;
	obj.Parse(data.c_str(), i);
	return obj;
}


// --------------------------------------------------------------------------------
int Json::Token(const char *buffer, size_t& index, std::string& ord)
{
	while (C == ' ' || C == 9 || C == 13 || C == 10)
		++index;
	size_t x = index; // origin
	if (C == '-' || isdigit(C)) // Number
	{
		bool dot = false;
		if (C == '-')
		{
			++index;
		}
		while (isdigit(C) || C == '.')
		{
			if (C == '.')
				dot = true;
			++index;
		}
		size_t sz = index - x;
		ord = std::string(buffer + x, sz);
		if (dot)
		{
			m_type = TYPE_REAL;
		}
		else
		{
			m_type = TYPE_INTEGER;
		}
		return -m_type;
	}
	else
	if (C == 34) // " - String
	{
		bool ign = false;
		x = ++index;
		while (C && (ign || C != 34))
		{
			if (ign)
			{
				ign = false;
			}
			else
			if (C == '\\')
			{
				ign = true;
			}
			++index;
		}
		size_t sz = index - x;
		ord = std::string(buffer + x, sz);
		decode(ord);
		++index;
		m_type = TYPE_STRING;
		return -m_type;
	}
	else
	if (!strncmp(&buffer[index], "null", 4)) // null value
	{
		m_type = TYPE_UNKNOWN;
		ord = std::string(buffer + x, 4);
		index += 4;
		return -m_type;
	}
	else
	if (!strncmp(&buffer[index], "true", 4)) // Boolean: true
	{
		m_type = TYPE_BOOLEAN;
		ord = std::string(buffer + x, 4);
		m_b_value = true;
		index += 4;
		return -m_type;
	}
	else
	if (!strncmp(&buffer[index], "false", 5)) // Boolean: false
	{
		m_type = TYPE_BOOLEAN;
		ord = std::string(buffer + x, 5);
		m_b_value = false;
		index += 5;
		return -m_type;
	}
	return buffer[index++];
}


// --------------------------------------------------------------------------------
char Json::Parse(const char *buffer, size_t& index)
{
	std::string ord;
	int token = Token(buffer, index, ord);
	if (token == -TYPE_REAL || token == -TYPE_INTEGER)
	{
		if (token == -TYPE_REAL)
		{
			m_d_value = atof(ord.c_str());
		}
		else
		{
			m_i_value = atoi(ord.c_str());
		}
	}
	else
	if (token == -TYPE_STRING)
	{
		m_str_value = ord;
	}
	else
	if (token == -TYPE_UNKNOWN)
	{
	}
	else
	if (token == -TYPE_BOOLEAN)
	{
	}
	else
	if (token == '[') // Array
	{
		m_type = TYPE_ARRAY;
		while (true)
		{
			char res;
			Json o;
			if ((res = o.Parse(buffer, index)) == 0)
			{
				m_array.push_back(o);
			}
			else
			if (res == ']')
			{
				break;
			}
			else
			if (res == ',') // another element follows
			{
			}
			else
			{
				throw std::string(std::string("Unexpected end of Array: ") + res);
			}
		}
	}
	else
	if (token == ']') // end of Array
	{
		return ']';
	}
	else
	if (token == '{') // Object
	{
		m_type = TYPE_OBJECT;
		int state = 0;
		std::string element_name;
		bool quit = false;
		while (!quit)
		{
			Json o;
			char res = o.Parse(buffer, index);
			switch (state)
			{
			case 0:
				if (res == ',') // another object follow
					break;
				if (res == '}') // end of Object
				{
					quit = true;
					break;
				}
				if (res || o.Type() != TYPE_STRING)
					throw std::string("Object element name missing");
				element_name = o.GetString();
				state = 1;
				break;
			case 1:
				if (res != ':')
					throw std::string("Object element separator missing");
				state = 2;
				break;
			case 2:
				if (res)
					throw std::string(std::string("Unexpected character when parsing anytype: ") + res);
				m_object[element_name] = o;
				state = 0;
				break;
			}
		}
	}
	else
	if (token == '}') // end of Object
	{
		return '}';
	}
	else
	if (token == ',')
	{
		return ',';
	}
	else
	if (token == ':')
	{
		return ':';
	}
	else
	{
		throw std::string("Can't parse Json representation: " + std::string(&buffer[index]));
	}
	return 0;
}


}
}
