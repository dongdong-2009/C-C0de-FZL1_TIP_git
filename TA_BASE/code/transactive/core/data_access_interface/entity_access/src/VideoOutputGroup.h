#if !defined( VideoOutputGroup_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define VideoOutputGroup_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoOutputGroup.h $
  * @author Katherine Thomson 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * VideoOutputGroup provides access to VideoOutputGroup entity data and a specific framework
  * for VideoOutputGroup entity parameters.
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class VideoOutputGroup : public EntityData
    {
    public:
        /**
          * VideoOutputGroup
          *
          * Constructs a new VideoOutputGroup object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        VideoOutputGroup();

        /**
          * VideoOutputGroup
          *
          * Constructs a new VideoOutputGroup object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this VideoOutputGroup in the database.
          */

        VideoOutputGroup( unsigned long key );

        /**
          * ~VideoOutputGroup
          *
          * Standard destructor.
          */

        virtual ~VideoOutputGroup() {};

        /**
          * getGroupType
          *
          * The VideoOutputGroup can only contain VideoOutputs of a single type,
          * due to the complexity of displaying different sized outputs in a group.
          * This method gets the type of VideoOutputs that can be included in this group.
          *
          * @return     std::string
          *             The type of VideoOutput that can be included in this group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the GroupType parameter cannot be converted to a string.
          */

        std::string getGroupType();

        /**
          * setGroupType
          *
          * The VideoOutputGroup can only contain VideoOutputs of a single type,
          * due to the complexity of displaying different sized outputs in a group.
          * This method sets the type of VideoOutputs that can be included in this group.
          *
          * @param      const std::string& groupType
          *             The type of VideoOutput that can be included in this group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setGroupType( const std::string& groupType );

        /**
          * getTypeSize
          *
          * The VideoOutputGroup can only contain VideoOutputs of a single type and size,
          * due to the complexity of displaying different sized outputs in a group.
          * This method gets the size of VideoOutputs that can be included in this group.
          * e.g. Small, Medium or Large for VideoMonitors.  For types that don't have
          * different sizes, this method will return Small.
          *
          * @return     std::string
          *             The type of VideoOutput that can be included in this group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the TypeSize parameter cannot be converted to a string.
          */

        std::string getTypeSize();

        /**
          * setTypeSize
          *
          * The VideoOutputGroup can only contain VideoOutputs of a single type,
          * due to the complexity of displaying different sized outputs in a group.
          * This method sets the size of VideoOutputs that can be included in this group.
          * e.g. Small, Medium or Large for VideoMonitors.  For types that don't have
          * different sizes, this method will return Small.
          *
          * @param      const std::string& typeSize
          *             The type of VideoOutput that can be included in this group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setTypeSize( const std::string& typeSize );

        /**
          * getPosition
          *
          * Gets the position of the VideoOutputGroup in the VDM overview. 
          * The left most section postion is 1, incrementing by 1 to the right.
          *
          * @return     unsigned short
          *             The position ( 1,2, ... n ).
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the Position parameter value 
          *             cannot be converted to an unsigned short greater than 1.
          */

        unsigned short getPosition();

        /**
          * setPosition
          *
          * Sets the position of the VideoOutputGroup in the VDM overview. 
          * The left most section postion is 1, incrementing by 1 to the right.
          *
          * @param      unsigned short position
          *             The position ( 1,2, ... n ).
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setPosition( unsigned short position );

        /**
          * getGroupDimensionX
          *
          * The VideoOutputGroup is displayed as a 2D matrix of VideoOutputs.
          * This method gets the width of the matrix.
          *
          * @return     unsigned short
          *             The number of VideoOutputs across the group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the GroupDimensionX parameter value 
          *             cannot be converted to an unsigned short.
          */

        unsigned short getGroupDimensionX();

        /**
          * setGroupDimensionX
          *
          * The VideoOutputGroup is displayed as a 2D matrix of VideoOutputs.
          * This method sets the width of the matrix.
          *
          * @param      unsigned short groupDimensionX
          *             The number of VideoOutputs across the group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setGroupDimensionX( unsigned short groupDimensionX );

        /**
          * getGroupDimensionY
          *
          * The VideoOutputGroup is displayed as a 2D matrix of VideoOutputs.
          * This method gets the height of the matrix.
          *
          * @return     unsigned short
          *             The number of VideoOutputs down the group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the GroupDimensionY parameter value 
          *             cannot be converted to an unsigned short.
          */

        unsigned short getGroupDimensionY();

        /**
          * setGroupDimensionY
          *
          * The VideoOutputGroup is displayed as a 2D matrix of VideoOutputs.
          * This method sets the height of the matrix.
          *
          * @param      unsigned short groupDimensionY
          *             The number of VideoOutputs down the group.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setGroupDimensionY( unsigned short groupDimensionY );

        /**
          * isCapableOfLayouts
          *
          * Determines if the VideoOutputGroup can display layouts.
          *
          * @return     bool
          *             True if layouts are supported.
          *             False otherwise.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the IsCapableOfLayouts parameter value 
          *             cannot be converted to a bool.
          */

        bool isCapableOfLayouts();

        /**
          * setCapableOfLayouts
          *
          * Sets if the VideoOutputGroup can display layouts.
          *
          * @param      bool isCapableOfLayouts
          *             True if layouts are supported.
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

        void setCapableOfLayouts( bool isCapableOfLayouts );

        /**
          * getVideoWallboardAgentName
          *
          * Get the name of the agent to contact to operate the wallboard for this group
          *
          * @return     std::string
          *             The name of the VideoWallLayoutAgent
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the VideoWallboardAgentName parameter cannot be converted to a string.
          */

        std::string getVideoWallboardAgentName();

        /**
          * setGroupType
          *
          * Set the name of the agent to contact to operate the wallboard for this group
          *
          * @param      const std::string& wallboardName
          *             The name of the VideoWallLayoutAgent.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if:
          *             1. The parameter name cannot be found
          *             2. There is more than one value for a parmeter
          *             3. The entity key is invalid ( and this is not a new entity ).
          */

        void setVideoWallboardAgentName( const std::string& wallboardName );

        
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
         
        //
        // Defines how true and false are defined for entity parameter values.
        //

        static const std::string PARAMETER_TRUE;
        static const std::string PARAMETER_FALSE;

    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        //
        // The entity parameter names.
        // 

        static const std::string GROUP_TYPE;
        static const std::string TYPE_SIZE;
        static const std::string POSITION;
        static const std::string GROUP_DIMENSION_X;
        static const std::string GROUP_DIMENSION_Y;
        static const std::string CAPABLE_OF_LAYOUTS;
        static const std::string WALLBOARD_AGENT_NAME;
       
        //
        // Valid sizes.
        //

        static const std::string SMALL;
        static const std::string MEDIUM;
        static const std::string LARGE;

        //
        // Disable copy constructor and assignment operator
        //

        VideoOutputGroup( const VideoOutputGroup& theVideoOutputGroup );
        VideoOutputGroup& operator=( const VideoOutputGroup& theVideoOutputGroup );

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
          * Sets the value for the named unsigned short parameter.
          *
          * Preconditions:  0 != getHelper()
          *
          * @param      const std::string& name
          *             The name of the parameter to set.
          * @param      unsigned short
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

        void setParameter( const std::string& name, unsigned short value);
  
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

    }; // VideoOutputGroup

    
    typedef boost::shared_ptr<VideoOutputGroup> VideoOutputGroupPtr;
} // TA_Base_Core

#endif // !defined(VideoOutputGroup_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
