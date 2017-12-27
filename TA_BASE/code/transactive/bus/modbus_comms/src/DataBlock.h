/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/modbus_comms/src/DataBlock.h $
  * @author  A. Ivankovic
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2015/01/21 17:58:35 $
  * Last modified by : $Author: haijun.li $
  *
  * A block of raw data which has a start address, an end address
  * and a length. This class is a template and the type of data in the
  * block is defined as a template argument. For example DataBlock<bool>
  * defines a block of booleans and DataBlock<WORD> defines a block of
  * word values. Note that the data block range is inclusive of the start
  * and end address.
  *
  */

#ifndef DataBlock_h
#define DataBlock_h

#include <vector>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;

namespace TA_Base_Bus
{
    template< class DataType > class DataBlock
    {

    public:
        /**
          * DataBlock
          *
          * Constructor.
          */
        DataBlock()
            : m_start( 0 ), m_end( 0 )
        {
        }

        /**
          * DataBlock
          *
          * Constructor which defines a block with the specified range.
          */
        DataBlock( int start, int end )
        {
            setRange( start, end );
        }

        /**
          * Copy Constructor.
          *
          * Constructs this data block based on another.
          *
          * @param block    The data block to be copied.
          */
	    DataBlock( const DataBlock< DataType > & block ) 
	    {
            *this = block;
	    }

        /**
          * start
          *
          * The start address of the data block.
          *
          * @return        The start address.
          */
        int start() const
        {
            return m_start;
        }

        /**
          * end
          *
          * The end address of the data block. Note that the end address
          * is included in the data block range.
          *
          * @return        The end address.
          */
        int end() const
        {
            return m_end;
        }

        /**
          * length
          *
          * The length of the data block. Note that the end address
          * is included in the data block range, hence the length is
          * equal to end() - start() + 1.
          *
          * @return        The block length.
          */
        int length() const
        {
            return m_end - m_start + 1;
        }

        /**
          * setStart
          *
          * Sets the start address and length of the data block.
          *
          * @param start    The start address of the data block.
          * @param length   The length of the data block.
          */
        void setStartAndLength( int start, int length )
        {
            TA_ASSERT( length > 0, "The length must be positive and non-zero." );

            m_start = start;
            m_end = m_start + length - 1;
            m_data.resize( length );
        }

        /**
          * setRange
          *
          * Sets the start and end address of the data block. Note that the
          * end address is included in the data block range.
          *
          * @param start    The start address of the data block.
          * @param end      The end address of the data block.
          */
        void setRange( int start, int end )
        {
            TA_ASSERT( end >= start, "The end address must be greater than or equal to the start." );

            m_start = start;
            m_end = end;
            m_data.resize( length() );
        }

        /**
          * Array Operator
          *
          * This array operator is const and only allows for the retrieval of
	      * data block element values.
          *
          * @param address  An address index into the data block.
          * @return         The element at the specified address.   
          *
          * Note that this array operator returns the data type by value rather
          * than a const reference which would have been more typical. This
          * was done because of complications with the vector<bool> template
          * instantiation having different array operator semantics from
          * other vector type instantiations.
          */
        DataType operator[] ( int address ) const
	    {
			//TD# 2399: Commented out TA_VERIFY
            //TA_VERIFY( address >= m_start, "The address must be >= the start address" );
            //TA_VERIFY( address <= m_end, "The address must be <= the end address" );

            if ( false == isInRange( address ) )
            {
                return 0;
            }

            return m_data[ address - m_start ];
	    }

        /**
          * set
          *
          * Sets element values within the data block.
          *
          * @param address  An address index into the data block.
          * @param value    The data value to use. 
          *
          * Note that a set method is supplied rather than an non-const
          * array operator because of complications with the vector<bool>
          * template instantiation having different array operator semantics from
          * other vector type instantiations.
          */
	    void set( int address, DataType value ) 
	    {
            TA_ASSERT( address >= m_start, "The address must be >= the start address" );
            TA_ASSERT( address <= m_end, "The address must be <= the end address" );

		    m_data[ address - m_start ] = value;
	    }

        /**
          * Assignment Operator
          *
          * Assigns the value of this data block from another.
          *
          * @param block    The data block to be copied.
          */
	    void operator=( const DataBlock< DataType >&  block ) 
	    {
            m_start = block.m_start;
            m_end = block.m_end;
            m_data = block.m_data;
	    }

        /**
          * Equality Operator
          *
          * Checks whether the contents of this data block is equivalent to another.
          * Note that the start address, end address and data contents must match
          * to be considered equal.
          *
          * @param block    The data block to be compared.
          */
        bool operator==( const DataBlock< DataType >& block ) const
        {
            return m_start == block.m_start &&
                   m_end == block.m_end &&
                   m_data == block.m_data;
        }

        /**
          * Inequality Operator
          *
          * Checks whether the contents of this data block are different to another.
          * Note that the start address, end address and data contents must match
          * to be considered equal.
          *
          * @param block    The data block to be compared.
          */
        bool operator!=( const DataBlock< DataType >& block ) const
        {
            return !( *this == block );
        }

        /**
          * setAll
          *
          * Sets the entire contents of the data block to the specified
          * value.
          */
        void setAll( DataType value )
        {
            for( int i= m_start; i <= m_end; ++i )
            {
                set( i, value );
            }
        }

        /**
          * isInRange
          *
          * Determines whether the supplied address is in the addressing range
          * of this data block.
          *
          * @param address  The address to check.
          * @return         Whether the address is in range.
          */
        bool isInRange( int address ) const
        {
            return ( address >= m_start && address <= m_end );
        }

       /**
          * setDataByDataBlock
          *
          * set the data use other datablock
          *
          * @param dataBlock a data block to be set
          * 
          *
          */

		void setDataByDataBlock(const DataBlock& dataBlock)
		{
			TA_ASSERT( dataBlock.m_start >= m_start, "The address must be >= the start address" );
            TA_ASSERT( dataBlock.m_end <= m_end, "The address must be <= the end address" );
			TA_ASSERT( dataBlock.m_end >= dataBlock.m_start, "The start address must be <= the end address" );
			
			std::copy(dataBlock.m_data.begin(),dataBlock.m_data.end(),m_data.begin()+(dataBlock.m_start - m_start));
		}

	/**
          * getDataBlock
          *
          * get a smaller datablock from the host data block;
          *
          * @param rtnDataBlockstart the return datablock whose address range must be between the host address range
          * 
          *
          */
		void getDataBlock(DataBlock& rtnDataBlock) const
		{
			TA_ASSERT( rtnDataBlock.m_start >= m_start, "The address must be >= the start address" );
			TA_ASSERT( rtnDataBlock.m_end <= m_end, "The address must be <= the end address" );
			TA_ASSERT( rtnDataBlock.m_end >= rtnDataBlock.m_start, "The start address must be <= the end address" );

			std::copy(m_data.begin()+(rtnDataBlock.m_start - m_start), m_data.begin() + (rtnDataBlock.m_end - m_start +  1), rtnDataBlock.m_data.begin());
		}
    private:
        int m_start;
        int m_end;
        std::vector< DataType > m_data;
    };
}

#endif // DataBlock_h
