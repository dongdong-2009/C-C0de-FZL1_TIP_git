#if !defined(ECSMANAGER_H__C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
#define ECSMANAGER_H__C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ECSManager.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * ECSManager holds the data specific to a ECSManager entity from the
  * database and allows read-only access to that data. 
  *
  * IMPORTANT: To ensure this object remains thread-safe, any added accessors must be
  * ThreadGuarded as they potentially modify state of object (i.e. if !validData).
  *
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Core
{
    class ECSManager : public EntityData
    {
    public:

        /**
         * ECSManager (constructor)
         *
         * Constructs a new ECSManager object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        ECSManager() {};


        /**
         * ECSManager (constructor)
         *
         * Constructs a new ECSManager object with the specified key.
         *
         * @param key The key to this ECSManager in the database.
         */
        ECSManager(unsigned long key);


        /**
         * Destructor
         */
        virtual ~ECSManager();

   
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
		static std::string getStaticType();

  
		/**
         * clone
         *
         * Returns a pointer to a new ECSManager object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the ECSManager object.
         */
        static IEntityData* clone(unsigned long key);
        
     
		/**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

	
		/** 
		  * getStationModeStatusTemplate
		  *
		  * description
		  *
		  *
		  * @return 
		  */
		std::string getStationModeStatusToken();


		/** 
		  * getStationModeInProgressTemplate
		  *
		  * description
		  *
		  *
		  * @return 
		  */
		std::string getStationModeInProgressToken();

		//devender++ WP-4677M51520006 09-Sep-2005
        ///** 
        //  * getStationModeControlTemplate
        //  *
        //  * description
        //  *
        //  *
        //  * @return 
        //  */
		//std::string getStationModeControlToken();
        //++devender WP-4677M51520006 09-Sep-2005

        /** 
          * getStationModeControlTemplate
          *
          * description
          *
          *
          * @return 
          */
		std::string getStationVisualLockToken();

        //devender++ WP-4677M51520006 09-Sep-2005 
		///** 
		//  * std::string getProposedStationModeToken
		//  *
		//  * description
		//  *
		//  *
		//  * @return 
		//  */
		//std::string getProposedStationModeToken();


        ///** 
        //  * getOverridingProposedStationMode1Token
        //  *
        // * description
        //  *
        //  *
        //  * @return 
        //  */
		//std::string getOverridingProposedStationMode1Token(); 
 

        ///** 
        //  * getOverridingProposedStationMode2Token
        //  *
        //  * description
        //  *
        //  *
        //  * @return 
        //  */
		//std::string getOverridingProposedStationMode2Token(); 
        //++devender WP-4677M51520006 09-Sep-2005


        /** 
          * getCentralControlLocationKey
          *
          * description
          *
          *
          * @return 
          */
		unsigned long getCentralControlLocationKey();


        /** 
          * getDataPointHealthPollInterval
          *
          * description
          *
          *
          * @return 
          */
        int getDataPointHealthPollInterval();


        //devender++ WP-4677M51520006 09-Sep-2005 
        ///** 
        //  * getProposedMasterMode1Token
        //  *
        //  * description
        //  *
        //  *
        //  * @return 
        //  */
		//std::string getProposedMasterMode1Token(); 


        ///** 
        //  * getProposedMasterMode2Token
        //  *
        //  * description
        //  *
        //  *
        //  * @return 
        //  */
		//std::string getProposedMasterMode2Token(); 


        ///** 
        //  * getcurrentMasterModeToken
        //  *
        //  * description
        //  *
        //  *
        //  * @return 
        //  */
		//std::string getCurrentMasterModeToken(); 
        //++devender WP-4677M51520006 09-Sep-2005

		std::string getAutoModeOnToken();

        //devender++ WP-4677M51520006 09-Sep-2005
        std::string getCurrentTimeTableToken();
        std::string getSelectTimeTableToken();
        //++devender WP-4677M51520006 09-Sep-2005

    private:
        // Private as they are not used.
        ECSManager( const ECSManager& theECSManager);            
		ECSManager& operator=(const ECSManager &);

//	private:
	public:

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        //devender++ WP-4677M51520006 09-Sep-2005
        //std::string m_stationModeControlToken;
		//std::string m_proposedStationModeToken;
        //++devender WP-4677M51520006 09-Sep-2005
        //devender++ WP-4677M51520006 09-Sep-2005
		//std::string m_overridingProposedStationMode1Token;
		//std::string m_overridingProposedStationMode2Token;
		//std::string m_proposedMasterMode1Token; 
		//std::string m_proposedMasterMode2Token; 
		//std::string m_currentMasterModeToken; 
        //++devender WP-4677M51520006 09-Sep-2005
		ReEntrantThreadLockable m_threadLock;

		//
		// constants
		//
		
		// Parameter Name
		static const std::string STATION_MODE_STATUS_TOKEN_PARAMETER_NAME;
		static const std::string STATION_MODE_IN_PROGRESS_TOKEN_PARAMETER_NAME;
        //devender++ WP-4677M51520006  09-Sep-2005
		//static const std::string STATION_MODE_CONTROL_TOKEN_PARAMETER_NAME;
		//static const std::string PROPOSED_STATION_MODE_TOKEN_PARAMETER_NAME;
		//static const std::string OVERRIDING_PROPOSED_STATION_MODE_1_TOKEN_PARAMETER_NAME;
		//static const std::string OVERRIDING_PROPOSED_STATION_MODE_2_TOKEN_PARAMETER_NAME;
        //++devender WP-4677M51520006 09-Sep-2005
		static const std::string CENTRAL_CONTROL_LOCATION_PARAMETER_NAME;
        //devender++ WP-4677M51520006  09-Sep-2005
		//static const std::string PROPOSED_MASTER_MODE_1_TOKEN_PARAMETER_NAME;
		//static const std::string PROPOSED_MASTER_MODE_2_TOKEN_PARAMETER_NAME;
		//static const std::string CURRENT_MASTER_MODE_TOKEN_PARAMETER_NAME;
        //++devender WP-4677M51520006  09-Sep-2005
		static const std::string DATAPOINT_HEALTH_POLL_INTERVAL_PARAMETER_NAME;
		static const std::string STATION_VISUAL_LOCK_PARAMETER_NAME;
		static const std::string AUTOMODE_ON_TOKEN_PARAMETER_NAME;

        static const std::string CURRENT_TIMETABLE_PARAMETER_NAME;
        static const std::string SELECT_TIMETABLE_PARAMETER_NAME;
    };
    
    typedef boost::shared_ptr<ECSManager> ECSManagerPtr;
} // namespace TA_Base_Core

#endif // !defined(ECSMANAGER_H__C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
