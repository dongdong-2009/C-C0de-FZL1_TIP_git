/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TTISEntityData.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * TTISEntityData is a concrete implementation of ITTISEntityData.
  * It proves read only access to TTIS entity data
  *
  */

#ifndef TTIS_ENTITY_DATA_H_INCLUDED
#define TTIS_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/ITTISEntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class TTISEntityData : public ITTISEntityData, public EntityData
    {

    public:
        /**
         * TTISEntityData (constructor)
         *
         * Constructs a new TTISEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        TTISEntityData() {};

        /**
         * TTISEntityData (constructor)
         *
         * Constructs a new TTISEntityData object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        TTISEntityData(unsigned long key);

        virtual ~TTISEntityData();

    public:

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

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        TTISEntityData( const TTISEntityData& theTTISData) {};            
		TTISEntityData& operator=(const TTISEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
    };

    
    typedef boost::shared_ptr<TTISEntityData> TTISEntityDataPtr;
} //namespace TA_Base_Core

#endif // #ifndef TTIS_ENTITY_DATA_H_INCLUDED
