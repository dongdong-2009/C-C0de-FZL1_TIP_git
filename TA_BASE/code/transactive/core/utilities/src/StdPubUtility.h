/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4677_T00060500/transactive/app/maintenance_management/mms_agent/src/mq/PubFunction.h $
  * @author:  xinsong xia
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2006/12/08 11:20:56 $
  * Last modified by:  $Author: buildteam $
  *
  * This class privide some static public function for MMS Agent, those function will be refined to form a public class library
  */

#if !defined(AFX_STD_PUBUTILITY_H__1E5CCA2B_9CF1_4096_A15A_492220F783D6__INCLUDED_)
#define AFX_STD_PUBUTILITY_H__1E5CCA2B_9CF1_4096_A15A_492220F783D6__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <wchar.h>
#include <assert.h>
#include <time.h>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    class CStdPubUtility
    {
    public:

        static bool isEmptyString(const char* szText)
        {
            return (szText == 0x0 || *szText == 0x0);
        }

        static bool isEmptyString(const wchar_t* wszText)
        {
            return (wszText == NULL || *wszText == 0x0);
        }

        static bool isRealSpace(wchar_t ch)
        {
            return (L'\t' == ch || L' ' == ch);
        }

        static bool isRealSpace(char ch)
        {
            return ('\t' == ch || ' ' == ch);
        }

        static bool isSpace(char ch)
        {
            return ('\r' == ch || '\t' == ch || '\n' == ch || ' ' == ch);
        }

        static bool isSpace(wchar_t ch)
        {
            return (L'\r' == ch || L'\t' == ch || L'\n' == ch || L' ' == ch);
        }

        static char* ltrim(char* szSourceText);
        static wchar_t* ltrim(wchar_t* wszSourceText);

        static char* trim(char* szSourceText);
        static wchar_t* trim(wchar_t* wszSourceText);
        static wchar_t* rtrim(wchar_t* wszSourceText);

        static char* removeSpeicalCharacter(const char* szSourceText, char* szDestBuffer, char chSpecialChar);

        static wchar_t* removeSpeicalCharacter(const wchar_t* wszSourceText, wchar_t* wszDestBuffer, wchar_t chSpecialChar);

        static std::string getLimitedString(std::string & inputStr, unsigned int maximumLength, bool allowTruncate = true)
        {
            if (allowTruncate && inputStr.length() > maximumLength)
            {
                std::string tempStr = inputStr.substr(0, maximumLength);
                assert(tempStr.length() == maximumLength);
                return tempStr;
            }
            else
            {
                return inputStr;
            }
        }

        static void string_replace(std::string & strSource, const std::string & strOld, const std::string &strNew);

        static void trimString(std::string & strSource);

        static void copyWString(wchar_t** szDestWord, const wchar_t* szSourceWord);

        static wchar_t* mergeWString(wchar_t*  szDestResult, wchar_t* szSourceResult, unsigned int* nRemainSize, unsigned int nInitSize);

        static char* cutToToken(char** szText);

        static wchar_t* seperateText(wchar_t** wszText, wchar_t wch);
        static char* seperateText(char** szText, char ch);

        static wchar_t* cutToToken(wchar_t** wszText);

        static bool readWholeFileContent(const char* fileName, bool shouldTrim, std::string & outputStr);

        static bool pathExists(const char* pathName, std::string & reason);

        static void toUpper(char* szSource);

        static void toLower(char* szSource);

        static int stricmp(const char* string1, const char* string2)
        {
            const char* ch1 = string1;
            const char* ch2 = string2;
            int res;

            while (true)
            {
                res = toupper(*ch1) - toupper(*ch2);

                if (res)
                {
                    return res;
                }
                else
                {
                    if (!*ch1)
                    {
                        return 0;
                    }
                }

                ch1++;
                ch2++;
            }
        }

        static std::string getTimeStr(timeb*);
        static std::string getTimeStr(const timeb&);
        static std::string getTimeStr(time_t);

        template <typename T>
        static std::vector<boost::shared_ptr<T> > toShared(const std::vector<T*>& v)
        {
            std::vector<boost::shared_ptr<T> > vs;

            for (size_t i = 0; i < v.size(); ++i)
            {
                vs.push_back(boost::shared_ptr<T>(v[i]));
            }

            return vs;
        }
    };
};

#endif // !defined(AFX_PUBFUNCTION_H__1E5CCA2B_9CF1_4096_A15A_492220F783D6__INCLUDED_)
