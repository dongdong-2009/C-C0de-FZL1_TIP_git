/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/CODE_4677_T00060500/transactive/app/maintenance_management/mms_agent/src/mq/PubFunction.cpp $
  * @author:   xinsong xia
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2006/12/08 11:20:56 $
  * Last mofified by:  $Author: buildteam $
  *
  *
  * This class privide some static public function for MMS Agent
  */

#pragma warning(disable:4786)

#include "StdPubUtility.h"
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <ace/OS.h>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = boost::filesystem;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_Base_Core
{
    const int MAX_CHAR_IN_LINE = 4096;

    char* CStdPubUtility::ltrim(char* szSourceText)
    {
        char* szToken = szSourceText;

        while (*szToken != '\0' && (CStdPubUtility::isSpace(*szToken)))
            szToken ++;

        return szToken;
    }

    wchar_t*   CStdPubUtility::ltrim(wchar_t* wszText)
    {
        //skip all balnks
        wchar_t* wszToken = wszText;

        while (*wszToken != '\0' && (isSpace(*wszToken)))
            wszToken ++;

        return wszToken;
    }

    char* CStdPubUtility::trim(char* szSourceText)
    {
        if (NULL == szSourceText)
            return NULL;

        if (strlen(szSourceText) == 0)
            return szSourceText;

        char* szStart = CStdPubUtility::ltrim(szSourceText);
        char* szReturn = szStart;
        assert(strlen(szStart) >= 0);

        char* szLast = NULL;

        while (*szStart != '\0')
        {
            if (CStdPubUtility::isSpace(*szStart))
            {
                if (NULL == szLast)
                    szLast = szStart;
            }
            else
                szLast = NULL;

            szStart ++;
        }

        if (szLast != NULL)
            // truncate at trailing space start
            *szLast = '\0';

        return szReturn;
    }

    wchar_t* CStdPubUtility::rtrim(wchar_t* wszSourceText)
    {
        assert(wszSourceText != NULL);
        wchar_t* wszLast = NULL;

        if (*wszSourceText == 0x0)
            return wszSourceText;

        wchar_t* wszEnd = wszSourceText + wcslen(wszSourceText) - 1;

        while (wszEnd > wszSourceText)
        {
            if (isSpace(*wszEnd))
            {
                *wszEnd = 0x0;
                wszEnd = wszEnd --;
            }
            else
            {
                break;
            }
        }

        if (isSpace(*wszEnd))
        {
            assert(wszSourceText == wszEnd);
            *wszEnd = 0x0;
        }

        return wszSourceText;
    }

    wchar_t* CStdPubUtility::trim(wchar_t* wszSourceText)
    {
        if (NULL == wszSourceText)
            return NULL;

        if (wcslen(wszSourceText) == 0)
            return wszSourceText;

        //if ( ! CanTrimString( wszSourceText ))
        //  return wszSourceText;

        wchar_t* wszStart = ltrim(wszSourceText);
        assert(wcslen(wszStart) >= 0);

        rtrim(wszStart);
        return wszStart;
    }

    wchar_t* CStdPubUtility::mergeWString(wchar_t*  szDestResult, wchar_t* szSourceResult, unsigned int* nRemainSize, unsigned int nInitSize)
    {
        assert(szDestResult != NULL && szSourceResult != NULL);
        size_t nAddWordLength = wcslen(szSourceResult) + 1;
        size_t nDestWordLength = wcslen(szDestResult) + 1;
        wchar_t* wszReturn = szDestResult;

        if (*nRemainSize < nAddWordLength)
        {
            assert(nInitSize > nAddWordLength);
            wszReturn = (wchar_t*) realloc(szDestResult, (nDestWordLength + nInitSize) * sizeof(wchar_t));
            *nRemainSize = nInitSize - nAddWordLength;
        }
        else
            *nRemainSize = *nRemainSize - nAddWordLength;

        wcscat(szDestResult, szSourceResult);

        return wszReturn;
    }

    void CStdPubUtility::copyWString(wchar_t** szDestWord, const wchar_t* szSourceWord)
    {
        assert(szSourceWord != NULL);

        //first release the memory
        if (*szDestWord != NULL)
        {
            delete (*szDestWord);
            *szDestWord = NULL;
        }

        size_t nWordLen = wcslen(szSourceWord);
        *szDestWord = new wchar_t[nWordLen + 1 ];
        wcscpy(*szDestWord, szSourceWord);
    }

    void CStdPubUtility::string_replace(std::string & strSource, const std::string & strOld, const std::string &strNew)
    {
        std::string::size_type pos = 0;
        std::string::size_type srclen = strOld.size();
        std::string::size_type dstlen = strNew.size();

        while ((pos = strSource.find(strOld, pos)) != std::string::npos)
        {
            strSource.replace(pos, srclen, strNew);
            pos += dstlen;
        }
    }

    void CStdPubUtility::trimString(std::string & strSource)
    {
        if (strSource.length() >= MAX_CHAR_IN_LINE)
        {
            std::cout << "string is too long to trim: " << strSource << std::endl;
            return;
        }

        char strBuffer[MAX_CHAR_IN_LINE];
        strcpy(strBuffer, strSource.c_str());
        char* szDest = CStdPubUtility::trim(strBuffer);
        strSource = szDest;
    }

    char* CStdPubUtility::removeSpeicalCharacter(const char* szSourceText, char* szDestBuffer, char chSpecialChar)
    {
        assert(! CStdPubUtility::isEmptyString(szSourceText));
        size_t nTextLength = strlen(szSourceText);
        size_t nDestIndex = 0;

        for (size_t i = 0; i < nTextLength; i++)
        {
            if (szSourceText[i] != chSpecialChar)
            {
                szDestBuffer[nDestIndex++] = szSourceText[i];
            }
        }

        szDestBuffer[nDestIndex] = 0x0;
        return szDestBuffer;
    }

    wchar_t* CStdPubUtility::removeSpeicalCharacter(const wchar_t* wszSourceText, wchar_t* wszDestBuffer, wchar_t chSpecialChar)
    {
        assert(! CStdPubUtility::isEmptyString(wszSourceText));
        size_t nTextLength = wcslen(wszSourceText);
        size_t nDestIndex = 0;

        for (size_t i = 0; i < nTextLength; i++)
        {
            if (wszSourceText[i] != chSpecialChar)
            {
                wszDestBuffer[nDestIndex++] = wszSourceText[i];
            }
        }

        wszDestBuffer[nDestIndex] = 0x0;
        return wszDestBuffer;
    }

    char* CStdPubUtility::seperateText(char** szText, char ch)
    {
        assert(*szText != NULL);

        if (**szText == '\0')
            return NULL;

        char* szToken = NULL;

        //skip all balnks
        while (**szText != '\0' && ((**szText) == ch))
            (*szText) ++;

        //forward
        szToken = *szText;

        while (**szText != '\0' && (**szText != ch))
            (*szText)++;

        //seperate the wszText
        if (**szText != '\0')
        {
            **szText = '\0';
            (*szText)++;
        }

        return CStdPubUtility::trim(szToken);
    }

    // delete the left and right character
    char* CStdPubUtility::cutToToken(char** szText)
    {
        if (**szText == L'\0')
            return NULL;

        char* szToken = NULL;

        //skip all balnks
        while (**szText != 0x0 && (isSpace(**szText)))
            (*szText) ++;

        //forward
        szToken = *szText;

        while (**szText != 0x0 && (!isSpace(**szText)))
            (*szText)++;

        //seperate the wszText
        if (**szText != 0x0)
        {
            **szText = 0x0 ;
            (*szText)++;
        }

        return szToken;
    }

    wchar_t* CStdPubUtility::seperateText(wchar_t** wszText, wchar_t wch)
    {
        assert(*wszText != NULL);

        if (**wszText == L'\0')
            return NULL;

        wchar_t* wszToken = NULL;

        //skip all balnks
        while (**wszText != L'\0' && ((**wszText) == wch))
            (*wszText) ++;

        //forward
        wszToken = *wszText;

        while (**wszText != L'\0' && (**wszText != wch))
            (*wszText)++;

        //seperate the wszText
        if (**wszText != L'\0')
        {
            **wszText = L'\0';
            (*wszText)++;
        }

        return CStdPubUtility::trim(wszToken);
    }

    // delete the left and right character
    wchar_t* CStdPubUtility::cutToToken(wchar_t** wszText)
    {
        if (**wszText == L'\0')
            return NULL;

        wchar_t* wszToken = NULL;

        //skip all balnks
        while (**wszText != L'\0' && (isSpace(**wszText)))
            (*wszText) ++;

        //forward
        wszToken = *wszText;

        while (**wszText != L'\0' && (!isSpace(**wszText)))
            (*wszText)++;

        //seperate the wszText
        if (**wszText != L'\0')
        {
            **wszText = L'\0';
            (*wszText)++;
        }

        return wszToken;
    }

    bool CStdPubUtility::readWholeFileContent(const char* fileName, bool shouldTrim, std::string & outputStr)
    {
        std::ifstream inputStream;
        outputStr = "";
        inputStream.open(fileName);

        if (inputStream.fail())
        {
            return false;
        }

        std::string oneLine;
        std::ostringstream outputStream;

        while (!inputStream.eof())
        {
            std::getline(inputStream, oneLine, '\n');

            if (shouldTrim)
            {
                trimString(oneLine);
            }

            if (oneLine.empty())
            {
                continue;
            }

            outputStream << oneLine << "\n";
        }

        inputStream.close();
        outputStr = outputStream.str();
        return true;
    }

    bool CStdPubUtility::pathExists(const char* pathName, std::string & reason)
    {
        try
        {
            if (! fs::exists(fs::path(pathName)))
            {
                reason = pathName;
                reason += "is invalid path";
                return false;
            }

            return true;
        }
        catch (const std::exception & ex)
        {
            reason = ex.what();
            return false;
        }
    }

    void CStdPubUtility::toUpper(char* szSource)
    {
        assert(szSource != NULL);

        while (*szSource != 0x0)
        {
            *szSource++ = ::toupper(*szSource);
        }
    }

    void CStdPubUtility::toLower(char* szSource)
    {
        assert(szSource != NULL);

        while (*szSource != 0x0)
        {
            *szSource++ = ::tolower(*szSource);
        }
    }

    std::string CStdPubUtility::getTimeStr(timeb* pt)
    {
        char buf[100] = { 0 };
        tm lt;
        timeb t;

        if (NULL == pt)
        {
            ::ftime(&t);
        }
        else
        {
            t = *pt;
        }

        ACE_OS::localtime_r(&t.time, &lt);
        size_t pos = ACE_OS::strftime(buf, 100, "%d/%m/%Y %H:%M:%S", &lt);
        ACE_OS::sprintf(buf + pos, ":%d", t.millitm);
        return buf;
    }

    std::string CStdPubUtility::getTimeStr(const timeb& tb)
    {
        char buf[100] = { 0 };
        tm lt;
        ACE_OS::localtime_r(&tb.time, &lt);
        size_t pos = ACE_OS::strftime(buf, 100, "%d/%m/%Y %H:%M:%S", &lt);
        ACE_OS::sprintf(buf + pos, ":%d", tb.millitm);
        return buf;
    }

    std::string CStdPubUtility::getTimeStr(time_t t)
    {
        char buf[100] = { 0 };
        tm lt;
        ACE_OS::localtime_r(&t, &lt);
        ACE_OS::strftime(buf, 100, "%d/%m/%Y %H:%M:%S", &lt);
        return buf;
    }
}
