#if !defined( VideoSwitchAgent_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_ )
#define VideoSwitchAgent_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/VideoSwitchAgent.h $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/05/14 23:14:46 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * VideoSwitchAgent provides access to VideoSwitchAgent entity data and a specific framework
  * for VideoSwitchAgent entity parameters.
  *
  */

#include <string>

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class VideoSwitchAgent : public EntityData
    {
    public:

        /**
          * VideoSwitchAgent
          *
          * Constructs a new VideoSwitchAgent object that contains the static values. This
          * includes type, so the getType() method can be called without any further action.
          */
        VideoSwitchAgent();

        /**
          * VideoSwitchAgent
          *
          * Constructs a new VideoSwitchAgent object with the specified key.
          *
          * @param      unsigned long key 
          *             The key to this VideoSwitchAgent in the database.
          */
        VideoSwitchAgent( unsigned long key );

        /**
          * ~VideoSwitchAgent
          *
          * Standard destructor.
          */
        virtual ~VideoSwitchAgent() {};


        /**
          * getSwitchConnectTimeout
          *
          * The maximum response time (milliseconds) before a non-blocking TcpSocket connect attempt
          * to VideoSwitch hardware is deemed to have failed.
          *
          * @return     unsigned long
          *             The maximum response time (milliseconds) before a non-blocking TcpSocket connect attempt
          *             to VideoSwitch hardware is deemed to have failed.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the SwitchConnectTimeout parameter cannot be converted to an unsigned long.
          */
        unsigned long getSwitchConnectTimeout();


        /**
          * getMaxSwitchResponseTime
          *
          * The maximum response time (milliseconds) before a request is deemed to have failed.
          *
          * @return     unsigned long
          *             The maximum response time (milliseconds) before a request is deemed to have failed.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the MaxSwitchResponseTime parameter cannot be converted to an unsigned long.
          */
        unsigned long getMaxSwitchResponseTime();


        /**
          * getSwitchPollInterval
          *
          * The period of time (milliseconds) inbetween poll commands sent to the video switch.
          *
          * @return     unsigned long
          *             The period of time (milliseconds) inbetween poll commands sent to the video switch.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the ActiveSwitchPollInterval parameter cannot be converted to an unsigned long.
          */
        unsigned long getSwitchPollInterval();


        /**
          * getLowPriorityStateValidationCycle
          *
          * The period of time (milliseconds) between low priority equipment state validation cycles.
          *
          * @return     unsigned long
          *             The period of time (milliseconds) between low priority equipment state validation cycles.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the LowPriorityStateValidationCycle parameter cannot be converted to an unsigned long.
          */
        unsigned long getLowPriorityStateValidationCycle();


        /**
          * getHighPriorityStateValidationCycle
          *
          * The period of time (milliseconds) between high priority equipment state validation cycles.
          *
          * @return     unsigned long
          *             The period of time (milliseconds) between high priority equipment state validation cycles.
          *
          * @exception  DatabaseException 
          *             Thrown if there is a database error.
          * @exception  DataException 
          *             Thrown if the HighPriorityStateValidationCyclep parameter cannot be converted to an unsigned long.
          */
        unsigned long getHighPriorityStateValidationCycle();

		std::string getAssetName();


		// get IMOS related parameter value, may return empty string
		std::string getCCTVServerIPAddress();
		unsigned long getCCTVServerPort();
		std::string getCCTVServerUserName();
		std::string getCCTVServerPassword();

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntityData Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getStaticType
          *
          * Gets the type value for this class.
          *
          * @return     std::string
          *             "VideoSwitchAgent"
          */

        static std::string getStaticType();

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

        static IEntityData* clone( unsigned long key );
                
        /**
          * invalidate
          *
          * Marks the data contained by this entity as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          * Also marks the VideoSwitchAgent data as invalid, so it will be reloaded in
          * a similar manner.
          */
        virtual void invalidate();

    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;

        //
        // The entity parameter names.
        // 

        static const std::string SWITCH_CONNECT_TIMEOUT;
        static const std::string MAX_SWITCH_RESPONSE_TIME;
        static const std::string SWITCH_POLL_INTERVAL;
        static const std::string LOW_PRIORITY_STATE_VALIDATION_CYCLE;
        static const std::string HIGH_PRIORITY_STATE_VALIDATION_CYCLE;
		static const std::string ASSET_NAME;
		static const std::string CCTV_SERVER_IP_ADDRESS;
		static const std::string CCTV_SERVER_PORT;
		static const std::string CCTV_SERVER_USER_NAME;
		static const std::string CCTV_SERVER_PASSWORD;

        //
        // Disable copy constructor and assignment operator
        //

        VideoSwitchAgent( const VideoSwitchAgent& theVideoSwitchAgent );
        VideoSwitchAgent& operator=( const VideoSwitchAgent& theVideoSwitchAgent );

    }; // VideoSwitchAgent

    
    typedef boost::shared_ptr<VideoSwitchAgent> VideoSwitchAgentPtr;
} // TA_Base_Core

#endif // !defined(VideoSwitchAgent_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
