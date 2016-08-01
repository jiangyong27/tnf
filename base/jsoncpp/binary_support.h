#ifndef __JSON_BINARY_SUPPORT_H_BY_PLUTOXIONG_20111115
#define __JSON_BINARY_SUPPORT_H_BY_PLUTOXIONG_20111115

#include <stack>
#include "value.h"
#include "reader.h"
#include "writer.h"

#include <stdint.h>
#include <byteswap.h>         

#if __BYTE_ORDER == __BIG_ENDIAN
#define ntohl(x)    (x)       
#define ntohs(x)    (x)       
#define htonl(x)    (x)       
#define htons(x)    (x)       
#define ntohll(x)   (x)       
#define htonll(x)   (x)       
#else  
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define ntohl(x)    __bswap_32 (x)
#define ntohs(x)    __bswap_16 (x)
#define htonl(x)    __bswap_32 (x)
#define htons(x)    __bswap_16 (x)
#define ntohll(x)   __bswap_64 (x)
#define htonll(x)   __bswap_64 (x)
#endif
#endif

namespace Json
{
	enum BinaryValueType //realtype
	{
		Integer1	= 0, ///< tiny int value (1字节) 
		Integer2	= 1, ///< small int value (2字节)
		Integer4 	= 2, ///< signed integer value(int32)(4字节)
		Integer8 	= 3, ///< big signed interger value(int64)(8字节)
		String1 	= 4, ///< string value  //1个字节表示长度
		String2 	= 5, ///< string value  //2个字节表示长度
		String4 	= 6, ///< string value  //4个字节表示长度
		Vector 		= 7, ///< array value (double list) 
		Map 		= 8, ///< object value (collection of name/value pairs).
		EXT 		= 9,
	};

	class BinaryReader
	{
		public:
			typedef char Char;
			typedef const Char *Location;

			struct Node
			{
				unsigned char type_;
				unsigned long size_;
			};

		public:
			BinaryReader(){};
			~BinaryReader(){};
			inline Value &currentValue();  
			bool readNode(Node & node);    
			bool readValue();         

			bool readInteger(Node & node); 
			bool readString(Node & node);  
			bool readVector(Node & node);  
			bool readMap(Node & node);

			bool parse(const char * pszData, size_t size, Value & root);
			bool parse(const std::string &document, Value &root);
			bool parse(const char *beginDoc, const char *endDoc, Value &root);
			bool parse(std::istream &is, Value &root);

		private:                      
			typedef std::stack<Value *> Nodes;
			Nodes nodes_;

			std::string document_;
			Location begin_;
			Location end_;
			Location current_;
	};

	class BinaryWriter: public Writer
	{
		public:                       
			BinaryWriter() {          
			}  
			;                         
			virtual ~BinaryWriter() { 
			}  
			;  

		public:
			virtual std::string write(const Value &root);

		private:
			void writeValue(const Value &value);

			static void writeInt(std::string & sBuf, int iValue);
			static void writeUInt(std::string & sBuf, unsigned int uiValue); 
			static void writeString(std::string & sBuf, const std::string & sValue);

			std::string document_;
	};

};

#endif
