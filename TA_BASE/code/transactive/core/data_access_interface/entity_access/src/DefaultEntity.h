/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DefaultEntity.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * This is an implemntation of the EntityData abstract class and is the "base" or "default"
 * entity. Any entity in the system can be instantiated as one of these.
 */

#if !defined(AFX_DEFAULTENTITY_H__2A9328BA_D93C_4A09_B794_5DFA39C1B936__INCLUDED_)
#define AFX_DEFAULTENTITY_H__2A9328BA_D93C_4A09_B794_5DFA39C1B936__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER 

#include "core/data_access_interface/entity_access/src/EntityData.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    class DefaultEntity : public EntityData  
    {
    public:

        DefaultEntity()
        {
        }


        ~DefaultEntity()
        {
        }


        DefaultEntity(unsigned long key,const std::string& type) 
            : EntityData(key,type)
        {
        }


        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();

        /**
         * clone
         *
         * Returns a pointer to a new Camera object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the Camera object.
         */
        IEntityData* clone(unsigned long key)
        {
            TA_ASSERT(false,"This method is not used in the Default Entity class");
            return NULL;        
        }

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        
        /**
         * getParameterValue
         *
         * Returns the value of an entity parameter.
         *
         * @param name The name of the entity parameter to retrieve
         *
         * @return The value for the entity parameter
         *
         * @exception DataException - Thrown if the parameters cannot be loaded or if there is no parameter
         *                            matching the name passed in
         * @exception DatabaseException - Thrown if the parameters could not be read from the database
         */
        std::string getParameterValue(const std::string& name);

    protected:


    private:

        DefaultEntity(DefaultEntity&);
        DefaultEntity& operator=(DefaultEntity&);
	};

    typedef boost::shared_ptr<DefaultEntity> DefaultEntityPtr;

} // cose namespace TA_Base_Core

#endif // !defined(AFX_DEFAULTENTITY_H__2A9328BA_D93C_4A09_B794_5DFA39C1B936__INCLUDED_)
