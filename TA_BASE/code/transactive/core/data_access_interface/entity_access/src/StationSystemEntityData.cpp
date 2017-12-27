/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationSystemEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/22 10:48:00 $
  * Last modified by: $Author: haijun.li $
  * 
  * StationSystemEntityData implements the interface IStationSystemEntityData to provide
  * read-only access to Station System entity data.
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/exceptions/src/GenericAgentException.h"   //wenching

namespace TA_Base_Core
{

    const std::string StationSystemEntityData::ENTITY_TYPE = "StationSystem";
	const std::string StationSystemEntityData::STATIONSYSTEM_SLAVE_ID = "SlaveID";
	const std::string StationSystemEntityData::STATUS_WORD_NUMBER = "StatusWordNumber";
	const std::string StationSystemEntityData::POLLING_START_ADDRESS = "SVRPollingTableStartAddress";
    const std::string StationSystemEntityData::POLLING_END_ADDRESS = "SVRPollingTableEndAddress";
	const std::string StationSystemEntityData::OUTPUT_START_BIT_ADDRESS = "OutputStartBitAddress";
    const std::string StationSystemEntityData::OUTPUT_END_BIT_ADDRESS = "OutputEndBitAddress";
	const std::string StationSystemEntityData::OUTPUT_START_WORD_ADDRESS = "OutputStartWordAddress";
    const std::string StationSystemEntityData::OUTPUT_END_WORD_ADDRESS = "OutputEndWordAddress";
	const std::string StationSystemEntityData::OUTPUT_SERVICE_NUMBER = "OutputServiceNumber";
	const std::string StationSystemEntityData::BLOCK_START_ADDRESS = "BlockStartAddress"; //TD16182
	const std::string StationSystemEntityData::BLOCK_END_ADDRESS = "BlockEndAddress"; //TD16182
	const std::string StationSystemEntityData::STATIONSYSTEM_GROUP_ID ="StationSystemGroupID";
	const std::string StationSystemEntityData::DIGITAL_WRITE_FUN_CODE = "DigitalWriteFunCode";

	StationSystemEntityData::StationSystemEntityData()
    {
    }


    StationSystemEntityData::StationSystemEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    StationSystemEntityData::~StationSystemEntityData()
    {
	
    }


	int StationSystemEntityData::getSlaveID()
	{
		char * stopString = "\0";
		int base = 10;

		int slaveID = 0;

		std::string value =  getHelper()->getParameter(STATIONSYSTEM_SLAVE_ID);
		if (value.empty())
		{
			slaveID = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}

			slaveID = strtoul( value.c_str(), &stopString, base );
		}

        return slaveID;
	}

	int StationSystemEntityData::getStatusWordNumber()
	{
		return getIntParameterValue(STATUS_WORD_NUMBER);
	}

    unsigned long StationSystemEntityData::getOutputStartBitAddress()
	{
		char * stopString = "\0";
		int base = 10;

		std::string value =  getHelper()->getParameter(OUTPUT_START_BIT_ADDRESS);
		unsigned long outputStartBitAddress = 0;
		if (value.empty())
		{
			outputStartBitAddress = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}
			
			outputStartBitAddress = strtoul( value.c_str(), &stopString, base );
		}

        return outputStartBitAddress;
	}


    unsigned long StationSystemEntityData::getOutputEndBitAddress()
	{
		char * stopString = "\0";
		int base = 10;

		std::string value =  getHelper()->getParameter(OUTPUT_END_BIT_ADDRESS);
		unsigned long outputEndBitAddress = 0;
		if (value.empty())
		{
			outputEndBitAddress = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}
			
			outputEndBitAddress = strtoul( value.c_str(), &stopString, base );
		}

		return outputEndBitAddress;
	}


    unsigned long StationSystemEntityData::getOutputStartWordAddress()
	{
		char * stopString = "\0";
		int base = 10;

		std::string value =  getHelper()->getParameter(OUTPUT_START_WORD_ADDRESS);
		unsigned long outputStartWordAddress = 0;
		if (value.empty())
		{
			outputStartWordAddress = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}
			
			outputStartWordAddress = strtoul( value.c_str(), &stopString, base );
		}

        return outputStartWordAddress;
	}


    unsigned long StationSystemEntityData::getOutputEndWordAddress()
	{
		char * stopString = "\0";
		int base = 10;

		std::string value =  getHelper()->getParameter(OUTPUT_END_WORD_ADDRESS);
		unsigned long outputEndWordAddress = 0;
		if (value.empty())
		{
			outputEndWordAddress = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}
			
			outputEndWordAddress = strtoul( value.c_str(), &stopString, base );
		}

        return outputEndWordAddress;
	}


    const std::string StationSystemEntityData::getOutputServiceNumber()
	{
        return getHelper()->getParameter(OUTPUT_SERVICE_NUMBER);
	}

	const std::string StationSystemEntityData::getDigitalWriteFunCode()
	{
		return getHelper()->getParameter(DIGITAL_WRITE_FUN_CODE);
	}

	unsigned long StationSystemEntityData::getPollingStartAddress()
	{
		char * stopString = "\0";
		int base = 10;

		std::string value =  getHelper()->getParameter(POLLING_START_ADDRESS);
		unsigned long pollingStartAddress = 0;
		if (value.empty())
		{
			pollingStartAddress = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}

			pollingStartAddress = strtoul( value.c_str(), &stopString, base );
		}

        return pollingStartAddress;
	}


	unsigned long StationSystemEntityData::getPollingEndAddress()
	{
		char * stopString = "\0";
		int base = 10;

		std::string value =  getHelper()->getParameter(POLLING_END_ADDRESS);
		unsigned long pollingEndAddress = 0;
		if (value.empty())
		{
			pollingEndAddress = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}
			
			pollingEndAddress = strtoul( value.c_str(), &stopString, base );
        }

        return pollingEndAddress;
	}

	const std::vector<std::pair<unsigned long, unsigned long> >& StationSystemEntityData::getBlockAddresses()
	{
		char * stopString = "\0";
		int base = 10;
		std::map<int, std::pair<unsigned long, unsigned long> > blockAddressesMap; //wenching

		// Initialise vector, wenching
		m_blockAddresses.clear();

		// 16182 for m_blockStartAddresses and m_blockEndAddresses
		EntityHelper::EntityParameterNameValueMap& allParameterValues = getHelper()->getAllParameters();
		EntityHelper::EntityParameterNameValueMapIter it = allParameterValues.begin();

		for (; it != allParameterValues.end(); ++it)
		{
			int index=0;
			bool isStartAddress = false;
			bool isEndAddress = false;
			
			// for block start address
			std::string::size_type posStartAddressPrefix = it->first.find( BLOCK_START_ADDRESS );

			if (posStartAddressPrefix != std::string::npos)
			{
				if (posStartAddressPrefix == 0)
				{
					std::string indexString = it->first.substr ( BLOCK_START_ADDRESS.length(), it->first.length() );
					std::istringstream ssIndex (indexString);
					ssIndex >> index;

					isStartAddress = true;
				}
				else
				{
					std::ostringstream desc;
					desc << "Parameter [" << it->first << "] not configured properly, should start with "
						 << BLOCK_START_ADDRESS;
					TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
				}
			}

			std::string::size_type posEndAddressPrefix = it->first.find( BLOCK_END_ADDRESS );
					
			if ( posEndAddressPrefix != std::string::npos) 
			{	
				if (posEndAddressPrefix == 0)
				{
					std::string indexString = it->first.substr ( BLOCK_END_ADDRESS.length(), it->first.length() );
					std::istringstream ssIndex (indexString);
					ssIndex >> index;

					isEndAddress = true;
				}
				else
				{
					std::ostringstream desc;
					desc << "Parameter [" << it->first << "] not configured properly, should start with "
						 << BLOCK_END_ADDRESS;
					TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
				}
			}

			if ( false == isStartAddress && false == isEndAddress )
			{
				continue;
			}

			unsigned long address = 0;
				
			if (it->second.empty())
			{
				address = 0;
			}
			else
			{
				// check if the first 2 characters from the string data are "0x"
				if ( ( "0x" == it->second.substr( 0, 2 ) ) || ( "0X" == it->second.substr( 0, 2 ) ) )
				{
					// hexadecimal number
					base = 16;
				}
				else
				{
					// decimal number
					base = 10;
				}
				address = strtoul( it->second.c_str(), &stopString, base );
			}

			if (true == isStartAddress)
			{
				blockAddressesMap[index-1].first = address;	
			}
			else if (true == isEndAddress)
			{
				blockAddressesMap[index-1].second = address;	
			}
		}

		std::map<int, std::pair<unsigned long, unsigned long> >::iterator Iter;
		for (Iter = blockAddressesMap.begin(); Iter != blockAddressesMap.end(); ++Iter)
		{
			if ( (*Iter).second.first >= 0 && (*Iter).second.second > 0 )
			{
				m_blockAddresses.push_back((*Iter).second);
			}
		}

		blockAddressesMap.clear();

        return m_blockAddresses;
	}

    IEntityData* StationSystemEntityData::clone ( unsigned long key )
    {
        return new StationSystemEntityData ( key );
    }

	std::string StationSystemEntityData::getType()
	{
		return getStaticType();
	}

    std::string StationSystemEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    void StationSystemEntityData::invalidate()
    {
        getHelper()->invalidate();
    }
	int StationSystemEntityData::getIntegerData ( const std::string & parameterName )
	{
		int data = 0;
		
		std::string stringValue = getHelper()->getParameter(parameterName);
		if (!stringValue.empty())
		{
			data = EntityHelper::getIntegerData(stringValue);
		}
		
		return data;
	}
	int StationSystemEntityData::getGroupID()
	{
		return getIntegerData ( STATIONSYSTEM_GROUP_ID ); 
	}

	int StationSystemEntityData::getIntParameterValue( const std::string& parameterName )
	{
		char * stopString = "\0";
		int base = 10;

		int intValue = 0;

		std::string value =  getHelper()->getParameter(parameterName);
		if (value.empty())
		{
			intValue = 0;
		}
		else
		{
			// check if the first 2 characters from the string data are "0x"
			if ( ( "0x" == value.substr ( 0, 2 ) ) ||
				( "0X" == value.substr ( 0, 2 ) ) )
			{
				// hexadecimal number
				base = 16;
			}
			else
			{
				// decimal number
				base = 10;
			}

			intValue = strtoul( value.c_str(), &stopString, base );
		}
		return intValue;
	}
} //close namespace TA_Base_Core
