#if !defined( ArchiveManagerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define ArchiveManagerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ArchiveManagerEntityData.h $
  * @author Katherine Thomson 
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/05/25 17:45:22 $
  * Last modified by: $Author: raghu.babu $
  * 
  * ArchiveManagerEntityData provides access to ArchiveManagerEntityData entity data 
  * and a specific framework for ArchiveManagerEntityData entity parameters.
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class ArchiveManagerEntityData : public EntityData
    {
    public:
        /**
          * ArchiveManagerEntityData
          *
          * Constructs a new ArchiveManagerEntityData object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        ArchiveManagerEntityData();

        /**
          * ArchiveManagerEntityData
          *
          * Constructs a new ArchiveManagerEntityData object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this ArchiveManagerEntityData in the database.
          */

        ArchiveManagerEntityData( unsigned long key );

        /**
          * ~ArchiveManagerEntityData
          *
          * Standard destructor.
          */

        virtual ~ArchiveManagerEntityData() {};

        /**
          * isArchiveManagerAlreadyRunning
          *
          * This method checks if there is an Archive Manager already connected to the schema
          * this Archive Manager is connected to.
          *
          * KT 3 Mar 04, PW #3136: Added to allow only a single instance of the Archive Manager.
          * 
          * @param      std::string& 
          *             This will be populated with the name of the workstation already running the
          *             Archive Manager. This is only populated if the return value is true.
          *
          * @return     bool
          *             True if a Archive Manager is already running. 
          *             False if not.
          */

        bool isArchiveManagerAlreadyRunning( std::string& workstationName );

        /**
          * getAutoArchiveDirectory
          *
          * Gets the directory to which data is automatically archived.  The files should be
          * copied from here onto the archive media (CD).
          *
          * @return     std::string
          *             The archive directory including the full path.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the AutoArchiveDirectory parameter cannot be converted to a string.
          */

        std::string getAutoArchiveDirectory();
        
        /**
          * setAutoArchiveDirectory
          *
          * Sets the directory to which data is automatically archived.  The files should be
          * copied from here onto the archive media (CD).
          *
          * @param      const std::string& autoAutoArchiveDirectory
          *             The archive directory including the full path.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for the AutoArchiveDirectory parameter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setAutoArchiveDirectory( const std::string& autoArchiveDirectory );

		// yangguang++ TD16713
		std::string getVolumeofDVD();
		void setVolumeofDVD( const std::string& autoArchiveDirectory );
		// ++yangguang TD16713

        /**
          * getManualArchiveDirectory
          *
          * Gets the directory to which data is manually archived.  The files should be
          * copied from here onto the archive media (CD).
          *
          * @return     std::string
          *             The archive directory including the full path.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the ManualArchiveDirectory parameter cannot be converted to a string.
          */

        std::string getManualArchiveDirectory();
        
        /**
          * setManualArchiveDirectory
          *
          * Sets the directory to which data is manually archived.  The files should be
          * copied from here onto the archive media (CD).
          *
          * @param      const std::string& manualManualArchiveDirectory
          *             The archive directory including the full path.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for the ManualArchiveDirectory parameter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setManualArchiveDirectory( const std::string& manualArchiveDirectory );

        /**
          * getDatabaseSystemUserName
          *
          * Gets the username of the System user for the database.
          *
          * @return     std::string
          *             The system user name.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the DatabaseSystemUserName parameter cannot be converted to a string.
          */

			
		/**
          * getImpFromUser
          *
          * Gets the user name in the exported data which has to be imported. 
          *
          * @return     std::string
          *             The user name in the exported data.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the FromUser parameter cannot be converted to a string.
          */
		std::string getImpFromUser();

		/**
          * getImpToUser
          *
          * Gets the user name to which the data has to be imported. 
          *
          * @return     std::string
          *             The user name of the database where the data has to be imported.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the FromUser parameter cannot be converted to a string.
          */
		
		 std::string getImpToUser();

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

        ArchiveManagerEntityData( const ArchiveManagerEntityData& theArchiveManagerEntityData );
        ArchiveManagerEntityData& operator=( const ArchiveManagerEntityData& theArchiveManagerEntityData );

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
        // The directories to which data is archived, including the full path. 
        //


    }; // ArchiveManagerEntityData

    typedef boost::shared_ptr<ArchiveManagerEntityData> ArchiveManagerEntityDataPtr;
} // TA_Base_Core

#endif // !defined(ArchiveManagerEntityData_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
