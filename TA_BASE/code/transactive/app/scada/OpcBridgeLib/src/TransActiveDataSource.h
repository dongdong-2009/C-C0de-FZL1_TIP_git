/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:     $File$
  * @author:    HoaVu
  * @version:   $Revision$
  *
  * Last modification:  $DateTime$
  * Last modified by:   $Author$
  *
  * Description:
  *
  * Object of the class TransActiveDataSource implements the interface ITransActiveDataSource and
  * performs the following functionalities:
  *
  *     1) Initialise the OPC Server (on the main thread)
  *
  *     2) Monitors callback events from the OPC Server for reading and writing of TransActive
  *        DataPoint's attributes.
  *
  *     3) Monitors callback events from the OPC Server for subscribing and unsubscribing of
  *        TransActive DataPoints's attributes.
  *
  */

#ifndef TRANSACTIVEDATASOURCE_H
#define TRANSACTIVEDATASOURCE_H 

#include <string>

#include "app/scada/OpcBridgeLib/src/Attribute.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/naming/src/INamedObject.h"
#include "core/threads/src/QueueProcessorPool.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"
// Disable identifier truncated in debug warning
#pragma warning ( disable:4786 )

namespace TA_Base_Core
{
    class IOpcTagAlias;
	class SingletonTimerUtil;
}

namespace TA_Base_App
{
    // Forward class declaration
    class OpcBridge;

    enum AttributeUpdateType
    {
        Add,
        UpdateProxy,
        Remove
    };

    class AttributeUpdate
    {
    public:

        AttributeUpdate( unsigned long entityKey,
                         const std::string& tagName,
                         AttributeUpdateType type,
						 EOPCATTRIBUTETYPE attrType,
						 OPCCacheData& opcEntityData,
                         TA_Base_Core::IOpcTagAlias* opcTagAlias = NULL)
        :
        m_entityKey( entityKey ),
        m_tagName( tagName ),
        m_type( type ),
		m_attrType(attrType),
        m_opcTagAlias( opcTagAlias ),
		m_opcEntityData( opcEntityData )
        {
        }

        // Used for queueUnique
        bool operator==(const AttributeUpdate& rhs)
        {
            return ( (m_entityKey == rhs.m_entityKey) &&
                     (m_tagName == rhs.m_tagName) &&
                     (m_type == rhs.m_type));
        }

        // Used for queueReplace
        bool isReplacedBy(const boost::shared_ptr<AttributeUpdate>& update)
        {
            // Replace adds with removes and vice versa
            if (m_entityKey == update->m_entityKey && 
                m_tagName == update->m_tagName &&
                update->m_type != UpdateProxy &&
                m_type != UpdateProxy)
            {
                return true;
            }

            // else replace if identical
            return *update == *this;
        }

        int getPriority()
        {
            switch (m_type)
            {
            case Add:
                return 2;
            case UpdateProxy:
                return 3;
            case Remove:
                return 1;
            }

            return 0;
        }

    public:
        unsigned long					m_entityKey;
        AttributeUpdateType				m_type;
		std::string						m_tagName;
		//the following members are extra info of creating attribute
		EOPCATTRIBUTETYPE               m_attrType;
        TA_Base_Core::IOpcTagAlias*		m_opcTagAlias;
		OPCCacheData  					m_opcEntityData;
    };

    typedef TA_Base_Core::QueueProcessorPool<AttributeUpdate, 
        TA_Base_Core::PriorityQueueProcessorWorker<AttributeUpdate> > AttributeUpdateProcessorPool;

    class TransActiveDataSource : public TA_Base_Core::Thread,
								  public TA_Base_Core::ITimeoutCallback,
                                  public TA_Base_Core::QueueProcessorPoolCallback<AttributeUpdate>
								  
    {

    public:

        /**
          * TransActiveDataSource
          *
          * Constructor
          *
          */
        TransActiveDataSource(OpcBridge* opcBridge);

        /**
          * ~TransActiveDataSource
          *
          * Destructor
          *
          */
        virtual ~TransActiveDataSource();

        /*
        ** Functions required for the Win-tech OPC Server Library
        */

    public:

		void terminateAndWaitThreadPool();
        /**
          * initialiseWtOPCsvr
          *
          * Initialise the OPC Server
          *
          */
        void initialiseWtOPCsvr(const GUID& ClsIdSvr, const std::string svrName, 
                                const std::string svrDesc, const std::string svrExePath, 
                                const unsigned int svrRate, const std::string vendorInfo);

        /**
          * uninitialiseWtOPCsvr
          *
          * Uninitialise the OPC Server
          *
          */
        void uninitialiseWtOPCsvr(const GUID& ClsIdSvr, const std::string svrName);


        /**
          * enableCallback
          *
          * Enable/Disable processing of Callback from WTOPCSVR
          *
          */
        void enableCallback(bool indication) { m_enableCallback = indication; }


        // override Thread functions
        virtual void run();

        virtual void terminate();

		virtual void timerExpired(long timerId, void* userData);
    protected:

        /**
          * getOpcBridge
          *
          * Gets a reference to the OpcBridge Class
          *
          * @return     a reference to OpcBridge
          */
        static OpcBridge& getOpcBridge();

        /**
          * UnknownTagNotificationHandler
          *
          * Process callback event from the OPC Server when an OPC Client is subscribing an OPCTag
          * associated with a DataPoint's attribute.
          *
          */
        static void CALLBACK UnknownTagNotificationHandler ( LPSTR path, LPSTR name );
        

        /**
          * RemoveTagNotificationHandler
          *
          * Process callback event from the OPC Server when an OPC Client is unsubscribing an OPCTag
          * associated with a DataPoint's attribute.
          *
          */
        static void CALLBACK RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name );


        /**
          * EventHandler
          *
          * This function allows the server application to receive event messages from the WtOPCsvr.dll 
          * during normal operation of the OPC Server Interfaces.  
          * Event messages are available as a debugging tool to trace execution of the logic 
          * through the various Interface procedures. 
          */
        static void CALLBACK EventHandler ( LPSTR evetMsg );

        /**
          * addQueueProcessorPoolItem
          *
          * Adds an item to the processor pool for addition or removal
          *
          */
        static void addUpdateProcessorPoolItem(
			unsigned long entityKey, std::string tagName, AttributeUpdateType type, 
			OPCCacheData& opcData, EOPCATTRIBUTETYPE attrType = E_OPC_ATTR_UNKNOWN, 
			TA_Base_Core::IOpcTagAlias* opcTagAlias = NULL);

        virtual void queueProcessorPoolCallback(boost::shared_ptr<AttributeUpdate> update);

		/**
          * registerOPCsvr
          *
          * update Windows Registry with an instance of the TransActiveOPCBridge
          * to allow other applications to find OPC Server.
          *
		  */	
		bool registerOPCsvr(const GUID& ClsIdSvr, const std::string svrName, 
                            const std::string svrDesc, const std::string svrExePath);

    private:

        // Private Copy Contructor
        TransActiveDataSource ( const TransActiveDataSource & obj );
        
        // Private Assignment Constructor
        TransActiveDataSource & operator= ( const TransActiveDataSource & rhs );

        // Private Constructor
        TransActiveDataSource();

    private:
        bool                                            m_initialised;
		bool											m_bRegistered;
        volatile bool									m_isTerminating;
        bool                                            m_updateTagPeriodically;
        unsigned long									m_updateTagInterval;
		unsigned long									m_registerExceedTime;
        static OpcBridge*                               m_opcBridge;
        static AttributeUpdateProcessorPool*            m_updateProcessorPool;
        static bool                                     m_enableCallback;
        static int                                      m_numWorkers;

		TA_Base_Core::Semaphore							m_timerSemphore;
		TA_Base_Core::SingletonTimerUtil&		        m_timerUtility;
    };
}

#endif
