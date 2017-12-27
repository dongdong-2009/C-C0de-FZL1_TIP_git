#include "CodeConverter.h"
#include "RunParams.h"
#include "DebugUtil.h"
#include <memory.h>
#define UTFLEN(x)  utf8_look_for_table[(x)]

namespace TA_Base_Core
{
    //0xxxxxxx
    //110xxxxx 10xxxxxx
    //1110xxxx 10xxxxxx 10xxxxxx
    //11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    unsigned char utf8_look_for_table[] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
    };

#if defined(WIN32)
    UINT strToCodePage(const std::string& charset)
    {
        if (TA_UTF8 == charset)
        {
            return CP_UTF8;
        }
        else if (TA_UTF7 == charset)
        {
            return CP_UTF7;
        }
        else
        {
            return CP_ACP;
        }
    }
#endif

    CodeConverter::CodeConverter(const char* from_charset, const char* to_charset)
    {
#if !defined(WIN32)
        cd = iconv_open(to_charset, from_charset);
#else
        m_from_charset = strToCodePage(from_charset);
        m_to_charset = strToCodePage(to_charset);
#endif
    }

#if !defined(WIN32)
    int CodeConverter::convert(const char* inbuf, int& inlenleft, char* outbuf, int& outlen)
    {
        const char** pin = &inbuf;
        char** pout = &outbuf;
        int outlenLeft = outlen;
        memset(outbuf, 0, outlen);
        int ret = iconv(cd, pin, (size_t*)&inlenleft, pout, (size_t*)&outlenLeft);
        outlen = outlen - outlenLeft;
        return ret;
    }
#else
    int CodeConverter::convert(const char* inbuf, int& inlenleft, char* outbuf, int& outlen)
    {
        int ret = 0;
        size_t unicodeLen = MultiByteToWideChar(m_from_charset, 0, inbuf, inlenleft, NULL, 0);

        /*if(unicodeLen <=0)
        return -1;*/

        wchar_t* pUnicode = NULL;
        pUnicode = new wchar_t[unicodeLen + 1];
        memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));
        ret = MultiByteToWideChar(m_from_charset, 0, inbuf, inlenleft, (LPWSTR)pUnicode, (int)unicodeLen);

        BYTE* pTargetData = NULL;
        int targetLen = WideCharToMultiByte(m_to_charset, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, 0, NULL, NULL);

        /*if(targetLen <=0)
        return -2;*/

        pTargetData = new BYTE[targetLen + 1];
        memset(pTargetData, 0, targetLen + 1);

        ret = WideCharToMultiByte(m_to_charset, 0, (LPWSTR)pUnicode, -1, (char*)pTargetData, targetLen, NULL, NULL);

        memcpy(outbuf, pTargetData, targetLen);
        outlen = targetLen - 1;

        delete [] pUnicode;
        delete [] pTargetData;

        return 0;
    }

#endif

    CodeConverter::~CodeConverter()
    {
#if !defined(WIN32)
        iconv_close(cd);
#endif
    }

    int CodeConverter::getUtf8Length(const char* str)
    {
        int clen = strlen(str);
        int len = 0;

        for (const char* ptr = str;
                *ptr != 0 && len < clen;
                len++, ptr += UTFLEN((unsigned char) * ptr));

        return len;
    }

    int CodeConverter::getUtf8BytesLength(const char* str, int nCharacterLen)
    {
        int nByteLen = strlen(str);
        int nlenByteTemp = 0;
        int nLenCharacterTemp = 0;

        for (const char* ptr = str; nlenByteTemp <= nByteLen;)
        {
            if (nLenCharacterTemp == nCharacterLen)
            {
                return nlenByteTemp;
            }

            nlenByteTemp += UTFLEN((unsigned char) * ptr);
            ptr += UTFLEN((unsigned char) * ptr);
            nLenCharacterTemp++;
        }

        return 0;
    }

    char* CodeConverter::subUtfString(const char* str, int start, int end)
    {
        int len = getUtf8Length(str);

        if (start >= len) return NULL;

        if (end > len) end = len;

        const char* sptr = str;

        for (int i = 0; i < start; ++i, sptr += UTFLEN((unsigned char) * sptr));

        const char* eptr = sptr;

        for (int i = start; i < end; ++i, eptr += UTFLEN((unsigned char) * eptr));

        int retLen = eptr - sptr;
        char* retStr = (char*)malloc(retLen + 1);
        memcpy(retStr, sptr, retLen);
        retStr[retLen] = 0;

        return retStr;
    }

#if defined(WIN32)

    std::wstring CodeConverter::ANSIToUnicode(const std::string& str)
    {
        return toWString(str, CP_ACP);
    }

    std::string CodeConverter::UnicodeToANSI(const std::wstring& str)
    {
        return toString(str, CP_ACP);
    }

    std::wstring CodeConverter::UTF8ToUnicode(const std::string& str)
    {
        return toWString(str, CP_UTF8);
    }

    std::string CodeConverter::UnicodeToUTF8(const std::wstring& str)
    {
        return toString(str, CP_UTF8);
    }

    std::string CodeConverter::toString(const std::wstring& fromStr, UINT toCodePage)
    {
        std::string toStr;
        toString(fromStr, toStr, toCodePage);
        return toStr;
    }

    std::wstring CodeConverter::toWString(const std::string& fromStr, UINT fromCodePage)
    {
        std::wstring toStr;
        toWString(fromStr, fromCodePage, toStr);
        return toStr;
    }

    void CodeConverter::toString(const std::wstring& fromStr, std::string& toStr, UINT toCodePage)
    {        
        size_t len = WideCharToMultiByte(toCodePage, 0, fromStr.c_str(), -1, NULL, 0, NULL, NULL);
        char* buff = new char[ len + 1];
        memset((void*)buff, 0, sizeof(char) * (len + 1));
        :: WideCharToMultiByte(toCodePage, 0, fromStr.c_str(), -1, buff, len, NULL, NULL);
        toStr = buff;
        delete[] buff ;
    }

    void CodeConverter::toWString(const std::string& fromStr, UINT fromCodePage, std::wstring& toStr)
    {
        size_t len = ::MultiByteToWideChar(fromCodePage, 0, fromStr.c_str(), -1, NULL, 0);
        wchar_t* buff = new wchar_t[len + 1];
        memset(buff, 0, (len + 1)* sizeof(wchar_t));
        :: MultiByteToWideChar(fromCodePage, 0, fromStr.c_str(), -1, buff, len);
        toStr = buff;
        delete[] buff ;
    }

    std::string CodeConverter::ANSIToUTF8(const std::string& str)
    {
        return UnicodeToUTF8(ANSIToUnicode(str));
    }

    std::string CodeConverter::UTF8ToANSI(const std::string& str)
    {
        return UnicodeToANSI(UTF8ToUnicode(str));
    }

#endif

    std::string CodeConverter::convertString(const std::string& str, const std::string& fromcode, const std::string& tocode)
    {
#if defined(WIN32)
        return (fromcode == TA_UTF8) ? UTF8ToANSI(str) : ANSIToUTF8(str);
#else
        iconv_t cd = iconv_open(tocode.c_str(), fromcode.c_str());

        if (cd == (iconv_t) -1)
        {
            return str;
        }

        const char* inbuf = str.c_str();
        size_t inleft = str.size() + 1;
        size_t outleft = 2 * inleft;
        char* outbuf = new char[outleft];
        char* outbufCopy = outbuf;
        size_t ret = iconv(cd, &inbuf, &inleft, &outbufCopy, &outleft);
        std::string outStr(outbuf);
        iconv_close(cd);
        delete[] outbuf;
        return ret != -1 ? outStr : str;
#endif
    }

    std::string CodeConverter::toUTF8(const std::string& str)
    {
        return convertString(str, TA_Base_Core::getRunParamValue(RPARAM_CHARSET, TA_GB18030), TA_UTF8);
    }

    std::string CodeConverter::toANSI(const std::string& str)
    {
        return convertString(str, TA_UTF8, TA_Base_Core::getRunParamValue(RPARAM_CHARSET, TA_GB18030));
    }
}
