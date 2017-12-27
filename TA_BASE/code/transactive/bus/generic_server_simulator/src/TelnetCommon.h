#ifndef TelnetCommon_H
#define TelnetCommon_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/TelnetCommon.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Contains declarations for basic types used for telnet command and option
  * usage
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssCommon.h"

#include <map>
#include <set>
#include <string>

namespace TA_Base_Bus
{
    namespace TelnetEndpoint
    {
        const bool Remote  = false;
        const bool Local   = true;
    } // namespace TelnetEndpoint

    class SimpleByteData
    {
    public:
        /// conversion constructor from unsigned char
        SimpleByteData(Byte b);
        /// conversion constructor from signed char
        SimpleByteData(signed char c);
        /// conversion constructor from int
        SimpleByteData(int i);

		/// value accessor
        Byte value() const;
		/// value accessor
        Byte data() const;
		/// value accessor
        Byte getValue() const;
        /// value accessor
        char getChar() const;

        /// equality with another SimpleByteData operator
        bool operator==(const SimpleByteData & rhs);
        /// equality with Byte operator
        bool operator==(Byte rhs);
        /// equality with signed char operator
        bool operator==(signed char rhs);
        /// equality with integer operator
        bool operator==(int rhs);

        /// assignment operator, but only from another SimpleByteData
        SimpleByteData & operator=(const SimpleByteData & rhs);

    protected:
		/// value mutator
        void setValue(Byte b);

    private:
		/// the value itself !!
        Byte m_value;

    }; // class SimpleByteData

    ///
    /// from the telnet protocol RFC0854, http://www.faqs.org/rfcs/rfc854.html
    ///
    class TelnetCommand : public SimpleByteData
    {
    public:
        /// conversion constructor from unsigned char
        TelnetCommand(Byte b);
        /// conversion constructor from signed char
        TelnetCommand(signed char c);
        /// conversion constructor from int
        TelnetCommand(int i);

        ///
        ///
        enum
        {
            EOR  = 238, // 0xEF End Of Record Marker
            SE   = 240, // 0xF0 End of subnegotiation parameters.
            NOP  = 241, // 0xF1 No operation.
            DM   = 242, // 0xF2 Data Mark. The data stream portion of a Synch.  This should always be accompanied by a TCP Urgent notification.
            BRK  = 243, // 0xF3 Break. NVT character BRK.
            IP   = 244, // 0xF4 Interrupt Process
            AO   = 245, // 0xF5 Abort output
            AYT  = 246, // 0xF6 Are You There?
            EC   = 247, // 0xF7 Erase Character
            EL   = 248, // 0xF8 Erase Line
            GA   = 249, // 0xF9 Go Ahead
            SB   = 250, // 0xFA Indicates that what follows is subnegotiation of the indicated option.
            WILL = 251, // 0xFB (option code) Indicates the desire to begin performing, or confirmation that you are now performing, the indicated option.
            WONT = 252, // 0xFC (option code) Indicates the refusal to perform, or continue performing, the indicated option.
            DO   = 253, // 0xFD (option code) Indicates the request that the other party perform, or confirmation that you are expecting the other party to perform, the indicated option.
            DONT = 254, // 0xFE (option code) Indicates the demand that the other party stop performing, or confirmation that you are no longer expecting the other party to perform, the indicated option.
            IAC  = 255  // 0xFF "Interpret as Command" escape character, to be followed by the code for the command
        };

        enum
        {
           	FLOW_OFF            = 0, //  Disable remote flow control
           	FLOW_ON	            = 1, //  Enable remote flow control
           	FLOW_RESTART_ANY    = 2, //  Restart output on any char
           	FLOW_RESTART_XON    = 3  //  Restart output only on XON
        };
    };

    static char * TelnetCommandStrings[] =
    {
        "SE", "NOP", "DM", "BRK", "IP", "AO", "AYT", "EC", "EL", "GA",
        "SB", "WILL", "WONT", "DO", "DONT", "IAC"
    };

    ///
    /// From telnet protocol option RFC0855 et. al.
    ///
    class TelnetOption : public SimpleByteData
    {
    public:
        /// conversion constructor from unsigned char
        TelnetOption(Byte b);
        /// conversion constructor from signed char
        TelnetOption(signed char c);
        /// conversion constructor from int
        TelnetOption(int i);

        enum
        {
            // "Expected to adopt and implement" Options 
            TransmitBinary      = 0,    // RFC 0856
            Echo                = 1,    // RFC 0857
            SuppressGoAhead     = 3,    // RFC 0858
            Status              = 5,    // RFC 0859
            TimingMark          = 6,    // RFC 0860
            NAOCRD              = 10,   // RFC 0652 Negotiate About Output Carriage-Return Disposition
            TerminalType        = 24,   // RFC 1091
            EndOfRecord         = 25,   // RFC 0885
            TerminalSpeed       = 32,   // RFC 1079
            RemoteFlowControl   = 33,   // RFC 1372
            ExtendedOptionsList = 255,  // RFC 0861

            // Suggested options
            Logout              = 18,   // RFC 0727
            DataEntryTerminal   = 20,   // RFC 1043 a.k.a. DODIIS
            Supdup              = 21,   // RFC 0736
            SendLocation        = 23,   // RFC 0779
            TacacsUserId        = 26,   // RFC 0927
            OutputMark          = 27,   // RFC 0933
            Regime3270          = 29,   // RFC 1041
            WindowSize          = 31,   // RFC 1073 a.k.a. NAWS
            LineMode            = 34,   // RFC 1184
            XDisplayLocation    = 35,   // RFC 1096
            Authentication      = 37,   // RFC 2941
            Encrypt             = 38,   // RFC 2946
            NewEnvironment      = 39,   // RFC 1572 (&c RFC 1571)
            Charset             = 42,   // RFC 2066
            Kermit              = 47,   // RFC 2840

            // Satirical. Or Satanical. You choose
            // (valid telnet options are 0-255)
            RandomlyLose        = 256,  // RFC 0748
            SubliminalMessage   = 257,  // RFC 1097

            // Arbitrary number to indicate this instance of TelnetOption
            // hasn't been assigned a valid option number yet.
            INVALID             = 222   // arbitrary
        } ;
    } ;

    static char * TelnetOptionStrings[40] =
    {
        "BINARY",  "ECHO",  "RCP",      "SGA",    "NAME",
        "STATUS",  "MARK",  "RCTE",     "NAOL",   "NAOP",
        "NAOCRD",  "NAOHTS","NAOHTD",   "NAOFFD", "NAOVTS",
        "NAOVTD",  "NAOLFD","EXTASCII", "LOGOUT", "BYTEMACRO",
        "DATATERM","SUPDUP","SUPDUPOUT","SENDLOC","TTYPE",
        "EODOFREC","TACACS","OUTPUTMRK","TTYLOC", "3270",
        "X.3PAD",  "NAWS",  "TSPEED",   "LFLOW",  "LINEMODE",
        "XDISPLOC","OLDENV","AUTH",     "ENCRYPT","NEWENV"
    };

    /// need "operator<" for insertion into map
    bool operator<(const TelnetOption & t1, const TelnetOption & t2);

    ///
    /// From the mind
    ///
    /// Note that this don't implement any assignment or equality operators
    /// even though it has an extra data member. That is because the extra
    /// data member, m_pending is not relevant to those comparisons.
    class TelnetOptionStatus : public SimpleByteData
    {
    public:
        /// conversion constructor from unsigned char
        TelnetOptionStatus(Byte b);
        /// conversion constructor from signed char
        TelnetOptionStatus(signed char c);
        /// conversion constructor from int
        TelnetOptionStatus(int i);
        /// conversion constructor from TelnetCommand
        TelnetOptionStatus(const TelnetCommand & cmd);
        /// need a default constructor to be able to place in a std::map
        TelnetOptionStatus();

        /** Used to determine if the value of an option is pending a response
          * from the remote client
          *
          * @return true if awaiting a response, false otherwise
          */
        bool isPending() const;

        /** Used to set the pending status of a telnet option.
          * Use clearPending to clear the status.
          */
        void setPending();

        /** Used to clear the pending status of a telnet option.
          * Use setPending to set the status.
          */
        void clearPending();

        enum
        {
            Will            = 23,
            Wont            = 25,
            Do              = 27,
            Dont            = 29,
            Invalid         = 111
        };

    private:
        /// a pending flag to determine if we've received a telnet option
        /// command as a response or unsolicited
        bool m_pending;

    }; // class TelnetOptionStatus


    ///
    ///
    typedef std::map< TelnetOption, TelnetOptionStatus > TelnetOptionMap;


    ///
    ///
    // typedef std::string TelnetBuffer;
    typedef ByteString TelnetBuffer;


    // ---------------------------------------------------------------------
    //
    // Inline Function Definitions
    // 
    // ---------------------------------------------------------------------
    inline Byte SimpleByteData::value() const
    {
        return m_value;
    }

    inline Byte SimpleByteData::data() const
    {
        return m_value;
    }

    inline Byte SimpleByteData::getValue() const
    {
        return m_value;
    }

    inline char SimpleByteData::getChar() const
    {
        return m_value;
    }

    inline void SimpleByteData::setValue(Byte b)
    {
        m_value = b;
    }

    inline bool SimpleByteData::operator==(const SimpleByteData & rhs)
    {
        return (m_value==rhs.m_value);
    }

    inline bool SimpleByteData::operator==(Byte rhs)
    {
        return (m_value==rhs);
    }

    inline bool SimpleByteData::operator==(signed char rhs)
    {
        return (m_value==rhs);
    }

    inline bool SimpleByteData::operator==(int rhs)
    {
        return (m_value==rhs);
    }

    inline SimpleByteData & SimpleByteData::operator=(const SimpleByteData & rhs)
    {
        //
        // not that self-assignment is bad, but it is to be avoided
        if ( &rhs != this )
        {
            m_value = rhs.m_value;
        }

        return *this;
    }

    inline bool operator<(const TelnetOption & t1, const TelnetOption & t2)
    {
        return (t1.value() < t2.value());
    }

    inline bool TelnetOptionStatus::isPending() const
    {
        return m_pending;
    }

    inline void TelnetOptionStatus::setPending()
    {
        m_pending = true;
    }

    inline void TelnetOptionStatus::clearPending()
    {
        m_pending = false;
    }

} // namespace TA_Base_Bus

#endif // #ifndef TelnetCommon_H
