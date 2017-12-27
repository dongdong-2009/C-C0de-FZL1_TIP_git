/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/DBRetriever.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __DB_RETRIEVER_H_INCLUDED__
#define __DB_RETRIEVER_H_INCLUDED__

#include "app/event/event_agent/src/IDataLoader.h"
#include "app/event/event_agent/src/DatabaseLoader.h"

#include "core/event/IDL/src/EventAgentCorbaDef.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/naming/src/NamedObject.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_App
{
	class DbEventRetriever
		: public TA_Base_Core::Thread
		, public IDataLoader 
		, public TA_Base_Core::RunParamUser
	{
	public: // methods
		enum REFRESH_TYPE
		{
			DATABASE_REFRESH = 0,
			AGENT_REFRESH
		};

		/**
		 * Destructor
		 */
		~DbEventRetriever();
		 
		/**
          * getInstance
          *
          * static function to retrieve the singleton instance
          */
        static DbEventRetriever& getInstance();

		/**
          * releaseInstance
          *
          * static function to release the singleton instance
          */
        static void releaseInstance();

		/**
          * run
          *
          * Overrides the pure virtual run method declared in Thread
          */
        virtual void run();

        /**
          * terminate
          *
          * Overrides the pure virtual terminate method declared in Thread
          */
        virtual void terminate();

		/** 
          * loadFromDatabase
          *
          * Overrides the pure virtual loadFromDatabase method declared in IDataLoader
          */
        virtual void loadFromDatabase();

        /** 
          * requestFromControllingAgent
          *
          * Overrides the pure virtual requestFromControllingAgent method declared in IDataLoader.
          */
        virtual void requestFromControllingAgent();

        /**
          * hasDataLoaded
          *
          * Overrides the pure virtual hasDataLoaded method declared in IDataLoader
          */
        virtual bool hasDataLoaded();

        /**
          * LoadData
          *
          * function to launch a new database loader to retrieve data from database.
          */
        void LoadData();

		/**
          * ResetDataLoader
          *
          * function to will delete existing data loader and reset data loaded flag.
          */
        void resetDataLoader();

		/**
          * startDbRefresh
          *
          * function sets the refresh type to database refresh and start thread to refresh.
          */
		void startDbRefresh();

		/**
          * startAgentSync
          *
          * function sets the refresh type to agent refresh and start thread to refresh.
          */
		void startAgentSync();

		/**
		 * onRunParamChange
		 *
		 * Called whenever a RunParam is updated during run-time.
		 *
		 * @param name The name of the RunParam that has changed.
		 * @param value The new value of the RunParam.
		 */
		void onRunParamChange(const std::string& name, const std::string& value);
		
	private: // methods
		/**
		 * Construction
		 */
		DbEventRetriever();

		// Disable copy constructor and assignment operator
		DbEventRetriever( const DbEventRetriever& theActor);
		DbEventRetriever& operator=(const DbEventRetriever &);

		
		// Database refresh mechanism
		void _dbRefreshProc();

		// agent refresh mechanism
		void _agentRefreshProc();

		// Get the Current database Connection string
		std::string getCurrentDatabaseString();

		// Register class for run param change
		void registerForDbRunParamChange();

		// Check if database change
		inline bool hasDatabaseChanged()
		{
			TA_Base_Core::ThreadGuard guard(m_bThreadLock);
			return m_bDatabaseChanged;
		}

	private: // variables
		typedef TA_Base_Core::NamedObject<TA_Base_Core::EventAgentCorbaDef,
			TA_Base_Core::EventAgentCorbaDef_ptr,
			TA_Base_Core::EventAgentCorbaDef_var>   EventAgentNameObj;
		
		// terminate flag for thread to exit
		bool m_bTerminate;

		// static instance of this class
		static DbEventRetriever* m_pInstance;

		// static lock for singleton creation
		static TA_Base_Core::NonReEntrantThreadLockable m_bSingletonLock;
		
		// Thread lock
		TA_Base_Core::ReEntrantThreadLockable m_bThreadLock;

		// Flag to indicate that data has already been loaded
		bool m_bDataLoaded;

		// Database loader 
		DatabaseLoader* m_pDbLoader;

		// Database Refresh Interval
		unsigned long m_ulDbRefreshInterval;

		// Agent Refresh Interval
		unsigned long m_ulAgentRefreshInterval;

		// location key
		unsigned long m_ulMyLocation;

		// Refresh type
		REFRESH_TYPE m_refreshType;

		// Current database connection string
		std::string m_currentDbString;

		// DbChange
		bool m_bDatabaseChanged;
	};

}

#endif //__DB_RETRIEVER_H_INCLUDED__