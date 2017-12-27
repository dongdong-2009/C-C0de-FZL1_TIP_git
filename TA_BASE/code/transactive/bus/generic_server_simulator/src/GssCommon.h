#ifndef GENERIC_SERVER_SIMULATOR_COMMON_H
#define GENERIC_SERVER_SIMULATOR_COMMON_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssCommon.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for typedefs, structs and constants common to Generic
  * Server Simulator classes.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <set>
#include <list>
#include <string>
#include <vector>

namespace TA_Base_Bus
{
    /**
      * Byte
      *
      * The same as unsigned char.  Would have liked to go via the ta_types
      * ta_uint8 to ensure cross-platformability, however on Windows it maps
	  * to "unsigned __int8" which apparently isn't the same as unsigned char,
      * or at least is different enough to stop VC++ disallowing conversion
      * between types templated on it.
      */
    typedef unsigned char Byte;

    /**
      * ByteArray
      *
      * Essentially the same as unsigned char *, but via the ta_types
      * unsigned 8-bit type.
      */
    typedef Byte * ByteArray;

    /**
      * ByteString
      *
      * Essentially the same as std::string, but templated on the ta_types
      * unsigned 8-bit type.
      */
    typedef std::basic_string<Byte, std::char_traits<Byte>, std::allocator<Byte> > ByteString;

    /**
      * ByteVector
      *
      * Template for an STL vector templated on the unsigned 8-bit type.
      */
    typedef std::vector<Byte> ByteVector;



    /**
      * Word
      *
      * The same as unsigned short int.
      */
    typedef unsigned short int Word;

    /**
      * WordPtr
      *
      * Essentially the same as unsigned short int *
      */
    typedef Word * WordPtr;

    /**
      * WordString
      *
      * Essentially the same as std::string, but templated on unsigned short int
      */
    typedef std::basic_string<Word, std::char_traits<Word>, std::allocator<Word> > WordString;

    /**
      * WordVector
      *
      * Template for an STL vector templated on the unsigned 16-bit type.
      */
    typedef std::vector<Word> WordVector;


    /** An enumeration representing the 3 types of C830 locations. Some of the
      * identifiers share the same value, being synonyms for that location.
      *
      * We can't just
      *
      * #include "core/data_access_interface/src/ILocation.h"
      *
      * because it only contains three locations, whereas we sometimes need to
      * differentiate between the three STATION locations.
      */
    typedef enum LocationType
    {
        OCC     = 1,
        DPT     = 2,
        DEPOT   = 2,    // synonymous with DPT
        MSS     = 3,
        SS1     = 4,
        SS2     = 5,
        STATION = 6,

        NO_LOCATION=-1  // an erroneous value
    };


    ///
    /// enumeration of values for the format for outputing tables.
    typedef enum TableOutputFormat
    {
        f_hex          = 1,
        f_human        = 2,
        f_default      = f_human
    };


    /** Sloppy union for the purposes of streaming Words to and from a
      * ByteVector.
      */
    union TwoByteUnion
    {
        Byte            the_bytes[2];
        Word            the_word[1];
    };

    /** Sloppy union for the purposes of streaming long Words to and from a
      * ByteVector.
      */
    union FourByteUnion
    {
        Byte            the_bytes[4];
        Word            the_words[2];
        unsigned long   the_long[1];
    };

    class AsciiControlCode
    {
        public:
        // 
        // from http://www.jimprice.com/jim-asc.htm#controlcodes
        //
        enum
        {
            NUL = 0x00, //        (null)                   
            SOH = 0x01, // Ctrl-A (start of heading)       
            STX = 0x02, // Ctrl-B (start of text)          
            ETX = 0x03, // Ctrl-C (end of text)   
            EOT = 0x04, // Ctrl-D (end of transmission) - Not the same as ETB    
            ENQ = 0x05, // Ctrl-E (enquiry)                
            ACK = 0x06, // Ctrl-F (acknowledge)            
            BEL = 0x07, // Ctrl-G (bell) - Caused teletype machines to ring a bell.  Causes a beep in many common terminals and terminal emulation programs.
            BS  = 0x08, // Ctrl-H (backspace) - Moves the cursor (or print head) move backwards (left) one space.
            TAB = 0x09, // Ctrl-I (horizontal tab) - Moves the cursor (or print head) right to the next tab stop.  The spacing of tab stops is dependent on the output device, but is often either 8 or 10.
            LF  = 0x0A, // Ctrl-J (NL line feed, new line) - Moves the cursor (or print head) to a new line.  On Unix systems, moves to a new line AND all the way to the left.
            VT  = 0x0B, // Ctrl-K (vertical tab)           
            FF  = 0x0C, // Ctrl-L (form feed) - Advances paper to the top of the next page (if the output device is a printer).
            CR  = 0x0D, // Ctrl-M (carriage return) - Moves the cursor all the way to the left, but does not advance to the next line.
            SO  = 0x0E, // Ctrl-N (shift out) - Switches output device to alternate character set.            
            SI  = 0x0F, // Ctrl-O (shift in)  - Switches output device back to default character set.
            DLE = 0x10, // Ctrl-P (data link escape)       
            DC1 = 0x11, // Ctrl-Q (device control 1)       
            DC2 = 0x12, // Ctrl-R (device control 2)       
            DC3 = 0x13, // Ctrl-S (device control 3)       
            DC4 = 0x14, // Ctrl-T (device control 4)       
            NAK = 0x15, // Ctrl-U (negative acknowledge)   
            SYN = 0x16, // Ctrl-V (synchronous idle)       
            ETB = 0x17, // Ctrl-W (end of transmission block) - Not the same as EOT  
            CAN = 0x18, // Ctrl-X (cancel)                 
            EM  = 0x19, // Ctrl-Y (end of medium)  
            SUB = 0x1A, // Ctrl-Z (substitute)             
            ESC = 0x1B, //        (escape) 
            FS  = 0x1C, //        (file separator)
            GS  = 0x1D, //        (group separator)
            RS  = 0x1E, //        (record separator)
            US  = 0x1F  //        (unit separator)         
        }; // anonymous enum
    }; // class AsciiCode

    ///
    /// 
    const int DEFAULT_VERBOSITY = 3;

    ///
    /// type definition for a list of strings, used by GssUtility::tokenise()
	/// and detokenise()
    typedef std::list<std::string> StringList;

    ///
    /// type definition for a list of strings, used by GssUtility::tokenise()
	/// and detokenise()
    typedef std::list<std::string> TokenList;

    ///
    /// type definition for a set of unsigned integers, used by
	/// GssUtility::formRange() and expandRange()
    typedef std::set< unsigned int > Range;

} // namespace TA_Base_Bus
#endif // #ifndef GENERIC_SERVER_SIMULATOR_COMMON_H
