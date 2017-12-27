#if !defined( RecordingUnit_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define RecordingUnit_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RecordingUnit.h $
  * @author Robert Young 
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * RecordingUnit is a derivation of EntityData. It provides access to RecordingUnit entity data and a specific framework
  * for RecordingUnit entity parameters.
  */

#include "core/data_access_interface/entity_access/src/VideoSwitchResource.h"

namespace TA_Base_Core
{
    class RecordingUnit : public VideoSwitchResource
    {
    public:
        
        //
        // The types of RecordingUnits.  If this is updated, also update
        // app/cctv/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h.
        //

        enum ERecordingUnitType
        {
            VCR = 0,
            DVD = 1
        };

        /**
          * RecordingUnit
          *
          * Constructs a new RecordingUnit object that contains only the static values. This
          * includes type, so the getType() method can be called without any further action.
          */

        RecordingUnit();

        /**
          * RecordingUnit
          *
          * Constructs a new RecordingUnit object with the specified key.
          *
          * @param   unsigned long key 
          *          The key to this RecordingUnit in the database.
          */

        RecordingUnit( unsigned long key );

        /**
          * ~RecordingUnit
          *
          * Standard destructor.
          */

        virtual ~RecordingUnit() {};

        ///////////////////////////////////////////////////////////////////
        // 
        // Supported Features
        //
        ///////////////////////////////////////////////////////////////////

        /**
          * isPlaybackSupported
          *
          * Returns a bool indicating if this RecordingUnit supports Playback.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the PlaybackSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the PlaybackSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isPlaybackSupported();

        /**
          * isRecordingSupported
          *
          * Returns a bool indicating if this RecordingUnit supports Recording.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the RecordingSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the RecordingSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isRecordingSupported();

        /**
          * isRecordingSpeedSupported
          *
          * Returns a bool indicating if this RecordingUnit supports Recording Speed.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     bool    
          *             True if the RecordingSpeedSupported parameter is 1.
          *             False otherwise.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the RecordingSpeedSupported parameter value 
          *             cannot be converted to a bool.
          */

        bool isRecordingSpeedSupported();

        /** 
          * getConsoleKey
          *
          * Returns the key of the console this RecordingUnit is associated with.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     unsigned long
          *             key of the console this RecordingUnit is associated with.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the Console Key parameter value 
          *             cannot be converted to an unsigned long.
          */

        unsigned long getConsoleKey();

        /**
          * getRecordingUnitType
          *
          * Returns the RecordingUnit type.
          *
          * Preconditions:  0 != getHelper()
          *
          * @return     ERecordingUnitType    
          *             The RecordingUnit type.
          * 
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the RecordingUnitType parameter value 
          *             cannot be converted to an integer.
          */

        ERecordingUnitType getRecordingUnitType();

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

		static std::string getStaticType();

        /**
          * clone
          *
          * Returns a pointer to a new RecordingUnit object that has been initialised with the 
          * specified key.
          *
          * @param  unsigned long key 
          *         The key with which to construct the RecordingUnit object.
          */

        static IEntityData* clone(unsigned long key);

    private:

        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        //
        // Console param name
        //

        static const std::string CONSOLE;

        // 
        // Supported Features
        //

        static const std::string PLAYBACK;
        static const std::string RECORDING;
        static const std::string RECORDING_SPEED;
        static const std::string RECORDING_UNIT_TYPE;

        //
        // Disable copy constructor and assignment operator
        //

        RecordingUnit( const RecordingUnit& theRecordingUnit );
        RecordingUnit& operator=( const RecordingUnit& theRecordingUnit );


    }; // RecordingUnit

    
    typedef boost::shared_ptr<RecordingUnit> RecordingUnitPtr;
} // TA_Base_Core

#endif // !defined(RecordingUnit_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
