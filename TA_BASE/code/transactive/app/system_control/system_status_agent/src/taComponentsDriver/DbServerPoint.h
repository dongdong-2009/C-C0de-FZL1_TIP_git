/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_status_agent/src/taComponentsDriver/DbServerPoint.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class provides a datapoint representing a Data Base Server
  * The boolean datapoint is high if the Data Base Server is contactable
  * and responds to polling - else low
  * Note that this implimenation does not raise alarms on low. It is
  * assumed that the datapoint will be configured so that alarms are 
  * raised on low datapoint value
  *
  * Note that this class connects to the database with connection parameters
  * that are specific to a given location rather than using the connection 
  * string passed in by the framework. That is because the one passed in 
  * by the framework may well point to the OCC server (redundancy), rather than the server
  * we are tying to resolve - see resolve() for more info.
  * The schema and password are retrieved from the framework supplied connection string however
  *
  * DB_SERVER_TIMEOUT_PARAM: C830 did not require that the timeout functionality be implemented for the
  * data base server. And after a browse through the SimpleDb implementation and the OCI Access API
  * I did not find any readily apparent mechanism to set the timeout programmatically. 
  * Hence, in the current implementation, setting the time-out param (see  onlineConfigUpdate() )
  * has no effect on the functionality.
  * I have left the code to update the Db Server timeout parameter in place in the hope that someday, 
  * someone smarter than myself will implement the functionality.

  */

#if !defined(AFX_DB_SERVER_POINT_H__79A8E911_C6D4_INCLUDED_)
#define AFX_DB_SERVER_POINT_H__79A8E911_C6D4_INCLUDED_

#include "core/otl/src/otlconf.h"
#include "app/system_control/system_status_agent/src/taComponentsDriver/AbstractPingDependentPoint.h"

#ifdef USING_OTL
class otl_connect;
class otl_stream;
#else
namespace ocl
{
    class OraConnection;
    class OraQuery;
}
#endif


namespace TA_Base_Core
{
	class SimpleDb;
}

namespace sql
{
	class Driver;
	class Connection; 
	class ResultSet;
}

namespace TA_Base_App
	{
	class DbServerPoint // : public AbstractPingDependentPoint //limin-- CL-21689
                        : public ISelfResolvingPointContainer  //limin++ CL-21689
	{
	public:

		/**
		 * DbServerPoint	
		 * 
		 * DTOR
		 *
		 */
		virtual ~DbServerPoint();

		/**
		 * getDataPoint	
		 * 
		 * @return the datapoint owned by this point
		 *
		 */
		virtual TA_Base_Bus::DataPoint* const getDataPoint();

		/*
		* getAddressIdentifier
		*
		* @return the raw IP address or hostname of the point.
		* Note that the address configured for the datapoint will probably contain more than
		* the above, e.g. mftStatus:192.168.70.60. This metod should just return 192.168.70.60 bit
		*/
		virtual std::string getAddressIdentifier();

		/**
		 * onInit
		 * 
		 * This is called for the first and only time when the mode changes
		 * from INIT to something else (either monitor or control) 
		 * on the driver that owns this point. Hence, you should do any
		 * value/state/alarm manipulation required here. Note that when the
		 * agent starts the point will already have a quality of:
		 * TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED
		 *
		 * @param pEntityData contains the updated configuration data
		 *
		 * This implimentation attempts to resolve and store a pointer
		 * to the System Controller. It does not throw is this can not be 
		 * accomplished
		 * It also sets the corba time out and port number via run params.
		 */
		virtual void onInit(TA_Base_Core::SystemStatusAgentEntityDataPtr pEntityData);

		/**
		 * 
		 * onlineConfigUpdate
		 *
		 * This method is called when onlineConfigUpdate is called on the TaComponentsDriver. 
		 * You should process any parameters specific to your point.
		 *
		 * This method will be called when the agent configuration is changed
		 * online. Note that online datapoint configuration updates are
		 * not passed to driver.
		 *
		 * This method updates the timeout parameter
		 *
		 * @param changedParams contains the names of the params that have changed
		 *
		 * See DB_SERVER_TIMEOUT_PARAM comments at top of class
		 */
		virtual void onlineConfigUpdate(std::vector<std::string> changedParams);

	   /**
		* convertToIPointContainer
		*
		* If this ISelfResolvingPointContainer can keep this dataPoint updated it should return
		* a ISelfResolvingPointContainer* which contains this point. If the ISelfResolvingPointContainer
		* does not want this point return 0
		* The IPointContainer* should be created on the heap. You do NOT need to
		* delete this point on destruction
		*/
		static ISelfResolvingPointContainer* convertToIPointContainer(TA_Base_Bus::DataPoint* const dataPoint);


		/**
		  *
		  * updateDataPoint
		  *
		  * This method first calls updateDataPointBasedOnPing()
		  * to see if it should bother trying to update the point. If the ping is OK it then
		  * employs derived class methods to update the data point
		  *
		  * This method also ensures that isUpdating() returns true
		  * when this method is running - not thread-safe.
		  */
		virtual void updateDataPoint();

		 /**
		* isUpdating
		*
		* returns false if the point is currently processing a call
		* to updateDataPoint().
		* THIS METHOD IS NOT THREAD-SAFE! Hence it is assumed that a
		* single client thread is employing this method
		*/
		virtual bool isUpdating();

	protected:
	   /**
		* updatePointBasedOnProcessStatus
		*
		* update the state of the point based on the status of the related object. 
		* You should set an appropriate state if even you can not read the status of the
		* related process (but return false). Ignore the posibliltiy of Lan failure, e.g. if you could
		* not reach a Data Base Server process set low, GOOD quality
		*
		*@precondition: isResolved() returns true
		* 
		*@return false if you can not resolve the process/object required to read
		* the status. If you update the point successfully return true.
		*
		* @throw if you can not write to the datapoint
		*
		*/
		virtual bool updatePointBasedOnProcessStatus();

	   /*
		* isResolved
		*
		*@return true if you feel that resolve() should not need not be called
		* prior to calling updateBasedOnProcessStatus(). Note that you can return true
		* even if it is possible (but unlikely) that the resolved object is no longer
		* valid
		* 
		*/
		virtual bool isResolved();

	   /**
		* setStateForNoProcessResolution
		*
		* update the state of the data point to an appropriate state given that you can not read the status of the
		* related process.
		*
		* @throw if you can not write to the datapoint
		* 
		*/
		virtual void setStateForNoProcessResolution();

	   /**
		* resolve
		*
		* resolve or re-resove (and store) the process or object that is used to 
		* determine the state that the datapoint should be set to
		*
		*@return true if the resolution is OK - else false
		* 
		* This implimentation creates and opens a database instance
		*
		* Note that this method connects to the database with a connection paramaters
		* that are specific to a given location rather than using the connection 
		* string passed in by the framework. That is because the one passed in 
		* by the framework may well point to the OCC server, rather than the server
		* we are tying to resolve - see resolve() for more info.
		*
		* The database connection string used has the form:
		* : <m_hostName>:<m_schemaName>:<m_dbPassword>
		* where m_hostName is supplied by datapoint config,
		* and m_schemaName and m_dbPassword are (hidden) run-time params.
		*
		* Note that the tnsnames.ora file must be configured so that 
		* the alias in that file is identical to the hostname for the
		* machine that the server instance is running on - IP addresses
		* can not be used. This convention is used so that the address config
		* can be used to both identfy the box that the db server is running on
		* and the alias in the tnsnames.ora file
		*/
		virtual bool resolve();	
		
	private:
		std::string m_strOTLConnection;

#ifdef USING_OTL
		otl_connect*	m_pOraConnection;
		otl_stream*     m_pOraQuery;
#else
        ocl::OraConnection* m_pOraConnection;
        ocl::OraQuery*      m_pOraQuery;
#endif

		sql::Driver*        m_pMysqlDriver;
		sql::Connection*    m_pMysqlConntion;
		sql::ResultSet*     m_pMysqlQuery;
        //++Bohong

		TA_Base_Bus::BooleanDataPoint* const m_datapoint;
	   
	   /**		
		* The database connection string used has the form:
		* : <m_hostName>:<m_schemaName>:<m_dbPassword>
		* where m_hostName is supplied by datapoint config,
		* and m_schemaName and m_dbPassword are (hidden) run-time params.
		*
		* Note that the tnsnames.ora file must be configured so that 
		* the alias in that file is identical to the hostname for the
		* machine that the server instance is running on - IP addresses
		* can not be used. This convention is used so that the address config
		* can be used to both identfy the box that the db server is running on
		* and the alias in the tnsnames.ora file
		*/
		std::string m_strHostName;   // for MySQL DB server or IP address
		std::string m_strServerName; // for Oracle TNS server name
        std::string m_strBaseServerName; // for fetching username and password
		std::string m_strUserName;
		std::string m_strPassword;
		std::string m_strShema;     // for Mysql DB server name

		TA_Base_Core::SystemStatusAgentEntityDataPtr m_entityData;

		//Database timeout param set via run param
		int m_dbTimeOutSec;
		int m_nDBType;
		bool m_isUpdating;

		static std::vector<std::string> m_dbStatusNames;
			


	private:
		//private methods:

        /** 
          * DbServerPoint
          *
          * @param DataPoint* const datapoint is the point
		  * to update. Precondition the point is a registered CORBA object  
		  *
		  * @param const std::string hostName is the hostname where the CS is running
          */
		DbServerPoint( 
			TA_Base_Bus::BooleanDataPoint* const datapoint,
			const std::string& hostName
		);


        /** 
          * poll
          *
		  * This call will poll the db server - to determine if it is happy
		  *
          * @throw if the db Server does not communicate, i.e. not happy!
          */
		void poll();

        /** 
          * getConnectionParameters
          *
		  * retrieves the database user name and password from the connection
		  * string supplied by the framework
          */
		void getConnectionParameters( std::string& username, std::string& password );

        //Bohong++
        bool _Connect();
        void _DetachDb();

		// oyzl++
		void _Init();
		void _OraConnection();
		void _MysqlConnect();
		void _DoOraQuery();
		void _DoMysqlQuery();
		void _ParseDBType(const std::string& strDBName);

        bool m_reConnect;
        //++Bohong
	};
} //end namespace TA_Base_App

#endif // !defined(AFX_DB_SERVER_POINT_H__79A8E911_C6D4_INCLUDED_)
