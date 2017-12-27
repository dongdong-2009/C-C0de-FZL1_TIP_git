/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssUtility.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * Definition/Implementation file for GssUtility class
  */
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <iostream>
#include <stdexcept>
// #include <stdlib.h>
#include <limits>
#include <time.h>
#include <ctype.h>
#include <algorithm>

using namespace TA_Base_Bus;

int GssUtility::data2string(const ByteVector & data, ByteString & str)
{
    // str.reserve(data.size());
    // std::copy(data.begin(), data.end(), str.begin());
    // return str.length();
    //
    // OR
    //
    // Going this route rather than std::copy overcomes a problem that both Win
    // and Solaris STL have with using iterators to append to std::basic_string
    int converted = 0;
    ByteVector::const_iterator it;
    unsigned char c_a[2]; c_a[0] = '\0'; c_a[1] = '\0';
    const unsigned char * c_p = c_a;

    for ( it = data.begin() ; it != data.end() ; it++ )
    {
        c_a[0] = *it;

        str.append(c_p);

        ++converted;
    }

    return converted;
}

int GssUtility::string2data(const ByteString & str, ByteVector & data)
{
    data.reserve(str.size());
    std::copy(str.begin(), str.end(), data.begin());

    return data.size();
}


int GssUtility::data2string(const ByteVector & data, std::string & str)
{
    // str.reserve(data.size());
    // std::copy(data.begin(), data.end(), str.begin());
    // return str.length();
    //
    // OR
    //
    // Going this route rather than std::copy overcomes a problem
    // that both Win and Solaris stl have with using iterators and appending to
    // std::strings
    int converted = 0;
    ByteVector::const_iterator it;
    char c_a[2]; c_a[0] = '\0'; c_a[1] = '\0';
    const char * c_p = c_a;

    for ( it = data.begin() ; it != data.end() ; it++ )
    {
        c_a[0] = *it;

        str.append(c_p);

        ++converted;
    }

    return converted;
}

int GssUtility::string2data(const std::string & str, ByteVector & data)
{
    // blasted std::copy, why can't it adjust the ByteVector so that data.size()
    // returns non-zero after the copy. But it doesn't do any memory checking
    // or pre-flight, so: boom.
    // data.reserve(str.size());
    // std::copy(str.begin(), str.end(), data.begin());

    for ( std::string::const_iterator siter = str.begin() ;
          siter != str.end() ;
          siter++
        )
    {
        data.push_back( *siter );
    }

    return data.size();
}

int GssUtility::data2hex(const ByteVector & data, std::string & str)
{
    int converted = 0;
    ByteVector::const_iterator it;

    for ( it = data.begin() ; it != data.end() ; it++ )
    {
        // going from *(vector<Byte>::iterator) to int in two steps overcomes
        // some cross-platform compiler type-coercion idiosyncrasies 
        Byte b = *it;
        int i = b;
        char c[3] = {0};

        sprintf(c,"%.2X",i);

        str.append(c);

        ++converted;
    }

    return converted;
}

int GssUtility::hex2data(const std::string & str, ByteVector & data)
{
    throw std::logic_error("GssUtility::hex2data not implemented");

    return 0;
}

int GssUtility::data2hex(const ByteString & data, std::string & str)
{
    int converted = 0;
    ByteString::const_iterator it;

    for ( it = data.begin() ; it != data.end() ; it++ )
    {
        // going from *(vector<Byte>::iterator) to int in two steps overcomes
        // some cross-platform compiler type-coercion idiosyncrasies 
        Byte b = *it;
        int i = b;
        char c[3] = {0};

        sprintf(c,"%.2X",i);

        str.append(c);

        ++converted;
    }

    return converted;
}

int GssUtility::data2hex(Byte data, std::string & str)
{
    int converted = 0;
    int i = data;
    char c[5] = {0};

    sprintf(c,"%.2X",i);
    str.append(c);

    ++converted;

    return converted;
}

std::string GssUtility::data2hex(Byte data)
{
    int i = data;
    char c[5] = {0};

    sprintf(c,"%.2X",i);
    std::string str = c;

    return str;
}


std::string GssUtility::num2Str(int i, const char * format)
{
    char buffstr[32] = {0};
    sprintf(buffstr, format, i);

    std::string s = buffstr;

    return s;
}


void GssUtility::bool2string(bool b, std::string & str, BooleanConversionOutput format)
{
    if (b)
    {
        switch (format)
        {
            case booleanOutputYesNo:
                str = "yes";
                break;
            case booleanOutputOneZero:
                str = "1";
                break;
            case booleanOutputTrueFalse:
            default:
                str = "true ";
                break;
        }
    }
    else
    {
        switch (format)
        {
            case booleanOutputYesNo:
                str = "no ";
                break;
            case booleanOutputOneZero:
                str = "0";
                break;
            case booleanOutputTrueFalse:
            default:
                str = "false";
                break;
        }
    }
}

/**
  * string2bool
  *
  * Converts a string to a boolean
  *
  * @param str the string representation. If it is one of
  *        "true" or "yes" or is not equal zero, truth is returned.
  *        If it is one of "false", "no" or '0' (not just any
  *        old zero, as atoi returns zero for non-numbers), then
  *        false is returned.
  * @param unknown Specifies the return value if the input is
  *        unrecognised.
  */
bool GssUtility::string2bool(const std::string & str, bool unknown)
{
    std::string s;

    // begin by lowercasing everything
    // freakin' std::string.append being type-fussy...
    char cc[2]; cc[1]='\0';
    for ( std::string::const_iterator c = str.begin() ; c!=str.end() ; c++ )
    {
        cc[0] = tolower(*c);
        s.append( cc );
    }

    if ( ( s=="true" ) || ( s=="yes" ) || (atoi(s.c_str())!=0) )
    {
        return true;
    }
    else if ( ( s=="false" ) || ( s=="no" ) || ( s=="0" ) )
    {
        return false;
    }

    return unknown;
}

LocationType GssUtility::string2LocationType(const std::string & s)
{
    LocationType l = NO_LOCATION;

    if ( ( s=="OCC") )
    {
        l=OCC;
    }
    else if ( ( s=="DEPOT") || ( s=="DPT") )
    {
        l=DEPOT;
    }
    else if ( (s=="MSS") || (s=="SSS") || (s=="STATION")
            || (s=="SS1") || (s=="SS2")
            || (s=="SSS1") || (s=="SSS2") )
    {
        l=STATION;
    }

    return (l);
}

//
// Trying to ensure that strtok() and hence tokenise() is used not used in a
// re-entrant situation, we use a non-reentrant thread lock which asserts if
// tokenise is used incorrectly.  Because the strtok data that we are preventing
// reentrant access to is static, we also make the lock so.
static TA_Base_Core::NonReEntrantThreadLockable s_strtokLock;

int GssUtility::tokenise(const char * s, TokenList & tokens, const char * delimiters, int maxtokens)
{
    TA_Base_Core::ThreadGuard strtokGuard(s_strtokLock);

    if (maxtokens<2)
    {
        tokens.push_back(std::string(s));
        return tokens.size();
    }

    int numchars = strlen(s)+1;

    if (numchars > 1024)
    {
        std::cerr << "WARNING: truncating input to 1024 characters" << std::endl;
        numchars=1024;
    }

    char s_[1024];
    strncpy(s_,s,1024); // pads with zeros if numchars<1024
    s_[numchars] = 0;
    int numtokens=0;

    char * token = strtok(s_, delimiters);
    while (( token != 0 ) && (numtokens < maxtokens-1))
    {
        tokens.push_back(token);
        numtokens++;

        token = strtok(0, delimiters);
    }

    //
    // add the remainder of the string, if there is any left and we stopped
    // because we were about to exceed the maxtokens limit.
    if ((token!=0) && (numtokens >= maxtokens-1))
    {
        int tokenlen = strlen(token);
        // remove the string terminator and replace it with the first delimiter
        // BUT ONLY if the end of the string is not nigh anyway
        if ( (token + tokenlen) < (s_ + numchars -1) )
        {
            token[tokenlen] = delimiters[0];
        }

        tokens.push_back(token);
    }

    return tokens.size();
}


void GssUtility::detokenise(TokenList & tokens, std::string & s, const char * padding)
{
    // false for the first iteration.
    bool do_pad = false;

    for ( TokenList::const_iterator it = tokens.begin() ;
          it != tokens.end() ;
          it++
        )
    {
        if (do_pad)
        {
            s.append(padding);
        }

        s.append(*it);
        do_pad = true;
    }
}

// declaration.  Required for strict ansi compliance
void expandPartialRange(const std::string & r, Range & range, unsigned int max);
// definition
void expandPartialRange(const std::string & r, Range & range, unsigned int max)
{
    unsigned int range_min = 0;
    unsigned int range_max = max;

    std::string::size_type minus = r.find_first_of('-');
    if (minus != std::string::npos)
    {
        // atoi should just stop when it sees the '-' or alternatively, and
        // conveniently, return 0 if there is nothing in front of it
        range_min = atoi(r.c_str());

        // atoi should return 0 if there is no life beyond the '-'
        range_max = atoi(&((r.c_str())[minus+1]));

        if (range_max<range_min)
        {
            range_max = range_min;
        }

        if (range_max>max)
        {
            range_max = max;
        }

        for (unsigned int i=range_min; i<=range_max ; i++)
        {
            range.insert(i);
        }
    }
    else // there's hopefully just a number on its own
    {
        range_min = atoi(r.c_str());
        range.insert(range_min);
    }
}

void GssUtility::expandRange(const std::string & r, Range & range, unsigned int max)
{
    TokenList ranges;
    GssUtility::tokenise(r.c_str(), ranges, ",");

    for ( TokenList::iterator iter = ranges.begin() ;
          iter != ranges.end() ;
          iter++
        )
    {
        std::string rng = (*iter);
        expandPartialRange(rng, range, max);
    }
}

// declaration.  Required for strict ansi compliance
void formPartialRange(unsigned int min, unsigned int max, std::string & r);
// definition
void formPartialRange(unsigned int min, unsigned int max, std::string & r)
{
    char r_[64] = {0};
    if ( min == max )
    {
        sprintf(r_, "%d",max);
    }
    else
    {
        sprintf(r_, "%d-%d",min,max);
    }

    if (!r.empty())
    {
        r += ",";
    }

    r += r_;
}

void GssUtility::formRange(const Range & range, std::string & r)
{
    if (range.empty())
    {
        r="";
        return;
    }

    unsigned int min = UINT_MAX;
    unsigned int max = 0;
    unsigned int last_val = 0;
    bool needs_forming = false; // decides whether we need a final call to formPartialRange

    for ( Range::const_iterator iter = range.begin() ;
          iter != range.end() ;
          iter++
        )
    {
        if (*iter > last_val+1)
        {
            formPartialRange(min,max,r);
            min = max = *iter;
            needs_forming = false;
        }
        else
        {
            if ((*iter) > max)
            {
                max = *iter;
            }

            if ((*iter) < min)
            {
                min = *iter;
            }

            needs_forming = true;
        }

        last_val = *iter;
    }

    if (needs_forming)
    {
        formPartialRange(min,max,r);
    }
}


unsigned int GssUtility::random_uint32(unsigned int lo, unsigned int hi)
{
    double normalised_rand = (double)rand()/(double)RAND_MAX;
    unsigned int rint = static_cast<unsigned int>((normalised_rand*(hi-lo))+lo);

    return rint;
}


Word GssUtility::randomWord(Word lo, Word hi)
{
    return (GssUtility::random_uint32(lo,hi) & 0x0000FFFF);
}

Byte GssUtility::randomByte(Byte lo, Byte hi)
{
    return (GssUtility::randomWord(lo,hi) & 0x00FF);
}


std::string GssUtility::outputFormat2String(TableOutputFormat f)
{
    std::string s = "Unknown";
    if ( f == f_hex )
    {
        s = "hex";
    }
    else if ( f == f_human )
    {
        s = "human";
    }

    return s;
}

static const unsigned char sByteVectorFiller[] = "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

int GssUtility::fill(ByteVector & v)
{
    int f_len = strlen(reinterpret_cast<const char *>(sByteVectorFiller));
    int v_len = v.size();

    int f_idx = 0;
    int v_idx = 0;

    while ( v_idx < v_len )
    {
        //
        // reset index to zero if we've reached the end of the filler.
        if ( f_idx >= f_len )
        {
            f_idx = 0;
        }

        v[v_idx] = sByteVectorFiller[f_idx];

        v_idx++; f_idx++;
    }

    return v_idx;
}

bool GssUtility::initialiseUtilities(int argc, char* argv[])
{
    if( ! TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv) )
    {
        TA_THROW (TA_Base_Core::UtilitiesException("Command line arguments are in invalid format"));
    }

    // Set the debug file name
    std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setFile(param.c_str());
    }

    // Set the debug level
    param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(param.c_str()));
    }

    // Set the maximum size of a debug file in bytes
    param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
    }

    // Set the maximum number of log files
    param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
    if (!param.empty())
    {
        TA_Base_Core::DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
    }

	// Use Pid encoding in filenames
	param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGPIDFILENAMES);
	if (!param.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().encryptPidInFilenames(param);
	}
	
	// Use debug level specific log files
	param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELFILE);
	if (!param.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().decodeLevelFilenames(param);
	}
	
	// Turn off logging for debug levels
	param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELOFF);
	if (!param.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().decodeDisabledLevels(param);
	}

    return true;
}


bool GssUtility::getBackgroundHostPort(std::string & hoststr, std::string & portstr)
{
    if ((hoststr.empty()) || (portstr.empty()))
    {
        std::string lhoststr = "";
        std::string lportstr = "";
        std::string paramstr = TA_Base_Core::RunParams::getInstance().get("BackgroundHost");
        if (!paramstr.empty())
        {
            TokenList tl;
            int numtoks = GssUtility::tokenise(paramstr.c_str(), tl, ":");
            if (numtoks==2)
            {
                lhoststr = tl.front();
                tl.pop_front();
                lportstr = tl.front();
                tl.pop_front();
            }
            else if (numtoks==1)
            {
                // it is assumed it is the ipaddress, not the port which is
                // the single part
                lhoststr = tl.front();
            }
            else
            {
                // don't try to decipher anything other than 1 or 2 parts of
                // what should have been a host:port string. Head for default
                // city.
            }
        }

        if (hoststr.empty())
        {
            if (lhoststr.empty())
            {
                hoststr = "localhost";
            }
            else
            {
                hoststr = lhoststr;
            }
        }

        if (portstr.empty())
        {
            if (lportstr.empty())
            {
                portstr = "2341";
            }
            else
            {
                portstr = lportstr;
            }
        }
    }

    return true;
}

