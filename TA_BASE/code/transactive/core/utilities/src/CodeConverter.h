#ifndef CODECONVERTER_H_INCLUDED
#define CODECONVERTER_H_INCLUDED

#if !defined(WIN32)
    #define LIBICONV_PLUG
    #include <iconv.h>
#else
#define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include <cstring>
#include <string>

#define TA_GB18030  "GB18030"
#define TA_GBK      "GBK"
#define TA_UTF8     "UTF-8"
#define TA_UTF7     "UTF-7"
#define TA_UTF16    "UTF-16"
#define TA_UTF16BE  "UTF-16BE"
#define TA_UTF16LE  "UTF-16LE"
#define TA_UTF32    "UTF-32"
#define TA_UTF32BE  "UTF-32BE"
#define TA_UTF32LE  "UTF-32LE"

namespace TA_Base_Core
{
    class CodeConverter
    {
    public:

        CodeConverter(const char* from_charset, const char* to_charset);
        ~CodeConverter();

        int convert(const char* inbuf, int& inleft, char* outbuf, int& outleft);

        static std::string convertString(const std::string& str, const std::string& fromcode, const std::string& tocode);
        static std::string toUTF8(const std::string& str);
        static std::string toANSI(const std::string& str);
        static int getUtf8Length(const char* str);
        static char* CodeConverter::subUtfString(const char* str, int start, int end);
        static int getUtf8BytesLength(const char* str, int nCharacterLen);

#if defined(WIN32)
        static std::wstring ANSIToUnicode(const std::string& str);
        static std::string UnicodeToANSI(const std::wstring& str);
        static std::wstring UTF8ToUnicode(const std::string& str);
        static std::string UnicodeToUTF8(const std::wstring& str);
        static std::string ANSIToUTF8(const std::string& str);
        static std::string UTF8ToANSI(const std::string& str);
        static std::string toString(const std::wstring& str, UINT toCodePage);
        static std::wstring toWString(const std::string& str, UINT fromCodePage);
        static void toString(const std::wstring& fromStr, std::string& toStr, UINT toCodePage);
        static void toWString(const std::string& fromStr, UINT fromCodePage, std::wstring& toStr);
#endif

    protected:

#if defined(WIN32)
        UINT m_from_charset;
        UINT m_to_charset;
#else
        iconv_t cd;
#endif
    };
}

#endif
