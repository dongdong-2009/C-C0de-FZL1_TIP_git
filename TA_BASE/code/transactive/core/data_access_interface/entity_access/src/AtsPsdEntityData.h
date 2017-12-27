/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AtsPsdEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifndef AtsPsd_ENTITY_DATA_H_INCLUDED
#define AtsPsd_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"


#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class AtsPsdEntityData :  public EntityData
    {

    public:
        /**
         * AtsPsdEntityData (constructor)
         *
         * Constructs a new AtsPsdEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        AtsPsdEntityData() {};

        /**
         * AtsPsdEntityData (constructor)
         *
         * Constructs a new AtsPsdEntityData object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        AtsPsdEntityData(unsigned long key);

        virtual ~AtsPsdEntityData();

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

    protected:

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        AtsPsdEntityData( const AtsPsdEntityData& theAtsPsdEntityData) {};            
		AtsPsdEntityData& operator=(const AtsPsdEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;


        bool m_isValidData;

    };

    typedef boost::shared_ptr<AtsPsdEntityData> AtsPsdEntityDataPtr;
} //namespace TA_Base_Core

#endif // #ifndef AtsPsd_ENTITY_DATA_H_INCLUDED
