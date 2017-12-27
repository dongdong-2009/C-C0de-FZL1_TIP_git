/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioGlobalEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifndef RADIOGLOBALENTITYDATA_H
#define RADIOGLOBALENTITYDATA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include <vector>

namespace TA_Base_Core
{   
    class RadioGlobalEntityData : public EntityData  
    {
        public:

            enum RadioPriority
            {
				Priority0,
                Priority1,
                Priority2,
                Priority3,
                Priority4,
                Priority5,
                Priority6,
                Priority7,
                Priority8,
                Priority9,
                Priority10,
                Priority11,
                Priority12,
                Priority13,
                Priority14,
                Priority15                
            };
            
            static RadioGlobalEntityData* getRadioGlobalsEntity();

            RadioGlobalEntityData();

            RadioGlobalEntityData(unsigned long key);

            virtual ~RadioGlobalEntityData();                        

            /**
             * clone
             *
             * Returns a pointer to a new Console object that has been initialised with the 
             * specified key.
             *
             * @param key The key with which to construct the Console object.
             */
            static IEntityData* clone(unsigned long key);

            /**
             * getType
             *
             * Returns the console type of this console (i.e. "Console").
             *
             * @return The console type of this console as a std::string
             */
            virtual std::string getType();

		    static std::string getStaticType();

            /**
             * invalidate
             *
             * Mark the data contained by this console as invalid. The next call to get...() 
             * following a call to invalidate() will cause all the data to be reloaded from
             * the database.
             */
            virtual void invalidate();           

            unsigned long getXonXoffTimeout();

            unsigned long getCircuitDataFramesPerPacket();
#if 0	    //TD17997 marc_bugfix change behavior for radio globals
            RadioPriority getNormalCallDefaultPriority();

            RadioPriority getEmergencyCallDefaultPriority();

            std::vector<RadioPriority> getAllowableNormalCallPriorities();

            std::vector<RadioPriority> getAllowableEmergencyCallPriorities();
#else
            RadioPriority getNormalCallPriority();

            RadioPriority getEmergencyCallPriority();
#endif

//            unsigned long getMultiPageSdsPageLimit();

//            std::string getMultiPageSdsHeaderTemplate();
            unsigned char getSerialiseTransactionLevel();
            
        private:

            //
            // Copy constructor and assignment operator are not used
            //
            RadioGlobalEntityData( const RadioGlobalEntityData& theRadio) {};            
            RadioGlobalEntityData& operator=(const RadioGlobalEntityData &) {};

            // Priorities can be in format 1-3, 5, 7

            // Break first the parameter value on ","
            std::vector<std::string> tokenizeParameterValue(std::string parameterValue);

            // Break strings in format 1-3
            std::vector<RadioPriority> getRadioPriorities(std::string parameterValue);

            // Used for conversions
            RadioPriority convertParameterToRadioPriority(int paramValue);

            // 
            // The entity type is fixed for all instances of this class.
            // 
            static const std::string ENTITY_TYPE;
            static const std::string XON_XOFF_TIMEOUT;
            static const std::string CIRCUIT_DATA_FRAMES_PER_PACKET;
            static const std::string NORMAL_CALL_PRIORITY;
            static const std::string EMERGENCY_CALL_PRIORITY;
#if 0
            static const std::string NORMAL_CALL_DEFAULT_PRIORITY;
            static const std::string EMERGENCY_CALL_DEFAULT_PRIORITY;
#endif
			static const std::string SERIALISE_TRANSACTION_LEVEL;

            // The list of allowable call priorities for emergency calls,
            // all values in the range 1..15. Sorted, no duplicates
            std::vector<RadioPriority> emergencyCallPriorities ;


  //          static const std::string MULTI_PAGE_SDS_HEADER_TEXT;
  //          static const std::string MULTI_PAGE_SDS_PAGE_LIMIT;

    };
    
    typedef boost::shared_ptr<RadioGlobalEntityData> RadioGlobalEntityDataPtr;
}
#endif 

