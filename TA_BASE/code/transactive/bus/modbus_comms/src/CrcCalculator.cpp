/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/CrcCalculator.cpp $
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

#include    "core/utilities/src/TAAssert.h"
#include    "bus/modbus_comms/src/CrcCalculator.h"

using namespace std;

namespace TA_Base_Bus
{
    CrcCalculator::CrcCalculator()
    {
        clear();
    }


    CrcCalculator::~CrcCalculator()
    {
    }


    void CrcCalculator::clear()
    {
        m_crc = 0xffff;
    }


    void CrcCalculator::updateCrc( const char* data, int dataSize )
    {
        TA_ASSERT( data != 0, "Data buffer is null" );
        TA_ASSERT( dataSize > 0, "Data size is not positive" );

        unsigned short carry;
        unsigned short temp1;
        unsigned short temp2;
        unsigned short temp3;
        unsigned short temp4;

        for ( int i=0; i<dataSize; i++ )
        {
            temp1 = m_crc & 0x00ff;
            temp4 = (unsigned short) data[i];
            temp4 &= 0x00ff;
            temp2 = temp1 ^ (temp4 & 0x00ff);
            temp3 = m_crc & 0xff00;
            m_crc = temp3 | temp2;
            for (int j=0; j<8; j++)
            {
                carry = m_crc & 0x0001;
                m_crc >>= 1;
                if (carry) m_crc ^= 0xa001;
            }
        }
    }
}
