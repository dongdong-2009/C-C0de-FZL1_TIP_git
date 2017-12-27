#if !defined( TisLogViewerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define TisLogViewerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TisLogViewerEntityData.h $
  * @author Adam Radics 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * TisLogViewerEntityData provides access to TisLogViewerEntityData entity data 
  * and a specific framework for TisLogViewerEntityData entity parameters.
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class TisLogViewerEntityData : public EntityData
    {
    public:

        /**
          * TisLogViewerEntityData
          *
          * Constructs a new TisLogViewerEntityData object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */
        TisLogViewerEntityData();

        /**
          * TisLogViewerEntityData
          *
          * Constructs a new TisLogViewerEntityData object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this TisLogViewerEntityData in the database.
          */
        TisLogViewerEntityData( unsigned long key );

        /**
          * ~TisLogViewerEntityData
          *
          * Standard destructor.
          */
        virtual ~TisLogViewerEntityData() {};

        /**
          * getRefreshIntervalInSecs
          *
          * Gets the period for the refreshing of new display logs from the database
          *
          * @return     unsigned long
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the DefaultDwellTime parameter cannot be converted to a string.
          */
        unsigned long getRefreshIntervalInSecs();
        
        /**
          * setRefreshIntervalInSecs
          *
          * Sets the period for the refreshing of new display logs from the database
          *
          * @param      unsigned long
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for the DefaultDwellTime parameter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */
        void setRefreshIntervalInSecs( unsigned long interval );



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

    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;
       
        //
        // Disable copy constructor and assignment operator
        //

        TisLogViewerEntityData( const TisLogViewerEntityData& theTisLogViewerEntityData );
        TisLogViewerEntityData& operator=( const TisLogViewerEntityData& theTisLogViewerEntityData );
   
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

        void setParameter( const std::string& name, const std::string& value);
    }; // TisLogViewerEntityData

    
    typedef boost::shared_ptr<TisLogViewerEntityData> TisLogViewerEntityDataPtr;
} // TA_Base_Core

#endif // !defined(TisLogViewerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
