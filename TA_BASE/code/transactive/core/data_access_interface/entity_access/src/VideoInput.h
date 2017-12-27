#if !defined( VideoInput_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define VideoInput_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoInput.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoInput provides access to VideoInput entity data and a specific framework
  * for VideoInput entity parameters. It inherits from EntityData.
  *
  * IMPORTANT: This class is used as a base helper class for all entities
  * that are VideoInputs.
  */

#include "core/data_access_interface/entity_access/src/VideoSwitchResource.h"

namespace TA_Base_Core
{
    class VideoInput : public VideoSwitchResource
    {
    public:

        /**
          * VideoInput
          *
          * Constructs a new VideoInput object.
          *
          * @param      const std::string& entityType
          *             The type of this entity.
          */

        VideoInput( const std::string& entityType = VI_DEFAULT_ENTITY_TYPE );

        /**
          * VideoInput
          *
          * Constructs a new VideoInput object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this VideoInput in the database.
          * @param      const std::string& entityType
          *             The type of this entity.
          */

        VideoInput( unsigned long key, const std::string& entityType = VI_DEFAULT_ENTITY_TYPE );
       
        /**
          * ~VideoInput
          *
          * Standard destructor.
          */

        virtual ~VideoInput() {};

        /**
          * getVideoInputGroupName
          *
          * Gets the name of the group that this VideoInput belongs to.
          * This name is used to group the VideoInputs in the GUIs.
          *
          * @return     std::string
          *             The group name.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the VideoInputGroupName parameter value 
          *             cannot be converted to a string.
          */

        std::string getVideoInputGroupName();

        /**
          * setVideoInputGroupName
          *
          * Sets the name of the group that this VideoInput belongs to.
          * This name is used to group the VideoInputs in the GUIs.
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

        void setVideoInputGroupName( const std::string& name );
        
        ///////////////////////////////////////////////////////////////////
        // 
        // Supported Features
        //
        ///////////////////////////////////////////////////////////////////

        /**
          * isSuperimposeDateSupported
          *
          * Returns a bool indicating if the date can be superimposed
          * on this VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the SuperimposeDateSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the SuperimposeDateSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isSuperimposeDateSupported();

        /**
          * setSuperimposeDateSupported
          *
          * Stores if the date can be superimposed on this VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isSuperimposeDateSupported
          *             True if the SuperimposeDateSupported parameter is 1.
          *             False otherwise.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setSuperimposeDateSupported( bool isSuperimposeDateSupported );

        /**
          * isSuperimposeNameSupported
          *
          * Returns a bool indicating if the VideoInput name can be superimposed
          * on this VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the SuperimposeNameSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the SuperimposeNameSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isSuperimposeNameSupported();

        /**
          * setSuperimposeNameSupported
          *
          * Stores if the VideoInput name can be superimposed on this VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isSuperimposeNameSupported
          *             True if the SuperimposeNameSupported parameter is 1.
          *             False otherwise.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setSuperimposeNameSupported( bool isSuperimposeNameSupported );

        /**
          * isSuperimposeTextSupported
          *
          * Returns a bool indicating if free text can be superimposed
          * on this VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the SuperimposeTextSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the SuperimposeTextSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isSuperimposeTextSupported();

        /**
          * setSuperimposeTextSupported
          *
          * Stores if free text can be superimposed on this VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isSuperimposeTextSupported
          *             True if the SuperimposeTextSupported parameter is 1.
          *             False otherwise.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setSuperimposeTextSupported( bool isSuperimposeTextSupported );

        ///////////////////////////////////////////////////////////////////
        // 
        // State Parameters 
        //
        ///////////////////////////////////////////////////////////////////
        
        /**
          * isPublicViewingAllowed
          *
          * Returns a bool indicating if this VideoInput is allowed to be viewed
          * publicly (e.g. on the internet).
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the PublicViewingAllowed parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the PublicViewingAllowed parameter value 
          *             cannot be converted to a bool.
          */

        bool isPublicViewingAllowed();

        /**
          * setPublicViewingAllowed
          *
          * Stores if this VideoInput is allowed to be viewed publicly
          * (e.g. on the internet).
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isPublicViewingAllowed    
          *             True  => The VideoInput is allowed to be viewed publicly.
          *                      Sets the PublicViewingAllowed parameter to 1.
          *             False => Sets the PublicViewingAllowed parameter to 0.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setPublicViewingAllowed( bool isPublicViewingAllowed );

        
        /**
          * isInService
          *
          * Returns a bool indicating if this VideoInput is in service.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the InService parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the InService parameter value 
          *             cannot be converted to a bool.
          */

        bool isInService();

        /**
          * setInService
          *
          * Stores if this VideoInput is in service.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isInService    
          *             True  => The VideoInput is in service.
          *                      Sets the InService parameter to 1.
          *             False => Sets the InService parameter to 0.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setInService( bool isInService );

        /**
          * isDateSuperimposedOnVideoInput
          *
          * Returns a bool indicating if the date is being superimposed on the VideoInput. 
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the isDateSuperimposedOnVideoInput parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the isDateSuperimposedOnVideoInput parameter value 
          *             cannot be converted to a bool.
          */

        bool isDateSuperimposedOnVideoInput();

        /**
          * setDateSuperimposedOnVideoInput
          *
          * Stores if the date is being displayed on the VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isDateSuperimposedOnVideoInput    
          *             True  => The date is superimposed on the VideoInput.
          *                      Sets the DateSuperimposedOnVideoInput parameter to 1.
          *             False => Sets the DateSuperimposedOnVideoInput parameter to 0.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setDateSuperimposedOnVideoInput( bool isDateSuperimposedOnVideoInput );

        /**
          * isNameSuperimposedOnVideoInput
          *
          * Returns a bool indicating if the VideoInput name is being superimposed on the VideoInput. 
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the isNameSuperimposedOnVideoInput parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the isNameSuperimposedOnVideoInput parameter value 
          *             cannot be converted to a bool.
          */

        bool isNameSuperimposedOnVideoInput();

        /**
          * setNameSuperimposedOnVideoInput
          *
          * Stores if the VideoInput name is being displayed on the VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      bool isNameSuperimposedOnVideoInput    
          *             True  => The name is superimposed on the VideoInput.
          *                      Sets the NameSuperimposedOnVideoInput parameter to 1.
          *             False => Sets the NameSuperimposedOnVideoInput parameter to 0.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setNameSuperimposedOnVideoInput( bool isNameSuperimposedOnVideoInput );

        /**
          * getTextSuperimposedOnVideoInput
          *
          * Returns the text that is being superimposed on the VideoInput. 
          * If no text is being superimposed, then "" is returned.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     std::string    
          *             The text being superimposed on the VideoInput or "". 
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          */

        std::string getTextSuperimposedOnVideoInput();

        /**
          * setTextSuperimposedOnVideoInput
          *
          * Stores the text that is being superimposed on the VideoInput.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      std::string superimposedText    
          *             The text to superimpose on the VideoInput 
          *             or "" if the text has been cleared.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setTextSuperimposedOnVideoInput( std::string superimposedText );

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
          * Returns a pointer to a new VideoInput object that has been initialised with the 
          * specified key.
          *
          * @return     IEntityData*
          *             A new instance of this entity.  It is the caller's responsibility
          *             to clean up the object.
          *
          * @param  unsigned long key 
          *         The key with which to construct the VideoInput object.
          */

        //virtual IEntityData* clone( unsigned long key );

    private:
        // 
        // The default entity type is used for VideoInputs that are not
        // derived from this class, but are of this class.
        // 

        static const std::string VI_DEFAULT_ENTITY_TYPE;

        //
        // Entity parameter names.
        //

        static const std::string VIDEO_INPUT_GROUP_NAME;
        
        // 
        // Supported Features
        //

        static const std::string SUPERIMPOSE_DATE_SUPPORTED;
        static const std::string SUPERIMPOSE_NAME_SUPPORTED;
        static const std::string SUPERIMPOSE_TEXT_SUPPORTED;
        // ADD NEW FEATURES HERE.

        // 
        // State Parameters 
        //

        static const std::string PUBLIC_VIEWING_ALLOWED;
        static const std::string IN_SERVICE;
        static const std::string DATE_SUPERIMPOSED;
        static const std::string NAME_SUPERIMPOSED;
        static const std::string SUPERIMPOSED_TEXT;

        //
        // Disable copy constructor and assignment operator
        //

        VideoInput( const VideoInput& theVideoInput );
        VideoInput& operator=( const VideoInput& theVideoInput );

    }; // VideoInput

    
    typedef boost::shared_ptr<VideoInput> VideoInputPtr;
} // TA_Base_Core

#endif // !defined(VideoInput_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
