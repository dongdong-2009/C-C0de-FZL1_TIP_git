//#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/102_library/src/HexHelper.h"			

//using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	HexHelper::HexHelper()
	{
	}

	HexHelper::~HexHelper()
	{
	}

	void HexHelper::hexToStr( unsigned char* pHex, int len, char* pStr )
	{
		for(int i = 0; i < len; i++)  
		{  
			sprintf(pStr, "%02X",pHex[i]);  
			pStr += 2;  
		}    
	}

	bool HexHelper::strToHex(const char* pStr,int len,unsigned char* pHex)
	{
		char high, low;  
		int idx, ii=0;  
		for (idx = 0; idx < len; idx += 3)   
		{  
			high = pStr[idx];  
			low = pStr[idx+1];  
			  
			if(high>='0' && high<='9')  
				high = high-'0';  
			else if(high>='A' && high<='F')  
				high = high - 'A' + 10;  
			else if(high>='a' && high<='f')  
				high = high - 'a' + 10;  
			else  
				return false;  
			  
			if(low>='0' && low<='9')  
				low = low-'0';  
			else if(low>='A' && low<='F')  
				low = low - 'A' + 10;  
			else if(low>='a' && low<='f')  
				low = low - 'a' + 10;  
			else  
				return false;  
			  
			pHex[ii++] = high<<4 | low;  
		}  
		return true; 
	}
	
	std::string HexHelper::hexToStr(std::vector<unsigned char>& vtChar)
	{
		// Convert from unsigned char vector to string
		std::stringstream incomingBuffer_s;
		for (unsigned int i = 0; i < vtChar.size(); i++)
		{
			incomingBuffer_s << vtChar[i];
		}
		std::string incomingBuffer_string = incomingBuffer_s.str();

		const char* incomingBuffer_ptr = incomingBuffer_string.c_str();
		std::stringstream result;
		int initial_loop = true;
		for (unsigned int j = 0; j < incomingBuffer_string.length(); j++)
		{
			unsigned int value = 0xff & (unsigned int) incomingBuffer_ptr[j];
			if (initial_loop == true)
			{
				result << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << value;
				initial_loop = false;
			}
			else
			{
				result << " " << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << value;
			}
		}	
		return result.str();
	}

	std::string HexHelper::hexToStr(const std::string& incomingBuffer_string)
	{
		const char* incomingBuffer_ptr = incomingBuffer_string.c_str();
		std::stringstream result;
		int initial_loop = true;
		for (unsigned int j = 0; j < incomingBuffer_string.length(); j++)
		{
			unsigned int value = 0xff & (unsigned int) incomingBuffer_ptr[j];
			if (initial_loop == true)
			{
				result << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << value;
				initial_loop = false;
			}
			else
			{
				result << " " << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << value;
			}
		}	
		return result.str();
	}



	bool HexHelper::strToHex(std::string strValue,std::vector<unsigned char>& vtChar)
	{
		int nLength = (strValue.length() + 1)/3;
		unsigned char* szTemp = new unsigned char[nLength];
		
		bool bResult =  strToHex(strValue.c_str(),strValue.size(),szTemp);
		
		if (bResult)
		{
			for (int i = 0; i < nLength ; i++)
			{
				vtChar.push_back(szTemp[i]);
			}
		}
		delete szTemp;
		return bResult;
	}
	
	std::string HexHelper::trim(const std::string& strTrim)
	{
		std::string::const_iterator it = strTrim.begin();
		while (it != strTrim.end() && isspace(*it))
			it++;
		
		std::string::const_reverse_iterator rit = strTrim.rbegin();
		while (rit.base() != it && isspace(*rit))
			rit++;
		
		return std::string(it,rit.base());
	}
		
}

