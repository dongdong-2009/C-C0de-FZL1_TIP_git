/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/ItaScada/src/CtaScada.h $
  * @author  Sean Liew
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  * Implementation of the Class CtaScada
  *
  */

#if !defined(CtaScada_032038ED_551E_45bf_91E2_A254B87D7BAF__INCLUDED_)
#define CtaScada_032038ED_551E_45bf_91E2_A254B87D7BAF__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "bus/scada/ItaScada/public_interfaces/ItaScada.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"


namespace TA_Base_Core
{
	class ConfigUpdateDetails;
}

namespace TA_Base_Bus
{
    class IEntity;
	class DataPointFactory;
	class DataPoint;

	/**
	* Implementation of ItaScada:
	*
	* Notes - two methods of access:
	*
	* 1 - External datapoints
	* Datapoints localised via DP entity name.  When a datapoint is
	* requested, this class will look it up in a map of past requests.  If it is not
	* found, then a new DataPointProxy is created.  The entity name must be used to
	* derive the Corba name.  This requires database access, which is why we must
	* cache the requests in a map.  The cache should map DP entity name to
	* DataPointProxy.
	*
	* 2 - Internal datapoints
	* Internal datapoints can be localised by either DP address or entity name. Requires
	* two maps, 1 from address to DataPoint* and 2 from entity name to DataPoint*. Note
	* that it may be possible to configure address non-uniquely.  If a set request is 
	* made on two local datapoints with the same address, they should both be updated.
	* Get operations will only return data from the first found.
	* 
	* Note in both situations, if the datapoint cannot be found, the methods should throw 
	* exceptions for the client code to process.
	* @created 04-Mar-2005 10:30:49 AM
	* @version 1.0
	* @updated 12-Apr-2005 14:23:17
	*/
    class CtaScada : public virtual ItaScada,
					 public TA_Base_Bus::IEntityUpdateEventProcessor,
                     public TA_Base_Bus::IDataPointAgent
	{

    public:

	    CtaScada();
	    virtual ~CtaScada();

		//
		// Operations required by interface ItaScada
		//

	    /**
	     * Write to a datapoint hosted by an external party using a session Id for rights
	     * checking.  If the session Id is NULL, then rights-checking is avoided.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param name    The entity name of the datapoint.
	     * @param value    The datapoint value to be written.  Ensure that the value is
	     * consistent with the datapoint configuration.
	     * @param sessionId    The operator sessionId for performing rights-checking.
	     *
	     */
		virtual void setExternalDataPointValue(const std::string& name, const std::string& value, const std::string& sessionId = "");

	    /**
	     * Obtain the value from a datapoint hosted by an external party.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     *
	     * @param name
	     * @param returnedValue
	     * @param returnedQuality
	     *
	     */
		virtual void getExternalDataPointValue(const std::string& name, std::string& returnedValue, TA_Base_Core::EDataPointQuality& returnedQuality);
        
        /**
        * Write to a local datapoint owned by this application.  The datapoint is
        * localised by the datapoint entity name.
        *
        * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
        *
        * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
        *
        * Throws CtaScadaWriteException if the write operation fails.
        * @param name
        * @param value
        * @param quality
        * @param timestamp
        *
        */
        virtual void setLocalDataPoint(TA_Base_Bus::DataPoint& dataPoint, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setLocalDataPoint(TA_Base_Bus::DataPoint& dataPoint, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setLocalDataPoint(TA_Base_Bus::DataPoint& dataPoint, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setLocalDataPoint(TA_Base_Bus::DataPoint& dataPoint, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void updateFieldState (DataPoint& dataPoint, DpValue& dpValue, time_t time, TA_Base_Core::EDataPointQuality quality);
        
        
        //Mintao++
        //TD13906
        /**
	      * getLocalDataPointByUniqueAddress
	      * 
	      * Read a local datapoint owned by this application.  The datapoint is
	      * localised by the datapoint address field. The caller must be sure the 
	      * address is unique in the local data points
          *
	      * @return void 
	      * @param : const std::string& address
	      * @param : const std::string& returnedValue
	      * @param : const std::string& returnedQuality
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void getLocalDataPointStateByUniqueAddress(const std::string& address, std::string& returnedValue,TA_Base_Core::EDataPointQuality& returnedQuality);
        

	    /**
	      * * > getAllLocalDataPoints
	      * 
	      * return all the local datapoints maintained by the ctascada, it is caller's responsibility to delete them
	      * 
	      * @return std::vector <TA_Base_Bus::DataPoint 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::map < unsigned long, TA_Base_Bus::DataPoint * > getAllLocalDataPoints();

        //Mintao++
        //TD13910

	    /**
	     * Update the quality flag of all local datapoints owned by this application.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param quality
	     *
	     */
		virtual void setAllLocalDataPointQualities(TA_Base_Core::EDataPointQuality quality);

	    /**
	     * Update the state of all local datapoints whose address contains the
	     * specified sub-string.
	     *
	     * Example - configured local datapoint addresses are:
	     * "R1 Summary", "R2 Summary", "R3 Summary", "R4 Summary"
	     * "R1 Connection", "R2 Connection", "R3 Connection", "R4 Connection"
	     * Calling setAllMatchingLocalDataPointQualities("Connection", ITA_SCADA_BAD) will
	     * set "R1 Connection", "R2 Connection", "R3 Connection", "R4 Connection" to BAD.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param addressSubString    Substring to match local datapoint addresses with
	     * @param value
	     * @param quality
		 * @param time
	     *
	     */
        virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        
		/**
	     * Update the quality flag of all local datapoints whose address contains the
	     * specified sub-string.
	     *
	     * Example - configured local datapoint addresses are:
	     * "R1 Summary", "R2 Summary", "R3 Summary", "R4 Summary"
	     * "R1 Connection", "R2 Connection", "R3 Connection", "R4 Connection"
	     * Calling setAllMatchingLocalDataPointQualities("Connection", ITA_SCADA_BAD) will
	     * set "R1 Connection", "R2 Connection", "R3 Connection", "R4 Connection" to BAD.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param addressSubString    Substring to match local datapoint addresses with
	     * @param quality
	     *
	     */
		virtual void setAllLocalDataPointQualitiesMatchingAddress(const std::string& addressSubString, TA_Base_Core::EDataPointQuality quality, bool substr = true);

	    /**
	     * Register an observer for the notification of updates to external datapoints.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     *
	     * @param name    The entity name of the external datapoint
	     * @param observer    The class wanting to observe the update
	     *
	     */
		virtual void registerForExternalDataPointUpdate(const std::string& name, ItaScadaExternalDataPointUpdateObserver* observer);


		/**
		  * registerForExternalDataPointUpdate
		  * 
		  * Register an observer for the notification of updates to external datapoints.
		  * Note: no need to delete the entity, the Proxy will do it.
		  * 
		  * @return virtual void 
		  * @param : const TA_Base_Core::IEntityDataPtr entity
		  * @param : ItaScadaExternalDataPointUpdateObserver* observer
		  * 
		  * @exception <exceptions> Optional
		  * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
		  */
		virtual void registerForExternalDataPointUpdate(TA_Base_Core::IEntityDataPtr entity, ItaScadaExternalDataPointUpdateObserver* observer);

	    /**
	     * Write to a datapoint hosted by an external party using a session Id for rights
	     * checking.  If the session Id is NULL, then rights-checking is avoided.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param name    The entity name of the datapoint.
	     * @param value    The datapoint value to be written.  Ensure that the value is
	     * consistent with the datapoint configuration.
	     * @param sessionId    The operator sessionId for performing rights-checking.
	     *
	     */
		virtual void setExternalDataPointState(const std::string& name, const std::string& value, const std::string& sessionId = "");

	    /**
	     * Obtain the value from a datapoint hosted by an external party.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     *
	     * @param name
	     * @param returnedValue
	     * @param returnedQuality
	     *
	     */
		virtual void getExternalDataPointState(const std::string& name, std::string& returnedValue, TA_Base_Core::EDataPointQuality& returnedQuality);

	    /**
	     * Write to a local datapoint owned by this application.  The datapoint is
	     * localised by the datapoint entity name.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param name
	     * @param value
	     * @param quality
	     * @param timestamp
	     *
	     */
		virtual void setLocalDataPointStateByName(const std::string& name, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setLocalDataPointStateByName(const std::string& name, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setLocalDataPointStateByName(const std::string& name, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        virtual void setLocalDataPointStateByName(const std::string& name, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp);
        
	    /**
	     * Write to a local datapoint owned by this application.  The datapoint is
	     * localised by the datapoint address field.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
	     *
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param address
	     * @param value
	     * @param quality
	     * @param timestamp
	     *
	     */
        virtual void setLocalDataPointStateByAddress(const std::string& address, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false);
        virtual void setLocalDataPointStateByAddress(const std::string& address, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false);
        virtual void setLocalDataPointStateByAddress(const std::string& address, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false);
        virtual void setLocalDataPointStateByAddress(const std::string& address, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false);
        
	    /**
	     * Register an observer for the notification of updates to external datapoints.
	     *
	     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
	     *
	     *
	     * Returns value and quality as part of the call to allow atomic initial value
	     * retrieval and subscription. Avoids missed and out of sync updates.
	     * @param name
	     * @param observer    The class wanting to observe the update
	     * @param returnedValue
	     * @param returnedQuality
	     *
	     */
		virtual TA_Base_Bus::DpValue registerForExternalDataPointUpdateAndGetState(const std::string& name, ItaScadaExternalDataPointUpdateObserver* observer, TA_Base_Core::EDataPointQuality& returnedQuality);

		/**
		  * registerForExternalDataPointUpdateAndGetState
		  * 
		  * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
		  * Note: no need to delete the entity, the Proxy will do it.
		  * 
		  * @return virtual TA_Base_Bus::DpValue 
		  * @param : const TA_Base_Core::IEntityDataPtr entity
		  * @param : ItaScadaExternalDataPointUpdateObserver* observer
		  * @param : TA_Base_Core::EDataPointQuality& returnedQuality
		  * 
		  * @exception <exceptions> Optional
		  */
	    virtual TA_Base_Bus::DpValue registerForExternalDataPointUpdateAndGetState(TA_Base_Core::IEntityDataPtr entity, ItaScadaExternalDataPointUpdateObserver* observer, TA_Base_Core::EDataPointQuality& returnedQuality);



		//
		// Operations required by interface IEntityUpdateEventProcessor
		//


		/**
  		 * processEntityUpdateEvent()
  		 *
  		 * Callback method of the IEntityUpdateEventProcessor interface above.
		 * Gets called when a change takes place in a particular entity identified by 
		 * entityKey parameters
		 *
  		 */
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);


		//
		// Operations specific to CtaScada
		//


		/**
		* Creates a DataPoint entity and stores it internally for further reference.  Returns a 
		* non empty pointer to IEntity if the operation is succesful, or an empty pointer to IEntity
		* if the operation is failed.
		*
		* @param    entityData    The basic configuration data from the database
		*
		*/
		virtual TA_Base_Bus::IEntity * addDatapoint(TA_Base_Core::IEntityDataPtr entityData);

		/**
		* Creates a DataPoint entity and stores it internally for further reference.  Returns a 
		* non empty pointer to IEntity if the operation is succesful, or an empty pointer to IEntity
		* if the operation is failed.
		*
		* @param    entityData    The basic configuration data from the database
		*
		*/
		virtual TA_Base_Bus::IEntity * addDatapointWithPersistedFieldValue(TA_Base_Core::IEntityDataPtr entityData);


        /**
		* Remove the reference to the DataPoint whose pkey matched with the specified entity key.
		*
		* @param    entityKey    The pkey of the DataPoint entity to be removed.
		*
		*/
		virtual void removeDataPoint(unsigned long entityKey);

//		/**
//		* Update the DataPoint with data specified in the configuration event
//		*
//		* @param    updateEvent    The specific configuration event
//		*
//		*/
//		virtual void updateDataPointConfiguration ( const TA_Base_Core::ConfigUpdateDetails & updateEvent);

		/**
		* Set all DataPoint entities created via the addDataPoint operation to Control or Monitor mode.
		*
		* @param    wantControl    A boolean parameter indicating if all DataPoint entities created
		*						   by this class instance shall be set to Control mode
		*						   (if the parameter is TRUE) or Monitor mode (if the parameter is FALSE)
		*
		*/
		virtual void controlDatapoints(bool wantControl);

//		/**
//		* Deactive the Corba Servant of all DataPoint entities created via the addDataPoint operation.
//		*/
//		virtual void deactivateDatapoints();

	    /**
	      * getDataPoint 
	      * 
	      * inherit method from the IDataPointAgent class. return the datapoints of the particular entity
	      * 
	      * @return TA_Base_Bus::DataPoint * 
	      * @param : unsigned long entityKey
	      * 
	      * @exception <exceptions> Optional
	      */
        TA_Base_Bus::DataPoint * getDataPoint ( unsigned long entityKey );
        

        /**
		 * getAllDataPoints
		 *
		 * To be implemented by the specific data point agent.  The method allows the class
		 * DataPointAgentCorbaDef_Impl to obtain all the references to the data points which 
         * is created and maintained by the agent.
		 *
		 */
        virtual void getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList );

        //TD15690 Mintao
		/**
		* Find a reference to the DataPoint entity whose address matched with the specified entity
		* address.  Returns a non empty pointer to the DataPoint entity if the operation is successful
		* or an empty pointer if the operation is failed.
		*
		* @param    entityAddress    The entity address to be used as the search criteria.
		*
		*/
		virtual TA_Base_Bus::DataPoint * findDataPointWithEntityAddress ( const std::string & entityAddress );

	    /**
	      * setupScadaInterface
	      * 
	      * set up both the bulkpoll interface and dpaccess interface
	      * 
	      * @return void 
	      * @param : std::string name
	      * 
	      * @exception <exceptions> Optional
	      */
        void setupScadaInterface(std::string name);

        virtual TA_Base_Bus::DataPointFactory* getDataPointFactory();
        
    protected:

		/**
		* Find a reference to the DataPoint entity whose name matched with the specified entity name.
		* Returns a non empty pointer to the DataPoint entity if the operation is successful or an 
		* empty pointer if the operation is failed.
		*
		* @param    entityName    The entity name to be used as the search criteria.
		*
		*/
		virtual TA_Base_Bus::DataPoint * findDataPointWithEntityName ( const std::string & entityName );

		/**
		* Find the references to all DataPoint entity whose addresses matched with the specified entity
		* address.
		*
		* @param    listOfDataPointsWithSameAddress		The list of DataPoints that have the same address as the specified
		* @param    entityAddress						The entity address to be used as the search criteria.
		* @param    substring							Indication if the specified address is a whole or part of the entity address
		*
		*/
		virtual void findAllDataPointWithEntityAddress ( const std::string & entityAddress, std::vector < TA_Base_Bus::DataPoint * > & listOfDataPointsWithSameAddress, bool substring = false );

		/**
		* Get a reference to the DataPoint Proxy whose name matched with the specified entity
		* name.  Returns a non empty pointer to the DataPoint Proxy if the operation is successful
		* or an empty pointer if the operation is failed.
		*
		* @param    entityName    The entity name to be used as the search criteria.
		*
		*/
		virtual TA_Base_Bus::DataPointProxySmartPtr * getDataPointProxyWithEntityName ( const std::string & entityName );

		virtual TA_Base_Bus::DataPointProxySmartPtr * getDataPointProxyWithEntityData (TA_Base_Core::IEntityDataPtr entity );

		/**
		* Convert the specified value as a string to TA_Base_Bus::DpValue for the specified DataPoint
		*
		* @param    dataPoint   The data point to be used in the conversion process
		* @param    value		The value as a string to be converted to TA_Base_Bus::DpValue
		*
		*/
		virtual TA_Base_Bus::DpValue stringToValue(TA_Base_Bus::DataPoint & dataPoint, const std::string & value);


		volatile bool m_inControlMode;

		TA_Base_Bus::DataPointFactory * m_dataPointFactory;

		std::map < std::string, TA_Base_Bus::DataPointProxySmartPtr * >	m_externalDataPointEntityNameMap;
		std::map < unsigned long, TA_Base_Bus::DataPoint * >			m_internalDataPointEntityKeyMap;
        
        typedef std::vector < TA_Base_Bus::DataPoint * >                InternalDataPointVector;
        typedef std::map < std::string, InternalDataPointVector >       InternalDataPointAddressMap;
        InternalDataPointAddressMap                                     m_internalDataPointAddressMap;

		std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * > m_proxyNameUpdateObserverMap;

		TA_Base_Core::ReEntrantThreadLockable m_externalDataPointEntityNameMapLock;
		TA_Base_Core::ReEntrantThreadLockable m_internalDataPointEntityKeyMapLock;
		TA_Base_Core::ReEntrantThreadLockable m_proxyNameUpdateHandlerMapLock;
        TA_Base_Core::ReEntrantThreadLockable m_internalDataPointAddressMapLock;
        
	private:
	    /**
	      * setupBulkPolledInterface
	      * 
	      * set up the entity name which is used later 
	      * 
	      * @return void 
	      * @param : std::string name
	      * 
	      * @exception <exceptions> Optional
	      */
        void setupBulkPolledInterface(std::string name);



		/**
		  * setupDatapointAccessInterface
		  * 
		  * setup Datapoint Access Interface
		  * 
		  * @return void 
		  * @param : std::string name
		  * 
		  * @exception <exceptions> Optional
		  */
		void setupDatapointAccessInterface(std::string name);

        TA_Base_Bus::DataPointAgentCorbaDef_Impl* m_bulkPolledInterface;
		TA_Base_Bus::DataPointAccessCorbaDef_Impl* m_dataPointAccessInterface;


		//
		// Declare the copy constructors as private to prevent pass-by-value
		//
		CtaScada ( const CtaScada & theInstance );
		CtaScada & operator= ( const CtaScada & rhs );
    };
}

#endif // !defined(CtaScada_032038ED_551E_45bf_91E2_A254B87D7BAF__INCLUDED_)
