#if !defined( VideoOutput_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define VideoOutput_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoOutput.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoOutput provides access to VideoOutput entity data and a specific framework
  * for VideoOutput entity parameters.
  * 
  * IMPORTANT: This class is used as a base helper class for all entities
  * that are VideoOutputs.
  */

#include "core/data_access_interface/entity_access/src/VideoSwitchResource.h"

namespace TA_Base_Core
{    
    class VideoOutput : public VideoSwitchResource
    {
    public:

        /**
          * VideoOutput
          *
          * Constructs a new VideoOutput object.
          *
          * @param      const std::string& entityType
          *             The type of this entity.
          */

        VideoOutput( const std::string& entityType = VO_DEFAULT_ENTITY_TYPE );

        /**
          * VideoOutput
          *
          * Constructs a new VideoOutput object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this VideoOutput in the database.
          * @param      const std::string& entityType
          *             The type of this entity.
          */

        VideoOutput( unsigned long key, const std::string& entityType = VO_DEFAULT_ENTITY_TYPE );

        /**
          * ~VideoOutput
          *
          * Standard destructor.
          */

        virtual ~VideoOutput() {};

        /**
          * getVideoOutputGroupName
          *
          * Gets the name of the group that this VideoOutput belongs to.
          * This name is used to group the VideoOutputs in the GUIs.
          *
          * @return     std::string
          *             The group name.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the VideoOutputGroupName parameter value 
          *             cannot be converted to a string.
          */

        std::string getVideoOutputGroupName();

        /**
          * setVideoOutputGroupName
          *
          * Sets the name of the group that this VideoOutput belongs to.
          * This name is used to group the VideoOutputs in the GUIs.
          *
          * @param      const std::string& name
          *             The group name.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setVideoOutputGroupName( const std::string& name );

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

        //virtual IEntityData* clone( unsigned long key );
                
    private:
        // 
        // The default entity type is used for VideoInputs that are not
        // derived from this class, but are of this class.
        // 

        static const std::string VO_DEFAULT_ENTITY_TYPE;

        //
        // The entity parameter name.
        //

        static const std::string VIDEO_OUTPUT_GROUP_NAME;

        //
        // Disable copy constructor and assignment operator
        //

        VideoOutput( const VideoOutput& theVideoOutput );
        VideoOutput& operator=( const VideoOutput& theVideoOutput );

    }; // VideoOutput

    
    typedef boost::shared_ptr<VideoOutput> VideoOutputPtr;
} // TA_Base_Core

#endif // !defined(VideoOutput_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
