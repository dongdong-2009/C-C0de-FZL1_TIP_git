/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/TA_String.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#ifndef TA_STRING_H_INCLUDED
#define TA_STRING_H_INCLUDED

// INTELLECTUAL PROPERTY NOTICE:
// Elements of the following library are based on work previously implemented by 
// the author, Lachlan Partington.  No restrictions are placed on Ripple Systems 
// relating to the use of this code, however the author asserts his right to use 
// these and similar implementations in his future work.


#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <sstream>
#include <string>
#include <vector>

#include <list>
#include <map>
#include <stdarg.h>

namespace TA_Base_Core
{
    // Shorthand names for unsigned integral types
    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned long ulong;


    // Parses a comma-separated string and returns the tokens in a vector.
    // TODO LPP: Surely a Tokenise() method for tokenising a string with an arbitrary
    // separator character has already been written elsewhere...
    void gTokeniseCommaSeparatedString(const std::string& str, std::vector<std::string>& tokens);


    // CONVERSIONS TO A STRING

    const unsigned int MAXMESGSIZE = 2000;

    // Converts a printf-style string to a std::string, substituting arguments in 'args'
    // into 'formatStr'. The string AFTER expansion must be < MAXMESGSIZE characters, 
    // otherwise substitution won't be done.
    // If an error occurs, false *might be* returned (or a core dump might occur).  
    // A plain-english description of the error will be assigned to *errorStr if 
    // errorStr is non-null.
    bool toString(va_list args, const char* formatStr, std::string& result, std::string* errorStr = NULL);

    // Helper to write a sequence of elements to a stream. For example, "{a, b, c, }".
    template <class Iter>
    std::ostream& writeToStream(std::ostream& stm, Iter first, Iter last)
    {
        stm << "{";

        while (first != last)
        {
            stm << *first << ", ";
            ++first;
        }

        stm << "}";

        return stm;
    }

    // Converts a sequence of elements to a string.  For example, "{a, b, c, }"
    template <class Iter>
    std::string toString(Iter first, Iter last)
    {
        std::ostringstream stm;
        writeToStream(stm, first, last);
        return stm.str();
    }


    template <class T1, class T2>
    std::ostream& operator<<(std::ostream& stm, const std::pair<T1, T2>& arg)
    {
        stm << "(" << arg.first << ", " << arg.second << ")";  
        return stm;
    }

    template <class T>
    std::ostream& operator<<(std::ostream& stm, const std::vector<T>& cont)
    {
        return writeToStream(stm, cont.begin(), cont.end());
    }

    template <class T>
    std::ostream& operator<<(std::ostream& stm, const std::list<T>& cont)
    {
        return writeToStream(stm, cont.begin(), cont.end());
    }

    template <class U, class V>
    std::ostream& operator<<(std::ostream& stm, const std::map<U, V>& cont)
    {
        return writeToStream(stm, cont.begin(), cont.end());
    }


    // TODO LPP: operator<< for set, deque, multimap, multiset
    // TODO LPP: AsHex


    template <class T>
    struct AsPtr_t
    {
        AsPtr_t(T* p) : m_p(p) {}
        T* m_p;
    };

    template <class T>
    AsPtr_t<T> AsPtr(T* p)
    {
        return AsPtr_t<T>(p);
    }

    // Converts a T* to a string of the form "0xADDRESS (TYPENAME)".
    // Note that the pointer must NOT be to a deleted object, as it is
    // dereferenced to get the typename.
    template <class T>
    std::ostream& operator<<(std::ostream& stm, const AsPtr_t<T>& obj)
    {
        // Difference between formatting of hex numbers in stream output on Windows and Solaris:
        // SOLARIS: 0xcf8840 
        // WINDOWS: 00D50C90        (no leading '0x')
	    #if !defined(SOLARIS)
            stm << "0x";
        #endif
        
        stm << std::hex << obj.m_p << " (";

        if (obj.m_p == NULL)
        {
			try
			{
				// Output the type of the pointer (eg. T* has type T)
				stm << typeid(obj.m_p).name(); 
			}
			catch (...)
			{
			}
        }
        else
        {
			try
			{
				// Output the most derived type of the object
				stm << typeid(*obj.m_p).name();
			}
			catch (...)
			{
			}
        }

        stm << ")";

        return stm;
    }

    template <class T>
    struct AsDeletedPtr_t
    {
        AsDeletedPtr_t(T* p) : m_p(p) {}
        T* m_p;
    };

    template <class T>
    AsDeletedPtr_t<T> AsDeletedPtr(T* p)
    {
        return AsDeletedPtr_t<T>(p);
    }

    template <class T>
    std::ostream& operator<<(std::ostream& stm, const AsDeletedPtr_t<T>& obj)
    {
        // TODO LPP: Remove this environment-specific behaviour once SIGBUS/SIGSEGV signals 
        // have been caught for Solaris.  In fact, this can be done in the operator<<(T*)
        // overload above.
	    #if defined(SOLARIS)
            stm << std::hex << obj.m_p << " (" << typeid(obj.m_p).name() << ") (POSSIBLY DELETED)";
        #else
            // On Windows, an access violation will occur if we tried to dereference an unitialised 
            // pointer (eg. 0xCDCDCDCD). The access violation will be translated to a structured 
            // exception, which can be caught with catch(...).  
            try
            {
                stm << AsPtr(obj.m_p);
            }
            catch (...)
            {
                stm << ") (DELETED)";
            }
        #endif
        
        return stm;
    }


    // Converts any type T to a string, providing an appropriate overload of
    // operator<<(ostream&, T) exists.
    // For example, it can be used for the basic types (char, short, etc),
    // pairs, vectors, maps, etc.

    // Note that it is preferable to add operator<< overloads rather than new
    // versions of toString, because operator<< is more efficient for 
    // building up strings.
    template <class T>
    std::string toString(const T& t)
    {
        std::ostringstream stm;
        stm << t;
        return stm.str();    
    }


    // CONVERSIONS FROM A STRING

    // fromString() attempts to convert the string 's' to 'result'.  If an error 
    // occurs, false is returned.  A plain-english description of the error will  
    // be assigned to *errorStr if errorStr is non-null.
    // Common conversion errors are:
    // - OVERFLOW:      String too long to fit into destination type. Eg. "abc" -> char, "70000" -> ushort
    // - WRONG_TYPE:    String contains characters which can't be converted to the destination type.
    //                  Eg. "123a" -> ushort
    // - WRONG_FORMAT:  String doesn't contain required format placeholders.  Eg. "{a, b" -> pair

/*
    bool fromString(const std::string& s, char& result,         std::string* errorStr = NULL);
    bool fromString(const std::string& s, short& result,        std::string* errorStr = NULL);
    bool fromString(const std::string& s, ushort& result,       std::string* errorStr = NULL);
    bool fromString(const std::string& s, int& result,          std::string* errorStr = NULL);
    bool fromString(const std::string& s, uint& result,         std::string* errorStr = NULL);
    bool fromString(const std::string& s, long& result,         std::string* errorStr = NULL);
    bool fromString(const std::string& s, ulong& result,        std::string* errorStr = NULL);
    bool fromString(const std::string& s, float& result,        std::string* errorStr = NULL);
    bool fromString(const std::string& s, double& result,       std::string* errorStr = NULL);
*/
    bool fromString(const std::string& s, std::string& result,  std::string* errorStr = NULL);

    // TODO LPP: fromString() for pair, vector, list, map 


    // This version of fromString() can be used inline in expressions.  
    // If an error occurs in the conversion, a default-constructed instance
    // of type T will be returned.
    template <class T>
    T fromString(const std::string& s)
    {
        T t;
        fromString(s, t);
        return t;
    }


    // OBSOLETE
    // Provided for backwards compatibilty

    // Converts the given unsigned long to a std::string.
	inline std::string gStringFromUnsignedLong(unsigned long number)
    {
        return toString(number);
    }

    // Returns a string of the form "(first, second)"
    inline std::string gStringFromPair(const std::string& first, const std::string& second)
    {
        std::pair<std::string, std::string> pair(first, second);
        return toString(pair);
    }

    template <class T>
    std::string gPtrToStr(T* t)
    {
        return toString(AsPtr(t));
    }

    // TODO LPP: Remove this function once SIGBUS/SIGSEGV signals have been caught for Solaris
    // by operator<<(T*) implementation. 
    template <class T>
    std::string gPtrToStr_PossiblyDeletedHACK(T* t)
    {
        return toString(AsDeletedPtr(t));
    }

}		// namespace TA_Base_Core

#endif // TA_STRING_H_INCLUDED
