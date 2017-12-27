#if !defined( TTISManagerEntityData_H__INCLUDED_ )
#define TTISManagerEntityData_H__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/CODE_TA_BASE_DDB/transactive/core/data_access_interface/entity_access/src/TTISManagerEntityData.h $
  * @author zhang hongzhi 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/03/27 14:57:34 $
  * Last modified by: $Author: trainteam $
  * 
  * TTISManagerEntityData provides access to TTISManagerEntityData entity data 
  * and a specific framework for TTISManagerEntityData entity parameters.
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class TTISManagerEntityData : public EntityData
    {
    public:
        /**
          * TTISManagerEntityData
          *
          * Constructs a new TTISManagerEntityData object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        TTISManagerEntityData();

        /**
          * TTISManagerEntityData
          *
          * Constructs a new TTISManagerEntityData object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this TTISManagerEntityData in the database.
          */

        TTISManagerEntityData( unsigned long key );

        /**
          * ~TTISManagerEntityData
          *
          * Standard destructor.
          */

        virtual ~TTISManagerEntityData() {};

        /**
          * getMaxScheduleMessage
          *
          * Gets the maximum number that TTIS Manager will queue message to train.  The number should be
          * limited by hardware, default is 6.
          *
          * @return     int
          *             Max schedule message number limit.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the AutoTTISDirectory parameter cannot be converted to a string.
          */

        int getMaxScheduleMessage();
        

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getType
          *
          * Returns the type of this entity. This method must be callable when the object
          * is constructed using the default constructor. The entity type string should be
          * stored as a static const variable in the concrete class. The entity type should
          * not be loaded from the database (note that this method defines no exceptions).
          *
          * @return      std::string
          *              The entity type of this entity.
          */

	    virtual std::string getType();
	    static std::string getStaticType();

        /**
          * clone
          *
          * Returns a new instance of this entity. This method is used to create a concrete
          * entity based upon type in the EntityAccessFactory.
          *
          * @return     IEntityData*
          *             A new instance of this entity.  It is the caller's responsibility
          *             to clean up the object.
          *
          * @param      unsigned long key 
          *             The pkey of this entity in the database.
          */ 

        static IEntityData* clone( unsigned long key );
                
        /**
          * invalidate
          *
          * Marks the data contained by this entity as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate();

    protected:
         


    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;
       
        //
        // Disable copy constructor and assignment operator
        //

        TTISManagerEntityData( const TTISManagerEntityData& theTTISManagerEntityData );
        TTISManagerEntityData& operator=( const TTISManagerEntityData& theTTISManagerEntityData );

        /** 
          * validateData
          *
          * Reloads the data if invalidate() has been called.
          *
          * Preconditions:  0 != getHelper()
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if a parameter value cannot be converted to the required type.
          */

        void validateData();
   
        /** 
          * setParameter
          *
          * Sets the value for the named string parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      const std::string& value
          *             The new parameter value.
          * @param      std::string& parameter
          *             The stored parameter to be set.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setParameter( const std::string& name, const std::string& value );

        //
        // The directories to which data is TTISd, including the full path. 
        //


    }; // TTISManagerEntityData

    typedef boost::shared_ptr<TTISManagerEntityData> TTISManagerEntityDataPtr;
} // TA_Base_Core

#endif // !defined(TTISManagerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
