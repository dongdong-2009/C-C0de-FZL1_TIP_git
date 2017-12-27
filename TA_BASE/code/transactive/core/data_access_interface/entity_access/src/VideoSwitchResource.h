#if !defined( VideoSwitchResource_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define VideoSwitchResource_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoSwitchResource.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * VideoSwitchResource provides access to VideoSwitchResource entity data and a specific framework
  * for VideoSwitchResource entity parameters. It inherits from EntityData.
  *
  * IMPORTANT: This class is used as a base helper class for all entities
  * that are VideoSwitchResources.
  *
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class VideoSwitchResource : public EntityData
    {
    public:
        /**
          * VideoSwitchResource
          *
          * Constructs a new VideoSwitchResource object.
          *
          * @param      const std::string& entityType
          *             The type of this entity.
          */

        VideoSwitchResource( const std::string& entityType = VSR_DEFAULT_ENTITY_TYPE );

        /**
          * VideoSwitchResource
          *
          * Constructs a new VideoSwitchResource object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this VideoSwitchResource in the database.
          * @param      const std::string& entityType
          *             The type of this entity.
          */

        VideoSwitchResource( unsigned long key, const std::string& entityType = VSR_DEFAULT_ENTITY_TYPE );
       
        /**
          * ~VideoSwitchResource
          *
          * Standard destructor.
          */

        virtual ~VideoSwitchResource() {};

        /** 
          * setDescription
          *
          * Sets the description field of the standard Entity Data for this Entity.
          *
          * @param      std::string
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException Thrown if the a parameter name cannot be found, or if
          *             there is more than one value for a parmeter, or if the
          *             the entity key is invalid (and this is not a new entity).     
          */

        void setDescription( const std::string& description );

		std::string getAssetName();
        
        ///////////////////////////////////////////////////////////////////
        // 
        // Supported Features
        //
        ///////////////////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////////////////
        // 
        // State Parameters 
        //
        ///////////////////////////////////////////////////////////////////
        

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData methods
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

        /**
          * clone
          *
          * Returns a pointer to a new VideoSwitchResource object that has been initialised with the 
          * specified key.
          *
          * @return     IEntityData*
          *             A new instance of this entity.  It is the caller's responsibility
          *             to clean up the object.
          *
          * @param  unsigned long key 
          *         The key with which to construct the VideoSwitchResource object.
          */

        //virtual IEntityData* clone( unsigned long key );
        
        /**
          * invalidate
          *
          * Marks the data contained by this entity as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate();

    protected:

        /** 
          * setParameter
          *
          * Sets the value for the named bool parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      bool
          *             The new parameter value.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setParameter( const std::string& name, bool value);

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
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setParameter( const std::string& name, const std::string& value);

        /** 
          * setParameter
          *
          * Sets the value for the named unsigned short parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      unsigned short
          *             The new parameter value.
          * @param      unsigned short& parameter
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

        void setParameter( const std::string& name, unsigned short value);
        
        // 
        // The entity type is NOT fixed for all instances of this class.  This
        // is different to most EntityData objects.  The reason for this change
        // is that VideoSwitchResource will generally be used as a base class to provide
        // access to all data that is common to VideoSwitchResources.
        // 

        const std::string m_entityType;

        //
        // Defines how true and false are defined for entity parameter values.
        //

        static const std::string PARAMETER_TRUE;
        static const std::string PARAMETER_FALSE;

    private:
        // 
        // The default entity type is used for VideoSwitchResources that are not
        // derived from this class, but are of this class.
        // 

        static const std::string VSR_DEFAULT_ENTITY_TYPE;
		static const std::string ASSET_NAME;

        //
        // Disable copy constructor and assignment operator
        //

        VideoSwitchResource( const VideoSwitchResource& theVideoSwitchResource );
        VideoSwitchResource& operator=( const VideoSwitchResource& theVideoSwitchResource );

    }; // VideoSwitchResource

    
    typedef boost::shared_ptr<VideoSwitchResource> VideoSwitchResourcePtr;
} // TA_Base_Core

#endif // !defined(VideoSwitchResource_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
