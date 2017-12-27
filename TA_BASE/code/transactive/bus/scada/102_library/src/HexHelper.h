#ifndef HEX_HELPER_H
#define HEX_HELPER_H
#include <stdio.h>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>

namespace TA_Base_Bus
{

	class HexHelper
	{
	public:
		static void hexToStr(unsigned char* pHex, int len, char* pStr);
		static std::string hexToStr(std::vector<unsigned char>& vtChar);
		static std::string hexToStr(const std::string& incomingBuffer_string);

		
		static bool strToHex(const char* pStr,int len,unsigned char* pHex);
		static bool strToHex(std::string strValue,std::vector<unsigned char>& vtChar);

		static std::string trim(const std::string& strTrim);

		//void HexDemp(unsigned char* pHex);
	private:
	
		HexHelper();
		virtual ~HexHelper();
	
	};
}

#endif

