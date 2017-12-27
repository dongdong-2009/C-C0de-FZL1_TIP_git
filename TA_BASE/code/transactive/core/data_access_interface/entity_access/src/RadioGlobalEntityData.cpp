/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioGlobalEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * <description>
  *
  */

#include "RadioGlobalEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "boost/tokenizer.hpp"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string RadioGlobalEntityData::ENTITY_TYPE                    = "RadioAgentSettings";
    const std::string RadioGlobalEntityData::XON_XOFF_TIMEOUT               = "XONXOFFTimeout";
    const std::string RadioGlobalEntityData::CIRCUIT_DATA_FRAMES_PER_PACKET  = "CircuitDataFramesPerPacket";
    const std::string RadioGlobalEntityData::NORMAL_CALL_PRIORITY           = "NormalCallPriority";
    const std::string RadioGlobalEntityData::EMERGENCY_CALL_PRIORITY        = "EmergencyCallPriority";
    const std::string RadioGlobalEntityData::SERIALISE_TRANSACTION_LEVEL     = "SerialiseTransactionLevel";
//  const std::string RadioGlobalEntityData::NORMAL_CALL_DEFAULT_PRIORITY    = "OutgoingNormalCallPriority";
//  const std::string RadioGlobalEntityData::EMERGENCY_CALL_DEFAULT_PRIORITY = "OutgoingEmergencyCallPriority"; 
//  const std::string RadioGlobalEntityData::MULTI_PAGE_SDS_HEADER_TEXT      = "MultiPageSdsHeader";
//  const std::string RadioGlobalEntityData::MULTI_PAGE_SDS_PAGE_LIMIT       = "MultiPageSdsPageLimit";

    RadioGlobalEntityData::RadioGlobalEntityData(unsigned long key)
            :EntityData(key, getType())
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "RadioGlobalEntityData (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "RadioGlobalEntityData (constructor)");        
    }     

	
    RadioGlobalEntityData::RadioGlobalEntityData()
        : EntityData()
    {
    }  


    RadioGlobalEntityData::~RadioGlobalEntityData()
    {
        
    }


    IEntityData* RadioGlobalEntityData::clone(unsigned long key)
    {
        return new RadioGlobalEntityData(key);                
    }


    /**
     * getType
     *
     * Returns the console type of this console (i.e. "Console").
     *
     * @return The console type of this console as a std::string
     */
    std::string RadioGlobalEntityData::getType()
    {
        return getStaticType();
    }


    std::string RadioGlobalEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    RadioGlobalEntityData* RadioGlobalEntityData::getRadioGlobalsEntity()
    {
		LOG( SourceInfo, DebugUtil::FunctionEntry, "getRadioGlobalsEntity" );

        IEntityDataList entityArray;
        RadioGlobalEntityData* radioGlobalEntity = NULL;

        try
        {
            entityArray = EntityAccessFactory::getInstance().getEntitiesOfType(getStaticType());
			unsigned long nSize = entityArray.size();

            if ( entityArray.size() != 1 )
            {
                TA_THROW(DataException("There can be only one Entity NAME",
                    DataException::NOT_UNIQUE,
                    "NAME"));
            }
            radioGlobalEntity = new RadioGlobalEntityData(entityArray.at(0)->getKey());

            // clean up
            for (IEntityDataList::iterator it = entityArray.begin(); it != entityArray.end(); ++it)
            {
                delete *it;
            }
            entityArray.clear();

        } 
        catch( const DatabaseException& e )
        {
            // clean up
            for (IEntityDataList::iterator it = entityArray.begin(); it != entityArray.end(); ++it)
            {
                delete *it;
            }
            entityArray.clear();
            
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );            
            TA_THROW(e);
        }
        catch( const DataException& e )
        {
            // clean up
            for (IEntityDataList::iterator it = entityArray.begin(); it != entityArray.end(); ++it)
            {
                delete *it;
            }
            entityArray.clear();
            
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );            
            TA_THROW(e);
        }
        catch (...)
        {
            // clean up
            for (IEntityDataList::iterator it = entityArray.begin(); it != entityArray.end(); ++it)
            {
                delete *it;
            }
            entityArray.clear();
            
            throw;
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "getRadioGlobalsEntity");

        return radioGlobalEntity;        
    }


    /**
     * invalidate
     *
     * Mark the data contained by this console as invalid. The next call to get...() 
     * following a call to invalidate() will cause all the data to be reloaded from
     * the database.
     */
    void RadioGlobalEntityData::invalidate()
    {
        getHelper()->invalidate();
    }


    unsigned long RadioGlobalEntityData::getXonXoffTimeout()
    {
		std::string value = getHelper()->getParameter(XON_XOFF_TIMEOUT);
		unsigned long xonXoffTimeout = 10;
		if (!value.empty())
		{
			xonXoffTimeout = EntityHelper::getUnsignedLongData(value);
		}
		
        return xonXoffTimeout;
    }


    unsigned long RadioGlobalEntityData::getCircuitDataFramesPerPacket()
    {
		std::string value = getHelper()->getParameter( CIRCUIT_DATA_FRAMES_PER_PACKET );
		
        unsigned long framesPerPacket = 1;
		
        if ( false == value.empty() )
		{
			framesPerPacket = EntityHelper::getUnsignedLongData( value );
		}
		
        return framesPerPacket;
    }


#if 0	//TD17997 marc_bugfix change implementation 
    RadioGlobalEntityData::RadioPriority RadioGlobalEntityData::getNormalCallDefaultPriority()
    {
		std::string value = getHelper()->getParameter(NORMAL_CALL_DEFAULT_PRIORITY);
		RadioPriority normalCallPriorityDefault = Priority1;
		if (!value.empty())
		{
			int paramValue = EntityHelper::getIntegerData(value);
			normalCallPriorityDefault = convertParameterToRadioPriority(paramValue);
		}

        return normalCallPriorityDefault;
 
    }

    RadioGlobalEntityData::RadioPriority RadioGlobalEntityData::getEmergencyCallDefaultPriority()
    {
		std::string value = getHelper()->getParameter(EMERGENCY_CALL_DEFAULT_PRIORITY);
		RadioPriority emergencyCallPriorityDefault = Priority1;
		if (!value.empty())
		{
			int paramValue = EntityHelper::getIntegerData(value);                    
			emergencyCallPriorityDefault = convertParameterToRadioPriority(paramValue);
		}
        
        return emergencyCallPriorityDefault;
 
    }    

    std::vector<RadioGlobalEntityData::RadioPriority> RadioGlobalEntityData::getAllowableNormalCallPriorities()
    {

		//NormalCallPriority would be in a range from 1-15
		std::string value = getHelper()->getParameter(NORMAL_CALL_PRIORITIES);
		if (!value.empty())
		{
			// The list of allowable call priorities for normal calls, 
			// all values in the range 1..15. Sorted, no duplicates
			return getRadioPriorities(value);
		}

		std::vector<RadioPriority> normalCallPriorities;
        return normalCallPriorities;

    }

    std::vector<RadioGlobalEntityData::RadioPriority> RadioGlobalEntityData::getAllowableEmergencyCallPriorities()
    {
		std::string value = getHelper()->getParameter(EMERGENCY_CALL_PRIORITIES);
		if (!value.empty())
		{
			// The list of allowable call priorities for emergency calls,
			// all values in the range 1..15. Sorted, no duplicates
			return getRadioPriorities(value);
		}

		std::vector<RadioPriority> emergencyCallPriorities;
        return emergencyCallPriorities;
    }
#else
    RadioGlobalEntityData::RadioPriority RadioGlobalEntityData::getNormalCallPriority()
    {
		std::string value = getHelper()->getParameter(NORMAL_CALL_PRIORITY);
		RadioPriority normalCallPriority = Priority1;
		if (!value.empty())
		{
			int paramValue = EntityHelper::getIntegerData(value);
			normalCallPriority = convertParameterToRadioPriority(paramValue);
		}

        return normalCallPriority;
 
    }

    RadioGlobalEntityData::RadioPriority RadioGlobalEntityData::getEmergencyCallPriority()
    {
		std::string value = getHelper()->getParameter(EMERGENCY_CALL_PRIORITY);
		RadioPriority emergencyCallPriority = Priority1;
		if (!value.empty())
		{
			int paramValue = EntityHelper::getIntegerData(value);                    
			emergencyCallPriority = convertParameterToRadioPriority(paramValue);
		}
        
        return emergencyCallPriority;
 
    }    

#endif    

#if 0	//For now delete implementation, unused
    unsigned long RadioGlobalEntityData::getMultiPageSdsPageLimit()
    {
		std::string value = getHelper()->getParameter(MULTI_PAGE_SDS_PAGE_LIMIT);
		// The max. number of pages to send in 1 SDS message 1..9
        unsigned long multiPageSdsPageLimit = 0;
		if (!value.empty())
		{
            
			multiPageSdsPageLimit = EntityHelper::getUnsignedLongData(value);
		}

        return multiPageSdsPageLimit;
    }


    std::string RadioGlobalEntityData::getMultiPageSdsHeaderTemplate()
    {
		// The header text to prepend on multi page SDS, it
		// separates page number and page count. 
		// eg. "[#%]" will give "[1/3]", "# of %" will give "1 of 3"
		return getHelper()->getParameter(MULTI_PAGE_SDS_HEADER_TEXT);
    }    
#endif

    unsigned char RadioGlobalEntityData::getSerialiseTransactionLevel()
    {
        std::string value = getHelper()->getParameter(SERIALISE_TRANSACTION_LEVEL);
		
		unsigned char serialiseTransactionLevel = 9; // default value (all)

		if (!value.empty())
		{
			serialiseTransactionLevel = static_cast<unsigned char>( EntityHelper::getIntegerData(value) );
		}
        
        return serialiseTransactionLevel;
    }
    std::vector<std::string> RadioGlobalEntityData::tokenizeParameterValue(std::string parameterValue)
    {
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        
        std::vector<std::string> parts;    
        boost::char_separator<char> separator(",");

        tokenizer tokens(parameterValue, separator);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }
        
        // if parts is empty, then this should return the entire string
        if (parts.size() == 0)
        {
            parts.push_back(parameterValue);
        }
        
        return parts;
    }

	
    std::vector<RadioGlobalEntityData::RadioPriority> RadioGlobalEntityData::getRadioPriorities(std::string parameterValue)
    {
        // The parameterValue may be in a format "1-3,5,7"
        // So the logic is :
        //      Breaks first the string by "," delimiter
        //      Those in ranges (1-3), gets the number and loops to get the number within.
        //      Check in priorityList if exist, else store
        //      Sort 
        //      Convert all contents of priorityList and store in radioPriorities

        std::vector<RadioGlobalEntityData::RadioPriority> radioPriorities;
        std::vector<unsigned int> priorityList;

        // Breaks first the string by "," delimiter        
        std::vector<std::string> commaParts = tokenizeParameterValue (parameterValue);
                
        if ( (commaParts.size() > 0) && (commaParts[0] != "") )
        {
            for (std::vector<std::string>::iterator commaIter = commaParts.begin();
                 commaIter  != commaParts.end(); commaIter++)
            {
                std::string item = commaIter ->c_str();
                // Those in ranges (1-3), gets the number and loops to get the number within.                
				if (item.find("-") != std::string::npos)
                {
                    std::string::size_type dashPos = item.find_first_of("-", 0 );

                    std::string::size_type first = atoi(item.substr(0,dashPos).c_str());
                    std::string::size_type last = atoi(item.substr(dashPos + 1, item.length()).c_str());
                    for (size_t i = first; i <= last; i++)
                    {
                        if ( std::find(priorityList.begin(), priorityList.end(),i) == priorityList.end() )
                        {    
                            priorityList.push_back(i);
                        }
                    }
                }
                else
                {
                    // Add in list if not yet exist
                    unsigned int nItem = static_cast<unsigned int>(atoi(item.c_str()));
                    if ( std::find(priorityList.begin(), priorityList.end(),nItem) == priorityList.end() )
                    {                        
                        priorityList.push_back(nItem);
                    }
                }
            }
        }

        // Sort
        std::sort(priorityList.begin(), priorityList.end());

        // Convert to RadioPriority type
        for(unsigned int i = 0;i < priorityList.size();i++)
        {
            RadioGlobalEntityData::RadioPriority radioPriority = convertParameterToRadioPriority(priorityList[i]); 
            radioPriorities.push_back(radioPriority);
        }
            
        return radioPriorities;
    }

    RadioGlobalEntityData::RadioPriority RadioGlobalEntityData::convertParameterToRadioPriority(int paramValue)
    {
        RadioPriority priority;

        switch (paramValue)
        {
			case Priority0:
				priority = Priority0;
				break;
            case Priority1:
                priority = Priority1;
                break;
            case Priority2:
                priority = Priority2;
                break;                
            case Priority3:
                priority = Priority3;
                break;                
            case Priority4:
                priority = Priority4;
                break;                                
            case Priority5:
                priority = Priority5;
                break;                
            case Priority6:
                priority = Priority6;
                break;                
            case Priority7:
                priority = Priority7;
                break;                
            case Priority8:
                priority = Priority8;
                break;                
            case Priority9:
                priority = Priority9;
                break;                
            case Priority10:
                priority = Priority10;
                break;                
            case Priority11:
                priority = Priority11;
                break;                
            case Priority12:
                priority = Priority12;
                break;                
            case Priority13:
                priority = Priority13;
                break;                
            case Priority14:
                priority = Priority14;
                break;                
            case Priority15:
                priority = Priority15;
                break;                
            default:
                //TA_THROW( DataException("No conversion specified for Radio Priority", DataException::NO_VALUE, "Radio Priority") );                                
				//If data is out of bounds, do not throw an exception.
				//just set the data at the extremes
				if (paramValue < Priority1)
				{
					priority = Priority1;
				}
				else
				{
					priority = Priority15;
				}
        }
        return priority;
    }
}


