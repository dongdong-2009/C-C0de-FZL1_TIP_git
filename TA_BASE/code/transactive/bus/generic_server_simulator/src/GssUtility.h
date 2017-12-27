#ifndef GENERIC_SERVER_SIMULATOR_UTILITY_H
#define GENERIC_SERVER_SIMULATOR_UTILITY_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssUtility.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for GssUtility class
  */

// for ntohs,ntohl,htonl,htons
#if !defined( WIN32 )
#include <arpa/inet.h>
#else
#include <winsock2.h>
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // !WIN32

#include "bus/generic_server_simulator/src/GssCommon.h"
#include <string>
#include <set>
#include <list>
#include <limits.h>

#define POP_TOKEN(toks) toks.front(); toks.pop_front()

namespace TA_Base_Bus
{
    /**
      * Class with utility (static) methods for the GenericServerSimulator
      */
    class GssUtility
    {
    public:

        /**
          * data2hex
          *
          * Converts the binary data in ByteVector to its hexadecimal representation
          *
          * @return the number of bytes converted to hex
          */
        static int data2hex(const ByteVector & data, std::string & str);

        /**
          * string2data
          *
          * Converts the binary data in ByteVector to its hexadecimal representation
          * An assumption is made that if there are an odd number of characters in the
          * string to be converted, a LEADING zero is missing (and hence added).
          *
          * @return the number of binary bytes converted from hex
          */
        static int hex2data(const std::string & str, ByteVector & data);

        /**
          * data2hex
          *
          * Converts the binary data in a ByteString to its hexadecimal representation
          *
          * @return the number of bytes converted to hex
          */
        static int data2hex(const ByteString & data, std::string & str);

        /**
          * data2hex
          *
          * Converts the binary data in a Byte to its hexadecimal representation
          *
          * @return the number of bytes converted to hex (1!)
          */
        static int data2hex(Byte data, std::string & str);

        /**
          * data2hex
          *
          * Converts the binary data in a Byte to its hexadecimal representation
          *
          * @return the number of bytes converted to hex (1!)
          */
        static std::string data2hex(Byte data);

        /**
          * data2string
          *
          * Converts each unsigned char of the binary data in ByteVector to
          * a signed char in a ByteString
          *
          * @return the number of bytes converted to hex
          */
        static int data2string(const ByteVector & data, ByteString & str);

        /**
          * string2data
          *
          * Converts each signed char in a ByteString to unsigned chars in
          * a ByteVector.
          *
          * @return the number of binary bytes converted from hex
          */
        static int string2data(const ByteString & str, ByteVector & data);

        /**
          * data2string
          *
          * Converts each unsigned char of the binary data in ByteVector to
          * a signed char in a std::string
          *
          * @return the number of bytes converted to hex
          */
        static int data2string(const ByteVector & data, std::string & str);

        /**
          * string2data
          *
          * Converts each signed char in a std::string to unsigned chars in
          * a ByteVector.
          *
          * @return the number of binary bytes converted from hex
          */
        static int string2data(const std::string & str, ByteVector & data);

        /**
          * Converts a number to a string.  Optional format parameter is in
          * sprintf format.
          *
          * @param i the integer to convert
          * @param format the optional sprintf format string. Default="%d"
          */
        static std::string num2Str(int i, const char * format = "%d");


        /**
          * An enumeration to represent the possible formats for converting
          * a boolean to a string
          */
        typedef enum BooleanConversionOutput
        {
            booleanOutputTrueFalse  = 0,
            booleanOutputYesNo      = 1,
            booleanOutputOneZero    = 2
        };

        /**
          * bool2string
          *
          * Converts the boolean to a string
          *
          * @param b the boolean value
          * @param str the string representation. One of "true " or "false"
          *             (note the trailing space, for formatting purposes)
          * @param format see the BooleanConversionOutput enumueration
          *             Defaults to booleanOutputTrueFalse.
          */
        static void bool2string(bool b, std::string & str, BooleanConversionOutput format=booleanOutputTrueFalse);

        /**
          * string2bool
          *
          * Converts a string to a boolean
          *
          * @param str the string representation. If it starts with one of
          *        "true" or "yes" or is not equal zero, truth is returned.
          *        If it starts with one of "false", "no" or is zero, then
          *        false is returned.
          * @param unknown Specifies the return value if the input is
          *        unrecognised.
          */
        static bool string2bool(const std::string & str, bool unknown=false);


        /**
          * string2Location
          *
          * Converts the given string to one of the permissible LocationType
          * enumerated values.
          * @param s the string to convert
          * @return the LocationType value
          */
        static LocationType string2LocationType(const std::string & s);


        /**
          * Breaks the given string into tokens according to the delimiters.
          * Uses strtok().
          *
          * @note applies an arbitrary limit of 1024 characters to the input
          *       string (which matches the CommandMonitor line length limit)
          * 
          * @param s the string to tokenise
          * @param tokens the resulting list of strings
          * @param delimiters the delimiters given to strtok
          * @param maxtokens the maximum number of tokens into which to break
          *         the string. When the limit is one token from being reached,
          *         the untokenised remainder of the string is placed into the
          *         final token. Defaults to 0xFFFF, which given the input line
          *         limit essentially means no limit to the tokens.
          * @return the number of tokens
          */
        static int tokenise(const char * s, TokenList & tokens, const char * delimiters=" ", int maxtokens=0xFFFF);


        /**
          * Performs the opposite function of tokenise.  Concatenates the
          * strings in the given list, padding with the optional given string>
          * 
          * @param tokens the list of strings to concatenate
          * @param s the resulting string
          * @param padding the string to pad betwixt each string in the list
          */
        static void detokenise(TokenList & tokens, std::string & s, const char * padding=" ");


        /** expands a range of form "x-y" or "x" or "x-" into a set containing
          * the values x..y inclusive.
          *
          * @param r the string range expression
          * @param range the set to expand the range into
          * @param max the maximum value to expand the range to if no upper
          *            bound is specified in r
          */
        static void expandRange(const std::string & r, Range & range, unsigned int max);


        /** forms a range from the set into the form "x-y" or "x"
          *
          * performs the complement of the expandRange function, turning the
          * values in the set into a string range.  Currently, any discontinuities
          * in the set are ignored, effectively the string becomes
          * "range.min()-range.max()"
          *
          * @param range the set to expand the range into
          * @param r the string range expression
          */
        static void formRange(const Range & range, std::string & r);

        /** getByte
          * retrieves a byte from the network byte stream/vector
          * @param v the ByteVector stream
          * @param i the index within the vector
          * @return the retrieved byte
          */
        static Byte getByte(const ByteVector & v, int i);


        /** getWord
          * retrieves a word from the network word stream/vector
          * @param v the ByteVector stream
          * @param i the index within the vector
          * @return the retrieved word
          */
        static Word getWord(const ByteVector & v, int i);


        /** putByte
          * places a byte onto the network byte stream/vector
          * @param b the Byte to place
          * @param v the ByteVector stream
          */
        static void putByte(Byte b, ByteVector & v);


        /** putWord
          * places a word onto the network word stream/vector
          * @param w the Word to place
          * @param v the ByteVector stream
          */
        static void putWord(Word w, ByteVector & v);

        /**
          * random unsigned integer generation, within a given range.
          * @param lo the lower end of the range (default=0)
          * @param hi the upper end of the range (default=UINT_MAX=0xFFFFFFFF)
          * @return a random unsigned integer between 'lo' and 'hi'
          */
        static unsigned int random_uint32(unsigned int lo=0, unsigned int hi=UINT_MAX);


        /**
          * random Word generation, within a given range.
          * @param lo the lower end of the range (default=0)
          * @param hi the upper end of the range (default=USHRT_MAX=0xFFFF)
          * @return a random Word (unsigned short integer) between 'lo' and 'hi'
          */
        static Word randomWord(Word lo=0, Word hi=USHRT_MAX);


        /**
          * random unsigned character generation, within a given range.
          * @param lo the lower end of the range (default=0)
          * @param hi the upper end of the range (default=UCHAR_MAX=0xFF)
          * @return a random Byte (unsigned char) between 'lo' and 'hi'
          */
        static Byte randomByte(Byte lo=0, Byte hi=UCHAR_MAX);


        /** converts a TableOutputFormat to its human readable string
         *
         * @param f the TableOutputFormat to convert from
         * @return the std::string representation of the format
         */
        static std::string outputFormat2String(TableOutputFormat f);


        /** fills a ByteVector with dummy data
         *
         * @param v the ByteVector to fill
         * @return the number of characters filled
         */
        static int fill(ByteVector & v);

    
        /** Copy of core/process_management/src/UtilityInitialiser code,
          * copied so as to reduce dependence of Simulators on external libs.
          * @param argc command line argument count
          * @param argv command line argument vector
          */
        static bool initialiseUtilities( int argc, char* argv[] );


        /** Ensures that the host and port of the background "console" server
          * have valid values.  It determines this from:
          * 1. The values already in the strings (from the 'background' command)
          * 2. The RunParam "BackgroundHost"
          * 3. Hard-coded defaults: host="localhost", port="2341"
          * 
          * @param hoststr the host string
          * @param portstr the port string
          * @return true if all went ok. Which it always does, but if that
          *         turns out to be otherwise, its looked after.
          */
        static bool getBackgroundHostPort(std::string & hoststr, std::string & portstr);

    private:

        /**
          * Hidden constructor, as this class is not to be instantiated
          */
        GssUtility();

        /**
          * Hidden destructor
          */
        ~GssUtility();

    }; // class GssUtility



    inline Byte GssUtility::getByte(const ByteVector & v, int i)
    {
        return v[i];
    }


    inline Word GssUtility::getWord(const ByteVector & v, int i)
    {
        TwoByteUnion tbu;
        tbu.the_bytes[0] = v[i];
        tbu.the_bytes[1] = v[i+1];
        return (ntohs(tbu.the_word[0]));
    }


    inline void GssUtility::putByte(Byte b, ByteVector & v)
    {
        v.push_back(b);
    }


    inline void GssUtility::putWord(Word w, ByteVector & v)
    {
        TwoByteUnion tbu;
        tbu.the_word[0] = htons(w);
        v.push_back(tbu.the_bytes[0]);
        v.push_back(tbu.the_bytes[1]);
    }


} // namespace TA_Base_Bus
#endif // #ifndef GENERIC_SERVER_SIMULATOR_UTILITY_H
