/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/CrcCalculator.h $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * Calculates Cyclic Redundancy Checks from character data provided
  * via method calls.
  *
  */

#ifndef CrcCalculator_h
#define CrcCalculator_h

#include    <string>

namespace TA_Base_Bus
{
    class CrcCalculator
    {

    public:
        /**
          * CrcCalculator
          *
          * Constructor.
          */
        CrcCalculator( );

        /**
          * ~CrcCalculator
          *
          * Destructor.
          */
        virtual ~CrcCalculator();

        /**
          * clear
          * 
          * Clears the CRC calculation value (ready to start calculation
          * for another set of data.
          */
        void clear();

        /**
          * updateCrc
          *
          * Updates the CRC calculation based on the supplied data block.
          *
          * @param data         The data buffer to update the CRC result with.
          * @param dataSize     The number of bytes of data to use.
          */
        void updateCrc( const char* data, int dataSize );

        /**
          * updateCrc
          *
          * Updates the CRC calculation based on the supplied data block.
          *
          * @param data         The data buffer to update the CRC result with.
          */
        void updateCrc( const std::string data )
        {
            updateCrc( data.c_str(), data.size() );
        }

        /**
          * updateCrc
          *
          * Updates the CRC calculation based on the supplied data byte.
          *
          * @param data         The data byte to update the CRC result with.
          */
        void updateCrc( char data )     
        {
            updateCrc( &data, 1 ); 
        }

        /**
          * getCrc
          *
          * Gets the current value of the CRC calculations.
          **/
        unsigned short getCrc() const
        {
            return m_crc;
        };

    private:
        CrcCalculator( const CrcCalculator& );
        CrcCalculator& operator=( const CrcCalculator& );

    private:
        unsigned short m_crc;
    };
}

#endif // CrcCalculator_h
