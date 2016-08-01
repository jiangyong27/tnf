#include <utility>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "binary_support.h"

namespace Json
{

static void valueToBinary(std::string& sBuf, Value::UInt64 value)
{
	if (value < 0xFF)
	{
		sBuf.push_back((char) Integer1);
		sBuf.push_back((char) value);
	}
	else if (value <= 0xFFFF)
	{
		sBuf.push_back((char) Integer2);
		uint16_t wTmp = htons(static_cast<uint16_t> (value));
		sBuf.append(reinterpret_cast<char*> (&wTmp), sizeof(wTmp));
	}
	else if (value <= 0xFFFFFFFF)
	{
		sBuf.push_back((char) Integer4);
		uint32_t dwTmp = htonl(static_cast<uint32_t> (value));
		sBuf.append(reinterpret_cast<char*> (&dwTmp), sizeof(dwTmp));
	}
	else
	{
		sBuf.push_back((char) Integer8);
		uint64_t ui64Tmp = htonll(value);
		sBuf.append(reinterpret_cast<char*> (&ui64Tmp), sizeof(ui64Tmp));
	}
}

static void valueToBinary(std::string& sBuf, const std::string& value)
{
	if (value.size() <= 0xFF)
	{
		sBuf.push_back((char) String1);
		sBuf.push_back(static_cast<unsigned char>(value.size()));
	}
	else if (value.size() <= 0xFFFF)
	{
		sBuf.push_back((char) String2);
		uint16_t wSize =
			htons(static_cast<uint16_t> (value.size()));
		sBuf.append(reinterpret_cast<char*> (&wSize), sizeof(wSize));
	}
	else
	{
		sBuf.push_back((char) String4);
		uint32_t dwSize = htonl(static_cast<uint32_t> (value.size()));
		sBuf.append(reinterpret_cast<char*> (&dwSize), sizeof(dwSize));
	}
	sBuf.append(value.data(), value.size());
}

// Class BinaryWriter
// //////////////////////////////////////////////////////////////////
std::string BinaryWriter::write(const Value & root)
{
	document_ = "";
	document_.push_back((char)0);
	writeValue(root);
	return document_;
}

void BinaryWriter::writeValue(const Value & value)
{
	switch (value.type())
	{
		case nullValue:
			break;
		case intValue:
			valueToBinary(document_, value.asUInt64());
			break;
		case uintValue:
			valueToBinary(document_, value.asUInt64());
			break;
		case realValue:  //错误的值，但是避免产生空值的情况
//			longlongvalueToBinary(document_, static_cast<unsigned long long>(value.asDouble()));
			break;
		case stringValue:
			valueToBinary(document_, value.asString());
			break;
		case booleanValue:
			valueToBinary(document_, (value.asBool() ? 1 : 0));
			break;
		case arrayValue:
			{
				document_.push_back((char) Vector);
				size_t dwSizePos = document_.size();
				uint32_t dwSize = 0;
				document_.append(reinterpret_cast<char*> (&dwSize), sizeof(dwSize));
				//uint32_t dwSize = htonl(static_cast<unsigned long> (value.size()));
				//document_.append(reinterpret_cast<char*> (&dwSize), sizeof(dwSize));
				for (unsigned int index = 0; index < value.size(); index++)
				{
					if (!value[index].isNull()) {
                        ++dwSize;
						writeValue(value[index]);
                    }
				}
				dwSize = htonl(dwSize);
				memcpy((char*) document_.data() + dwSizePos,
						reinterpret_cast<char*> (&dwSize), sizeof(dwSize));
				break;
			}
		case objectValue:
			{
				document_.push_back((char) Map);
				size_t dwSizePos = document_.size();
				uint32_t dwSize = 0;
				document_.append(reinterpret_cast<char*> (&dwSize), sizeof(dwSize));

				Value::Members members(value.getMemberNames());
				for (Value::Members::iterator it = members.begin(); it != members.end(); ++it)
				{
					const std::string &name = *it;
					if (!value[name].isNull())
					{
						dwSize++;
						document_.push_back(name.size());
						document_.append(name.data(), name.size());
						writeValue(value[name]);
					}
				}
				dwSize = htonl(dwSize);
				memcpy((char*) document_.data() + dwSizePos,
						reinterpret_cast<char*> (&dwSize), sizeof(dwSize));

				break;
			}
	}
}

}
