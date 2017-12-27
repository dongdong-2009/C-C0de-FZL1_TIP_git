/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/ItaScada/src/CtaScada.cpp $
  * @author  Sean Liew
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2016/01/18 15:18:43 $
  * Last modified by : $Author: Ouyang $
  *
  * Implementation of the Class CtaScada
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4250)
#pragma warning(disable:4503)
#endif // defined (_MSC_VER)

#include <algorithm>

#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/CtaScadaExceptions.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/types/public_types/Constants.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "bus/scada/ItaScada/public_interfaces/ItaScadaExternalDataPointUpdateObserver.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/ItaScada/src/CtaScada.h"

namespace TA_Base_Bus
{
	//
	// constructors and destructor
	//

	CtaScada::CtaScada ( const CtaScada & theInstance )
	{
		throw;
	}


	CtaScada & CtaScada::operator= ( const CtaScada & rhs )
	{
		throw;
	}


    CtaScada::CtaScada()
	:
	m_inControlMode ( false ),
	m_dataPointFactory ( 0 ),
	m_bulkPolledInterface(NULL),
	m_dataPointAccessInterface(NULL)
	{
		// initialise internal lists
		m_externalDataPointEntityNameMap.clear();		
		m_internalDataPointEntityKeyMap.clear();
        m_internalDataPointAddressMap.clear();
		m_proxyNameUpdateObserverMap.clear();

		// create handle to DataPointFactory
		m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);

    }


    CtaScada::~CtaScada()
	{
		// clean up the internal DataPoints.  Note that the entities created by instance
		// of this class are actually owned by the GenericAgent frame work
		// and therefore we leave it to the GenericAgent to delete the objects.		
		m_internalDataPointEntityKeyMap.clear();

		// clean up the proxies
		std::map < std::string, TA_Base_Bus::DataPointProxySmartPtr * >::iterator itr1;
		for ( itr1 =	m_externalDataPointEntityNameMap.begin();
			  itr1 != m_externalDataPointEntityNameMap.end();
			  itr1++ )
		{
			delete (*itr1).second;
			(*itr1).second = 0;
		}
		m_externalDataPointEntityNameMap.clear();

		// cannot delete the Observers
		std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * >::iterator itr2;
		for ( itr2 = m_proxyNameUpdateObserverMap.begin();
			  itr2 != m_proxyNameUpdateObserverMap.end();
			  itr2++ )
		{
			(*itr2).second->clear();
			delete (*itr2).second;
			(*itr2).second = 0;
		}
		m_proxyNameUpdateObserverMap.clear();

		// clean up handle to DataPointFactory
		if ( 0 != m_dataPointFactory )
		{
			delete m_dataPointFactory;
			m_dataPointFactory = 0;
		}
        // if bulk interface is created
        if ( NULL != m_bulkPolledInterface )
        {
			delete m_bulkPolledInterface;
	        m_bulkPolledInterface = NULL;
        } 

		if(NULL != m_dataPointAccessInterface)
		{
			delete m_dataPointAccessInterface;
			m_dataPointAccessInterface = NULL;
		}
    }

    void CtaScada::setupScadaInterface(std::string name)
    {
		FUNCTION_ENTRY("setupScadaInterface");
        setupBulkPolledInterface(name);
        setupDatapointAccessInterface(name);
		FUNCTION_EXIT;
    }

    void CtaScada::setupBulkPolledInterface(std::string name)
    {
		FUNCTION_ENTRY("setupBulkPolledInterface");
	    // create an instance of the class DataPointAgentCorbaDef_Impl
        m_bulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl (name, this );
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "bulk polling interface has been created in ctaScada.");
		FUNCTION_EXIT;
    }

	void CtaScada::setupDatapointAccessInterface(std::string name)
	{
		FUNCTION_ENTRY("setupDatapointAccessInterface");
		// create an instance of the class DataPointAgentCorbaDef_Impl
        m_dataPointAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl (name, this );
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "DataPoint Access Interface has been created in ctaScada.");
		FUNCTION_EXIT;
	}


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
	void CtaScada::setExternalDataPointValue(const std::string& name,const std::string& value,const std::string& sessionId /*=""*/)
	{
		FUNCTION_ENTRY("setExternalDataPointValue");

		// obtain the handle to one of the proxy in the list whose name matches with
		// the specified name
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		dataPointProxy = getDataPointProxyWithEntityName ( name );

		// if able to get a handle to the specified proxy
		if ( 0 != dataPointProxy )
		{
			try
			{
				// set the specified value to the proxy
				(*dataPointProxy)->setValue ( value, sessionId );
			}
			catch ( TA_Base_Core::ScadaProxyException & ex )
			{
				TA_THROW (TA_Base_Core::CtaScadaWriteException ( ex.what() ));
			}
			catch ( ... )
			{
				std::ostringstream ss;
				ss << "Unknown exception from DataPointProxy with name: {" << name << "} Value {" << value << "} cannot be set";

				TA_THROW (TA_Base_Core::CtaScadaWriteException ( ss.str().c_str() ));
			}
		}
		else
		{
			std::ostringstream ss;
			ss << "Cannot find DataPointProxy with name: {" << name << "} Value {" << value << "} cannot be set";

			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(ss.str().c_str()));
		}

		FUNCTION_EXIT;
    }


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
	void CtaScada::getExternalDataPointValue(const std::string& name,std::string& returnedValue,TA_Base_Core::EDataPointQuality& returnedQuality)
	{
		FUNCTION_ENTRY("getExternalDataPointValue");

		// obtain the handle to one of the proxy in the list whose name matches with
		// the specified name
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		dataPointProxy = getDataPointProxyWithEntityName ( name );

		// if able to get a handle to the specified proxy
		if ( 0 != dataPointProxy )
		{
			try
			{
				// get the value from the proxy
				returnedValue = (*dataPointProxy)->getValueAsString();

				// get the quality from the proxy, convert it into EDataPointQuality
				returnedQuality = ( ( TA_Base_Core::ITA_QUALITY_GOOD > (*dataPointProxy)->getStatus() ) ? TA_Base_Core::ITA_QUALITY_BAD : TA_Base_Core::ITA_QUALITY_GOOD );
			}
			catch ( TA_Base_Core::ScadaProxyException & ex )
			{
				TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException ( ex.what() ));
			}
			catch ( ... )
			{
				std::stringstream ss;
				ss << "Unknown exception from DataPointProxy with name: {" << name << "} Cannot get value";

				TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException ( ss.str().c_str() ));
			}
		}
		else
		{
			std::ostringstream ss;
			ss << "Cannot find DataPointProxy with name: {" << name << "} Cannot get value";

			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(ss.str().c_str()));
		}

		FUNCTION_EXIT;
    }


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
	void CtaScada::setLocalDataPoint(DataPoint& dataPoint,const std::string& value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
	{
		FUNCTION_ENTRY("setLocalDataPoint (string)");

        TA_Base_Bus::DpValue dpValue(stringToValue ( dataPoint, value ));
		updateFieldState(dataPoint, dpValue, timestamp, quality);

		FUNCTION_EXIT;
    }


	void CtaScada::setLocalDataPoint(DataPoint& dataPoint,const double value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
	{
		FUNCTION_ENTRY("setLocalDataPoint (double)");

		switch (dataPoint.getDataPointDataType())
		{
		case TA_Base_Bus::DPT_INT16_TYPE:
		case TA_Base_Bus::DPT_INT32_TYPE:
		case TA_Base_Bus::DPT_INT16S_TYPE:
		case TA_Base_Bus::DPT_INT32S_TYPE:
		case TA_Base_Bus::DPT_UINT16_TYPE:
		case TA_Base_Bus::DPT_UINT32_TYPE:
		case TA_Base_Bus::DPT_IEEE32_TYPE:
			{
				TA_Base_Bus::DpValue dpValue(dataPoint.getDataPointDataType(), dataPoint.getEnumLabelsMap(), dataPoint.getBooleanLabels());
				dpValue.setFloat ( value );

				updateFieldState(dataPoint, dpValue, timestamp, quality);
			}
			break;

		default:
			{
				std::ostringstream ss;
				ss << "Datapoint with name {" << dataPoint.getDataPointName() << "} has unsupported datapoint datatype: {" << int(dataPoint.getDataPointDataType()) << "}";

				TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(ss.str().c_str()));

			}
		}

		FUNCTION_EXIT;
    }


	void CtaScada::setLocalDataPoint(DataPoint& dataPoint,const unsigned long value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
	{
		FUNCTION_ENTRY("setLocalDataPoint (unsigned long)");

		switch (dataPoint.getDataPointDataType())
		{
		case TA_Base_Bus::DPT_ENUM_TYPE:
			{
				TA_Base_Bus::DpValue dpValue(dataPoint.getDataPointDataType(), dataPoint.getEnumLabelsMap(), dataPoint.getBooleanLabels());
				dpValue.setEnum ( value );

				updateFieldState(dataPoint, dpValue, timestamp, quality);
			}
			break;

        case TA_Base_Bus::DPT_INT16_TYPE:
		case TA_Base_Bus::DPT_INT32_TYPE:
		case TA_Base_Bus::DPT_INT16S_TYPE:
		case TA_Base_Bus::DPT_INT32S_TYPE:
		case TA_Base_Bus::DPT_UINT16_TYPE:
		case TA_Base_Bus::DPT_UINT32_TYPE:
		case TA_Base_Bus::DPT_IEEE32_TYPE:
			{
				TA_Base_Bus::DpValue dpValue(dataPoint.getDataPointDataType(), dataPoint.getEnumLabelsMap(), dataPoint.getBooleanLabels());
                double newValue = value;
				dpValue.setFloat ( value );

				updateFieldState(dataPoint, dpValue, timestamp, quality);
			}
			break;

		default:
			{
				std::ostringstream ss;
				ss << "Datapoint with name {" << dataPoint.getDataPointName() << "} has unsupported datapoint datatype: {" << int(dataPoint.getDataPointDataType()) << "}";

				TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(ss.str().c_str()));

			}
		}

		FUNCTION_EXIT;
    }


	void CtaScada::setLocalDataPoint(DataPoint& dataPoint,const bool value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
	{
		FUNCTION_ENTRY("setLocalDataPoint (bool)");

		switch (dataPoint.getDataPointDataType())
		{
		case TA_Base_Bus::DPT_BOOL_TYPE:
			{
				TA_Base_Bus::DpValue dpValue(dataPoint.getDataPointDataType(), dataPoint.getEnumLabelsMap(), dataPoint.getBooleanLabels());
				dpValue.setBoolean ( value );

				updateFieldState(dataPoint, dpValue, timestamp, quality);
			}
			break;
		
		default:
			{
				std::ostringstream ss;
				ss << "Datapoint with name {" << dataPoint.getDataPointName() << "} has unsupported datapoint datatype: {" << int(dataPoint.getDataPointDataType()) << "}";

				TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(ss.str().c_str()));

			}
		}

		FUNCTION_EXIT;
    }


	void CtaScada::updateFieldState (DataPoint& dataPoint, DpValue& dpValue, time_t timestamp, TA_Base_Core::EDataPointQuality quality)
	{
		try
		{
			//
			// update the DataPoint with the specified value/quality/timestamp
			//

			timeb newTimestamp = TA_Base_Core::defaultTime;
			newTimestamp.time = timestamp;
			TA_Base_Bus::DataPointState newState ( dpValue, newTimestamp , (TA_Base_Bus::EQualityStatus) quality );
			dataPoint.updateFieldState( newState );
		}
		catch ( ... )
		{
			std::ostringstream ss;
			ss << "Unexpected exception from Datapoint with name {" << dataPoint.getDataPointName() << "}";

			TA_THROW (TA_Base_Core::CtaScadaWriteException(ss.str().c_str()));
		}
	}


    //Mintao++
    //TD13906
    /**
     * Read a local datapoint owned by this application.  The datapoint is
     * localised by the datapoint address field.
     *
     * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
     *
     * Throws CtaScadaInvalidConfigurationException if the datapoint type is incorrect.
     *
     * Throws CtaScadaWriteException if the write operation fails.
     * @param address
     *
     */
    void CtaScada::getLocalDataPointStateByUniqueAddress(const std::string& address,std::string& returnedValue,TA_Base_Core::EDataPointQuality& returnedQuality)
	{
		FUNCTION_ENTRY("getLocalDataPointByUniqueAddress");
        
		// obtain the handles to all DataPoints in the list whose addresses matches with
		// the specified address
		InternalDataPointVector listOfDataPointsWithSameAddress;
		listOfDataPointsWithSameAddress.clear();

		findAllDataPointWithEntityAddress ( address, listOfDataPointsWithSameAddress );

		// if able to get handles to the specified DataPoints
        // the datapoint address must be unique locally, otherwise the invalid configuration exception will be thrown
        //TD13906
        //Mintao++
        if ( 1 == listOfDataPointsWithSameAddress.size() )
		{
            TA_Base_Bus::DpValue currentValue = listOfDataPointsWithSameAddress[0]->getCurrentValue();
            returnedValue = currentValue.getValueAsString();
			// get the quality from the datpoint, convert it into EDataPointQuality
            TA_Base_Bus::EQualityStatus currentStatus = listOfDataPointsWithSameAddress[0]->getQualityStatus();
			returnedQuality = ( ( TA_Base_Core::ITA_QUALITY_GOOD > currentStatus) ? TA_Base_Core::ITA_QUALITY_BAD : TA_Base_Core::ITA_QUALITY_GOOD );
        }
		else
		{
			std::ostringstream ss;
			ss << "Found " << listOfDataPointsWithSameAddress.size() << " Datapoints with address {" << address << "} - not unique!";

			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(ss.str().c_str()));
		}

		FUNCTION_EXIT;
    }

    //Mintao++
    //TD13910
    std::map < unsigned long, TA_Base_Bus::DataPoint * > CtaScada::getAllLocalDataPoints()
    {
		FUNCTION_ENTRY("getAllLocalDataPoints");

		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );
		FUNCTION_EXIT;
        return m_internalDataPointEntityKeyMap;
    }
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
	void CtaScada::setAllLocalDataPointQualities(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("setAllLocalDataPointQualities");

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

		bool CtaScadaWriteException = false;

		if ( 0 < m_internalDataPointEntityKeyMap.size() )
		{
			// for each of the DataPoint in the list
			std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;
			for ( itr = m_internalDataPointEntityKeyMap.begin();
				  itr != m_internalDataPointEntityKeyMap.end();
				  itr++ )
			{
				try
				{
					// get the current timestamp and value of the DataPoint
					timeb currentTimestamp = (*itr).second->getTimebStructure();
					TA_Base_Bus::DpValue dpValue ( (*itr).second->getCurrentValue() );

					// update the DataPoint with same timestamp, value and the new quality status
					TA_Base_Bus::DataPointState newState ( dpValue, currentTimestamp , (TA_Base_Bus::EQualityStatus) quality );
					(*itr).second->updateFieldState( newState );
				}
				catch ( ... )
				{
					std::stringstream ss;
					ss << "Error while trying to update the data point " << (*itr).second->getDataPointName() << " with quality status = " << quality;
					LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str() );

					CtaScadaWriteException = true;
				}
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints found - cannot set quality to " << int(quality);
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}

		if ( true == CtaScadaWriteException )
		{
			std::ostringstream message;
			message << "Write errors found - see previous instrumentation";
			TA_THROW (TA_Base_Core::CtaScadaWriteException(message.str().c_str()));
		}

		FUNCTION_EXIT;
    }

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
	 * @param address
	 *
	 */
    void CtaScada::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp)
    {
        FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");
        
        setLocalDataPointStateByAddress(addressSubString, value, quality, timestamp, true);
        
        FUNCTION_EXIT;
    }
    
    void CtaScada::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp)
    {
        FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");
        
        setLocalDataPointStateByAddress(addressSubString, value, quality, timestamp, true);
        
        FUNCTION_EXIT;
    }
    
    void CtaScada::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp)
    {
        FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");
        
        setLocalDataPointStateByAddress(addressSubString, value, quality, timestamp, true);
        
        FUNCTION_EXIT;
    }
    
    void CtaScada::setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp)
    {
        FUNCTION_ENTRY("setAllLocalDataPointStateMatchingAddress");
        
        setLocalDataPointStateByAddress(addressSubString, value, quality, timestamp, true);
        
        FUNCTION_EXIT;
    }
    
    
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
	void CtaScada::setAllLocalDataPointQualitiesMatchingAddress(const std::string& addressSubString, TA_Base_Core::EDataPointQuality quality, bool substr /* = true */)
	{
		FUNCTION_ENTRY("setAllLocalDataPointQualitiesMatchingAddress");

		bool CtaScadaWriteException = false;

		// obtain the handles to all DataPoints in the list whose addresses matches with
		// the specified address
		InternalDataPointVector listOfDataPointsWithSameAddress;
		listOfDataPointsWithSameAddress.clear();

		findAllDataPointWithEntityAddress ( addressSubString, listOfDataPointsWithSameAddress, substr );

		// if able to get handles to the specified DataPoints
		if ( 0 < listOfDataPointsWithSameAddress.size() )
		{
			// for each of the DataPoint in the list
			InternalDataPointVector::iterator itr;
			for ( itr = listOfDataPointsWithSameAddress.begin();
				  itr != listOfDataPointsWithSameAddress.end();
				  itr++ )
			{
				try
				{
					// get the current timestamp and value of the DataPoint
					timeb currentTimestamp = (*itr)->getTimebStructure();
					TA_Base_Bus::DpValue dpValue ( (*itr)->getCurrentValue() );

					// update the DataPoint with same timestamp, value and the new quality status
					TA_Base_Bus::DataPointState newState ( dpValue, currentTimestamp , (TA_Base_Bus::EQualityStatus) quality );
					(*itr)->updateFieldState( newState );
				}
				catch ( ... )
				{
					std::stringstream ss;
					ss << "Error while trying to update the data point " << (*itr)->getDataPointName() << " with quality status = " << quality;
					LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, ss.str().c_str() );

					CtaScadaWriteException = true;
				}
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints found - cannot set quality to " << int(quality);
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}

		if ( true == CtaScadaWriteException )
		{
			std::ostringstream message;
			message << "Write errors found - see previous instrumentation";
			TA_THROW (TA_Base_Core::CtaScadaWriteException(message.str().c_str()));
		}

		FUNCTION_EXIT;
	}


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
	void CtaScada::registerForExternalDataPointUpdate(const std::string& name,ItaScadaExternalDataPointUpdateObserver* observer)
	{
		FUNCTION_ENTRY("registerForExternalDataPointUpdate");

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_proxyNameUpdateHandlerMapLock );

		// obtain the handle to one of the proxy in the list whose name matches with
		// the specified name
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		dataPointProxy = getDataPointProxyWithEntityName ( name );

		// if able to get a handle to the specified proxy
		if ( 0 != dataPointProxy )
		{
			// find if we already have observers for this proxy
			std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * >::iterator mapItr = m_proxyNameUpdateObserverMap.find ( name );

			// if found one
			if ( m_proxyNameUpdateObserverMap.end() != mapItr )
			{
				// save the specified observer for later notification
				(*mapItr).second->push_back ( observer );
			}
			// if found none
			else
			{
				// create an entry in the list for the specified proxy and observer
				std::vector < ItaScadaExternalDataPointUpdateObserver * > * listOfObservers = new std::vector < ItaScadaExternalDataPointUpdateObserver * >;
				listOfObservers->push_back ( observer );
				m_proxyNameUpdateObserverMap [ name ] = listOfObservers;
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints found with name {" << name << "} Cannot register observer";
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}
		
		FUNCTION_EXIT;
    }

	void CtaScada::registerForExternalDataPointUpdate(TA_Base_Core::IEntityDataPtr entity, ItaScadaExternalDataPointUpdateObserver* observer)
	{
		FUNCTION_ENTRY("registerForExternalDataPointUpdate");
		
		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_proxyNameUpdateHandlerMapLock );
		
		// obtain the handle to one of the proxy in the list whose name matches with
		// the specified name
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		dataPointProxy = getDataPointProxyWithEntityData ( entity );
		
		// if able to get a handle to the specified proxy
		if ( 0 != dataPointProxy )
		{
			std::string entityName = entity->getName() ;
			// find if we already have observers for this proxy
			std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * >::iterator mapItr = m_proxyNameUpdateObserverMap.find ( entityName );
			
			// if found one
			if ( m_proxyNameUpdateObserverMap.end() != mapItr )
			{
				// save the specified observer for later notification
				(*mapItr).second->push_back ( observer );
			}
			// if found none
			else
			{
				// create an entry in the list for the specified proxy and observer
				std::vector < ItaScadaExternalDataPointUpdateObserver * > * listOfObservers = new std::vector < ItaScadaExternalDataPointUpdateObserver * >;
				listOfObservers->push_back ( observer );
				m_proxyNameUpdateObserverMap [ entityName ] = listOfObservers;
			}
		}
		else
		{
			std::ostringstream message;
			message << "Entity with name {" << entity->getName() << "} is not a DataPoint";
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}
		
		FUNCTION_EXIT;
    }


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
	void CtaScada::setExternalDataPointState(const std::string& name,const std::string& value,const std::string& sessionId /*=""*/)
	{
		FUNCTION_ENTRY("setExternalDataPointState");

		// why is this operation the same as setExternalDataPointValue....?
		// pass all input parameter to setExternalDataPointValue

		setExternalDataPointValue ( name, value, sessionId );

		FUNCTION_EXIT;
    }


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
	void CtaScada::getExternalDataPointState(const std::string& name,std::string& returnedValue,TA_Base_Core::EDataPointQuality& returnedQuality)
	{
		FUNCTION_ENTRY("getExternalDataPointState");

		// why is this operation the same as getExternalDataPointValue....?
		// pass all input parameter to setLocalDataPointByName 

		getExternalDataPointValue ( name, returnedValue, returnedQuality );

		FUNCTION_EXIT;
    }


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
    void CtaScada::setLocalDataPointStateByName(const std::string& name,const std::string& value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
    {
        FUNCTION_ENTRY("setLocalDataPointStateByName");
        
        // obtain the handle to one of the DataPoint in the list
        TA_Base_Bus::DataPoint * dataPoint = 0;
        dataPoint = findDataPointWithEntityName ( name );
        
        // if able to get a handle to the specified DataPoint
        if ( 0 != dataPoint )
        {
            setLocalDataPoint ( *dataPoint, value, quality, timestamp );
        }
        else
        {
            std::ostringstream message;
		message << "Cannot find DataPoint with name {" << name << "} Value {" << value << "} is not set";
		TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
        }
        
        FUNCTION_EXIT;
    }
    
    void CtaScada::setLocalDataPointStateByName(const std::string& name,const double value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
    {
        FUNCTION_ENTRY("setLocalDataPointStateByName");
        
        // obtain the handle to one of the DataPoint in the list
        TA_Base_Bus::DataPoint * dataPoint = 0;
        dataPoint = findDataPointWithEntityName ( name );
        
        // if able to get a handle to the specified DataPoint
        if ( 0 != dataPoint )
        {
            setLocalDataPoint ( *dataPoint, value, quality, timestamp );
        }
        else
        {
            std::ostringstream message;
		message << "Cannot find DataPoint with name {" << name << "} Value {" << value << "} is not set";
		TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
        }
        
        FUNCTION_EXIT;
    }
    
    void CtaScada::setLocalDataPointStateByName(const std::string& name,const unsigned long value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
    {
        FUNCTION_ENTRY("setLocalDataPointStateByName");
        
        // obtain the handle to one of the DataPoint in the list
        TA_Base_Bus::DataPoint * dataPoint = 0;
        dataPoint = findDataPointWithEntityName ( name );
        
        // if able to get a handle to the specified DataPoint
        if ( 0 != dataPoint )
        {
            setLocalDataPoint ( *dataPoint, value, quality, timestamp );
        }
        else
        {
            std::ostringstream message;
		message << "Cannot find DataPoint with name {" << name << "} Value {" << value << "} is not set";
		TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
        }
        
        FUNCTION_EXIT;
    }
    
    void CtaScada::setLocalDataPointStateByName(const std::string& name,const bool value,TA_Base_Core::EDataPointQuality quality,time_t timestamp)
    {
        FUNCTION_ENTRY("setLocalDataPointStateByName");
        
        // obtain the handle to one of the DataPoint in the list
        TA_Base_Bus::DataPoint * dataPoint = 0;
        dataPoint = findDataPointWithEntityName ( name );
        
        // if able to get a handle to the specified DataPoint
        if ( 0 != dataPoint )
        {
            setLocalDataPoint ( *dataPoint, value, quality, timestamp );
        }
        else
        {
		std::ostringstream message;
		message << "Cannot find DataPoint with name {" << name << "} Value {" << (value?"true":"false") << "} is not set";
		TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
        }
        
        FUNCTION_EXIT;
    }
    

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
	void CtaScada::setLocalDataPointStateByAddress(const std::string& address,const std::string& value,TA_Base_Core::EDataPointQuality quality,time_t timestamp, bool substr /* = false */)
	{
		FUNCTION_ENTRY("setLocalDataPointStateByAddress");

		// obtain the handles to all DataPoints in the list whose addresses matches with
		// the specified address
		InternalDataPointVector listOfDataPointsWithSameAddress;
		listOfDataPointsWithSameAddress.clear();

		findAllDataPointWithEntityAddress ( address, listOfDataPointsWithSameAddress, substr );

		// if able to get handles to the specified DataPoints
		if ( 0 < listOfDataPointsWithSameAddress.size() )
		{
			// for each of the DataPoint in the list
			InternalDataPointVector::iterator itr;
			for ( itr = listOfDataPointsWithSameAddress.begin();
				  itr != listOfDataPointsWithSameAddress.end();
				  itr++ )
			{
				setLocalDataPoint ( **itr, value, quality, timestamp );
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints with address matching " << address << ": cannot set value to " << value;
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}

		FUNCTION_EXIT;
    }

	void CtaScada::setLocalDataPointStateByAddress(const std::string& address,const double value,TA_Base_Core::EDataPointQuality quality,time_t timestamp, bool substr /* = false */)
	{
		FUNCTION_ENTRY("setLocalDataPointStateByAddress (double)");

		// obtain the handles to all DataPoints in the list whose addresses matches with
		// the specified address
		InternalDataPointVector listOfDataPointsWithSameAddress;
		listOfDataPointsWithSameAddress.clear();

		findAllDataPointWithEntityAddress ( address, listOfDataPointsWithSameAddress, substr );

		// if able to get handles to the specified DataPoints
		if ( 0 < listOfDataPointsWithSameAddress.size() )
		{
			// for each of the DataPoint in the list
			InternalDataPointVector::iterator itr;
			for ( itr = listOfDataPointsWithSameAddress.begin();
				  itr != listOfDataPointsWithSameAddress.end();
				  itr++ )
			{
				setLocalDataPoint ( **itr, value, quality, timestamp );
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints with address matching " << address << ": cannot set value to " << value;
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}

		FUNCTION_EXIT;
    }

	void CtaScada::setLocalDataPointStateByAddress(const std::string& address,const unsigned long value,TA_Base_Core::EDataPointQuality quality,time_t timestamp, bool substr /* = false */)
	{
		FUNCTION_ENTRY("setLocalDataPointStateByAddress (unsigned long)");

		// obtain the handles to all DataPoints in the list whose addresses matches with
		// the specified address
		InternalDataPointVector listOfDataPointsWithSameAddress;
		listOfDataPointsWithSameAddress.clear();

		findAllDataPointWithEntityAddress ( address, listOfDataPointsWithSameAddress, substr );

		// if able to get handles to the specified DataPoints
		if ( 0 < listOfDataPointsWithSameAddress.size() )
		{
			// for each of the DataPoint in the list
			InternalDataPointVector::iterator itr;
			for ( itr = listOfDataPointsWithSameAddress.begin();
				  itr != listOfDataPointsWithSameAddress.end();
				  itr++ )
			{
				setLocalDataPoint ( **itr, value, quality, timestamp );
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints with address matching " << address << ": cannot set value to " << value;
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}

		FUNCTION_EXIT;
    }

	void CtaScada::setLocalDataPointStateByAddress(const std::string& address,const bool value,TA_Base_Core::EDataPointQuality quality,time_t timestamp, bool substr /* = false */)
	{
		FUNCTION_ENTRY("setLocalDataPointStateByAddress (bool)");

		// obtain the handles to all DataPoints in the list whose addresses matches with
		// the specified address
		InternalDataPointVector listOfDataPointsWithSameAddress;
		listOfDataPointsWithSameAddress.clear();

		findAllDataPointWithEntityAddress ( address, listOfDataPointsWithSameAddress, substr );

		// if able to get handles to the specified DataPoints
		if ( 0 < listOfDataPointsWithSameAddress.size() )
		{
			// for each of the DataPoint in the list
			InternalDataPointVector::iterator itr;
			for ( itr = listOfDataPointsWithSameAddress.begin();
				  itr != listOfDataPointsWithSameAddress.end();
				  itr++ )
			{
				setLocalDataPoint ( **itr, value, quality, timestamp );
			}
		}
		else
		{
			std::ostringstream message;
			message << "No Datapoints with address matching " << address << ": cannot set value to " << (value?"true":"false");
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}

		FUNCTION_EXIT;
    }


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
	TA_Base_Bus::DpValue CtaScada::registerForExternalDataPointUpdateAndGetState(const std::string& name, ItaScadaExternalDataPointUpdateObserver* observer, TA_Base_Core::EDataPointQuality& returnedQuality)
	{
		FUNCTION_ENTRY("registerForExternalDataPointUpdateAndGetState");

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_proxyNameUpdateHandlerMapLock );

		// obtain the handle to one of the proxy in the list whose name matches with
		// the specified name
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		dataPointProxy = getDataPointProxyWithEntityName ( name );

		// if able to get a handle to the specified proxy
		if ( 0 == dataPointProxy )
		{
			std::ostringstream message;
			message << "No Datapoints with name matching " << name << ": can be registered for update.";
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}
        
        // find if we already have observers for this proxy
        std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * >::iterator mapItr = m_proxyNameUpdateObserverMap.find ( name );

        // if found one
        if ( m_proxyNameUpdateObserverMap.end() != mapItr )
        {
            // save the specified observer for later notification
            (*mapItr).second->push_back ( observer );
        }
        else    // if none found
        {
            // create an entry in the list for the specified proxy and observer
            std::vector < ItaScadaExternalDataPointUpdateObserver * > * listOfObservers = new std::vector < ItaScadaExternalDataPointUpdateObserver * >;
            listOfObservers->push_back ( observer );
            m_proxyNameUpdateObserverMap [ name ] = listOfObservers;
        }

        // assign the returned value and quality
        returnedQuality = ( ( TA_Base_Core::ITA_QUALITY_GOOD > (*dataPointProxy)->getStatus() ) ? TA_Base_Core::ITA_QUALITY_BAD : TA_Base_Core::ITA_QUALITY_GOOD );
		
		FUNCTION_EXIT;
		return (*dataPointProxy)->getValue();
    }

	TA_Base_Bus::DpValue CtaScada::registerForExternalDataPointUpdateAndGetState(TA_Base_Core::IEntityDataPtr entity, ItaScadaExternalDataPointUpdateObserver* observer, TA_Base_Core::EDataPointQuality& returnedQuality)
	{
		FUNCTION_ENTRY("registerForExternalDataPointUpdateAndGetState");
		
		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_proxyNameUpdateHandlerMapLock );
		
		// obtain the handle to one of the proxy in the list whose name matches with
		// the specified name
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		dataPointProxy = getDataPointProxyWithEntityData ( entity );
		
		// if able to get a handle to the specified proxy
		if ( 0 == dataPointProxy )
		{
			std::ostringstream message;
			message << "No Datapoints with Entity name matching " << entity->getName() << ": can be registered for update.";
			TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
		}
        
		std::string entityName = entity->getName();

        // find if we already have observers for this proxy
        std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * >::iterator mapItr = m_proxyNameUpdateObserverMap.find ( entityName );
		
        // if found one
        if ( m_proxyNameUpdateObserverMap.end() != mapItr )
        {
            // save the specified observer for later notification
            (*mapItr).second->push_back ( observer );
        }
        else    // if none found
        {
            // create an entry in the list for the specified proxy and observer
            std::vector < ItaScadaExternalDataPointUpdateObserver * > * listOfObservers = new std::vector < ItaScadaExternalDataPointUpdateObserver * >;
            listOfObservers->push_back ( observer );
            m_proxyNameUpdateObserverMap [ entityName ] = listOfObservers;
        }
		
        // assign the returned value and quality
        returnedQuality = ( ( TA_Base_Core::ITA_QUALITY_GOOD > (*dataPointProxy)->getStatus() ) ? TA_Base_Core::ITA_QUALITY_BAD : TA_Base_Core::ITA_QUALITY_GOOD );
		
		FUNCTION_EXIT;
		return (*dataPointProxy)->getValue();
    }

	/**
	* Creates a DataPoint entity and stores it internally for further reference.  Returns a 
	* non empty pointer to IEntity if the operation is succesful, or an empty pointer to IEntity
	* if the operation is failed.
	*
	* @param    entityData    The basic configuration data from the database
	*
	*/
	TA_Base_Bus::IEntity* CtaScada::addDatapoint(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("addDatapoint");

		TA_Base_Bus::DataPoint * dataPointEntity = 0;

		try
		{
			// cast the entityData to DataPointEntityData
            TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
                boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(entityData);
			
			// if the cast is successful, i.e the db entity is a DataPoint
			if ( dataPointEntityData.get() != NULL )
			{
				std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;

				// find if we already have the specified DataPoint entity created
				{
					// stop multi threads accessing this list
					TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

					itr = m_internalDataPointEntityKeyMap.find ( dataPointEntityData->getKey() );
				}

				// if does not exist yet
				if ( m_internalDataPointEntityKeyMap.end() == itr )
				{
					// create a DataPoint based on the specified entity data
					dataPointEntity = m_dataPointFactory->createDataPoint ( dataPointEntityData );

					// insert the newly created data point to internal lists for further
					// referencing.
					if ( 0 != dataPointEntity )
					{
						// update internal list
						{
							// stop multi threads accessing this list
							TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

							m_internalDataPointEntityKeyMap[ dataPointEntity->getEntityKey() ] = dataPointEntity;
						}
                        
                        {
                            // stop multi threads accessing this list
                            TA_Base_Core::ThreadGuard guard( m_internalDataPointAddressMapLock );
                            
                            InternalDataPointAddressMap::iterator addrItr = m_internalDataPointAddressMap.find(dataPointEntity->getDataPointAddress());
                            
                            if ( addrItr != m_internalDataPointAddressMap.end() )
                            {
                                addrItr->second.push_back(dataPointEntity);
                            }
                            else
                            {
                                InternalDataPointVector dataPointVector;
                                dataPointVector.push_back(dataPointEntity);
                                
                                m_internalDataPointAddressMap[ dataPointEntity->getDataPointAddress() ] = dataPointVector;
                            }
                        }
                        
						// set operation mode
						if ( true == m_inControlMode )
						{
							dataPointEntity->setToControlMode();
						}
						else
						{
							dataPointEntity->setToMonitorMode();
						}
					}
					// if fail to create the DataPoint entity, do nothing
				}
				// else do nothing if already created
			}
			// else do nothing if the db entity is not a DataPoint
		}
		catch ( ... )
		{
			// do nothing
		}

		return dataPointEntity;

		FUNCTION_EXIT;
	}

	/**
	* Creates a DataPoint entity and stores it internally for further reference.  Returns a 
	* non empty pointer to IEntity if the operation is succesful, or an empty pointer to IEntity
	* if the operation is failed.
	*
	* @param    entityData    The basic configuration data from the database
	*
	*/
	TA_Base_Bus::IEntity* CtaScada::addDatapointWithPersistedFieldValue(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("addDatapointWithPersistedFieldValue");

		TA_Base_Bus::DataPoint * dataPointEntity = 0;

		try
		{
			// cast the entityData to DataPointEntityData
            TA_Base_Core::DataPointEntityDataPtr dataPointEntityData =
                boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(entityData);
			
			// if the cast is successful, i.e the db entity is a DataPoint
			if ( dataPointEntityData.get() != NULL )
			{
				std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;

				// find if we already have the specified DataPoint entity created
				{
					// stop multi threads accessing this list
					TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

					itr = m_internalDataPointEntityKeyMap.find ( dataPointEntityData->getKey() );
				}

				// if does not exist yet
				if ( m_internalDataPointEntityKeyMap.end() == itr )
				{
					// create a DataPoint based on the specified entity data
					dataPointEntity = m_dataPointFactory->createDataPoint ( dataPointEntityData );

					// insert the newly created data point to internal lists for further
					// referencing.
					if ( 0 != dataPointEntity )
					{
						// update internal list
						{
							// stop multi threads accessing this list
							TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

							m_internalDataPointEntityKeyMap[ dataPointEntity->getEntityKey() ] = dataPointEntity;
						}
                        
                        {
                            // stop multi threads accessing this list
                            TA_Base_Core::ThreadGuard guard( m_internalDataPointAddressMapLock );
                            
                            InternalDataPointAddressMap::iterator addrItr = m_internalDataPointAddressMap.find(dataPointEntity->getDataPointAddress());
                            
                            if ( addrItr != m_internalDataPointAddressMap.end() )
                            {
                                addrItr->second.push_back(dataPointEntity);
                            }
                            else
                            {
                                InternalDataPointVector dataPointVector;
                                dataPointVector.push_back(dataPointEntity);
                                
                                m_internalDataPointAddressMap[ dataPointEntity->getDataPointAddress() ] = dataPointVector;
                            }
                        }
                        
						// set operation mode
						if ( true == m_inControlMode )
						{
							dataPointEntity->setToControlMode();
						}
						else
						{
							dataPointEntity->setToMonitorMode();
						}
                        dataPointEntity->setPersistedValueFromDB();
					}
					// if fail to create the DataPoint entity, do nothing
				}
				// else do nothing if already created
			}
			// else do nothing if the db entity is not a DataPoint
		}
		catch ( ... )
		{
			// do nothing
		}

		return dataPointEntity;

		FUNCTION_EXIT;
	}


	/**
	* Remove the reference to the DataPoint whose pkey matched with the specified entity key.
	*
	* @param    entityKey    The pkey of the DataPoint entity to be removed.
	*
	*/
	void CtaScada::removeDataPoint(unsigned long entityKey)
	{
		FUNCTION_ENTRY("removeDataPoint");

		try
		{
            TA_Base_Bus::DataPoint* dataPoint = NULL;
            
            {
                // stop multi threads accessing this list
			    TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );
		    
			    // check if we have any DataPoint whose pkey matched the specified entity key
			    std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr = m_internalDataPointEntityKeyMap.find ( entityKey );

			    // if found one
			    if ( m_internalDataPointEntityKeyMap.end() != itr )
			    {
                    dataPoint = itr->second;

                    // remove the reference
				    m_internalDataPointEntityKeyMap.erase ( itr );
			    }
            }
            
            if ( dataPoint != NULL )
            {
                // stop multi threads accessing this list
                TA_Base_Core::ThreadGuard guard( m_internalDataPointAddressMapLock );
                
                // check if we have any DataPoint whose pkey matched the specified entity key
                InternalDataPointAddressMap::iterator itr = m_internalDataPointAddressMap.find ( dataPoint->getDataPointAddress() );
                
                // if found one
                if ( m_internalDataPointAddressMap.end() != itr )
                {
                    InternalDataPointVector::iterator dpvItr = itr->second.begin();
                    
                    while ( itr->second.end() != dpvItr )
                    {
                        if ( entityKey != (*dpvItr)->getEntityKey() )
                        {
                            dpvItr++;
                        }
                        else
                        {
                            dpvItr = itr->second.erase(dpvItr);
                        }
                    }
                    
                    if ( true == itr->second.empty() )
                    {
                        m_internalDataPointAddressMap.erase (itr);
                    }
                }
            }
        }
		catch ( ... )
		{
		}

		FUNCTION_EXIT;
	}


	/**
	* Set all DataPoint entities created via the addDataPoint operation to Control or Monitor mode.
	*
	* @param    wantControl    A boolean parameter indicating if all DataPoint entities created
	*						   by this class instance shall be set to Control mode
	*						   (if the parameter is TRUE) or Monitor mode (if the parameter is FALSE)
	*
	*/
	void CtaScada::controlDatapoints(bool wantControl)
	{
		FUNCTION_ENTRY("controlDatapoints");
        m_inControlMode = wantControl;

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

		std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;
		for ( itr = m_internalDataPointEntityKeyMap.begin();
			  itr != m_internalDataPointEntityKeyMap.end();
			  itr++ )
		{
			try
			{
				if ( true == m_inControlMode )
				{
					(*itr).second->setToControlMode();
				}
				else
				{
					(*itr).second->setToMonitorMode();
				}
			}
			catch ( ... )
			{
			}
		}
        if ( NULL != m_bulkPolledInterface )
        {
	        // stop receiving remote requests
            if (m_inControlMode)
            {
	            // start receiving remote requests
	            m_bulkPolledInterface->setToControl();
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "bulk polling interface has been set to control mode.");
            }
            else
            {
	            m_bulkPolledInterface->setToMonitor();
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "bulk polling interface has been set to monitor mode.");
            }
        }

		if(NULL != m_dataPointAccessInterface)
		{
			// stop receiving remote requests
            if (m_inControlMode)
            {
				// start receiving remote requests
				m_dataPointAccessInterface->setToControl();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "datapoint access interface has been set to control mode.");
            }
            else
            {
				m_dataPointAccessInterface->setToMonitor();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "datapoint access interface has been set to monitor mode.");
            }
		}
        FUNCTION_EXIT;
	}


//	/**
//	* Deactive the Corba Servant of all DataPoint entities created via the addDataPoint operation.
//	*/
//	void CtaScada::deactivateDatapoints()
//	{
//		FUNCTION_ENTRY("deactivateDatapoints");
//
//		// stop multi threads accessing this list
//		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );
//
//		// for each of the DataPoint entities in the list
//		std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;
//		for ( itr = m_internalDataPointEntityKeyMap.begin();
//			  itr != m_internalDataPointEntityKeyMap.end();
//			  itr++ )
//		{
//			// deactivate the entity
//			try
//			{
//				(*itr).second->deactivateServant();
//			}
//			catch ( ... )
//			{
//				// do nothing
//			}
//		}
//
//		FUNCTION_EXIT;
//	}


	/**
	* Find a reference to the DataPoint entity whose name matched with the specified entity name.
	* Returns a non empty pointer to the DataPoint entity if the operation is successful or an 
	* empty pointer if the operation is failed.
	*
	* @param    entityName    The entity name to be used as the search criteria.
	*
	*/
	TA_Base_Bus::DataPoint * CtaScada::findDataPointWithEntityName ( const std::string & entityName )
	{
		FUNCTION_ENTRY("findDataPointWithEntityName");

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

		TA_Base_Bus::DataPoint * dataPointEntity = 0;

		// for each of the DataPoint in the list
		std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;
		for ( itr = m_internalDataPointEntityKeyMap.begin();
			  itr != m_internalDataPointEntityKeyMap.end();
			  itr++ )
		{
			// if the name matched with the specified entity name
			if ( entityName == (*itr).second->getDataPointName() )
			{
				// get the reference to the DataPoint
				dataPointEntity = (*itr).second;

				// found the first entity that matches--> no need to go any further
				break;
			}
		}

		return dataPointEntity;

		FUNCTION_EXIT;
	}


	/**
	* Find a reference to the DataPoint entity whose address matched with the specified entity
	* address.  Returns a non empty pointer to the DataPoint entity if the operation is successful
	* or an empty pointer if the operation is failed.
	*
	* @param    entityAddress    The entity address to be used as the search criteria.
	*
	*/
	TA_Base_Bus::DataPoint * CtaScada::findDataPointWithEntityAddress ( const std::string & entityAddress )
	{
		FUNCTION_ENTRY("findDataPointWithEntityAddress");

        std::string targetEntityAddress(entityAddress);
        std::transform(entityAddress.begin(), entityAddress.end(),
            targetEntityAddress.begin(), toupper);

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );

		TA_Base_Bus::DataPoint * dataPointEntity = 0;

		// for each of the DataPoint in the list
		std::map < unsigned long, TA_Base_Bus::DataPoint * >::iterator itr;
		for ( itr = m_internalDataPointEntityKeyMap.begin();
			  itr != m_internalDataPointEntityKeyMap.end();
			  itr++ )
		{
			// if the address matched with the specified entity address
			if ( targetEntityAddress == (*itr).second->getDataPointAddress() )
			{
				// get the reference to the DataPoint
				dataPointEntity = (*itr).second;

				// found the first entity that matches--> no need to go any further
				break;
			}
		}

		return dataPointEntity;

		FUNCTION_EXIT;
	}


	/**
	* Find the references to all DataPoint entity whose addresses matched with the specified entity
	* address.
	*
	* @param    listOfDataPointsWithSameAddress		The list of DataPoints that have the same address as the specified
	* @param    entityAddress						The entity address to be used as the search criteria.
	* @param    substring							Indication if the specified address is a whole or part of the entity address
	*
	*/
	void CtaScada::findAllDataPointWithEntityAddress ( const std::string & entityAddress, InternalDataPointVector & listOfDataPointsWithSameAddress, bool substring /*= false*/ )
	{
		FUNCTION_ENTRY("findAllDataPointWithEntityAddress");

        std::string targetEntityAddress(entityAddress);
        std::transform(entityAddress.begin(), entityAddress.end(),
            targetEntityAddress.begin(), toupper);

        // stop multi threads accessing this list
        TA_Base_Core::ThreadGuard guard( m_internalDataPointAddressMapLock );
        
        
        // if it is indicated that the specified address is part of the whole entity address
        if ( true == substring )
        {
            // for each of the DataPoint in the list
            InternalDataPointAddressMap::iterator itr;
            for ( itr = m_internalDataPointAddressMap.begin();
            itr != m_internalDataPointAddressMap.end();
            itr++ )
            {
                // check if the current DataPoint's address has the specified address
                std::string::size_type pos = std::string::npos;
                pos = itr->first.find ( targetEntityAddress );
                
                // if there is a match
                if ( std::string::npos != pos )
                {
                    // get the reference to the DataPoint
                    listOfDataPointsWithSameAddress.insert( listOfDataPointsWithSameAddress.end(), (*itr).second.begin(), (*itr).second.end() );
                }
                // else do nothing
            }
        }
        // if it is indicated that the specified address is the whole of the entity address
        else
        {
            // if the address matched with the specified entity address
            InternalDataPointAddressMap::iterator itr = m_internalDataPointAddressMap.find(targetEntityAddress);
            
            if ( itr != m_internalDataPointAddressMap.end())
            {
                // get the reference to the DataPoint
                listOfDataPointsWithSameAddress = itr->second;
            }
            // else do nothing
        }
        
		FUNCTION_EXIT;
	}


	/**
	* Get a reference to the DataPoint Proxy whose name matched with the specified entity
	* name.  Returns a non empty pointer to the DataPoint Proxy if the operation is successful
	* or an empty pointer if the operation is failed.
	*
	* @param    entityName    The entity name to be used as the search criteria.
	*
	*/
	TA_Base_Bus::DataPointProxySmartPtr * CtaScada::getDataPointProxyWithEntityName ( const std::string & entityName )
	{
		FUNCTION_ENTRY("getDataPointProxyWithEntityName");

		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_externalDataPointEntityNameMapLock );

		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;

		try
		{
			// check if we already have a proxy matching the specified entity name
			std::map < std::string, TA_Base_Bus::DataPointProxySmartPtr * >::iterator itr =	m_externalDataPointEntityNameMap.find ( entityName );

			// if find one
			if ( m_externalDataPointEntityNameMap.end() != itr )
			{
				// get the reference to the proxy
				dataPointProxy = (*itr).second;
			}
			// if find none
			else
			{
				// get the entity
				TA_Base_Core::IEntityData* entity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);

				TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entity);
				
				if(NULL == dpEntityData)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "IEntityData -> DataPointEntityData, cast failure.");
                    delete entity;
                    entity = NULL;
				}
				else
				{
					TA_Base_Core::DataPointEntityDataPtr dpEntityDataSmartPtr(dpEntityData);
					// create the new proxy
					DataPointProxySmartPtr *dataPointProxy = new DataPointProxySmartPtr();
					TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy ( dpEntityDataSmartPtr, *this, *dataPointProxy );
					
					// save the proxy for further reference
					m_externalDataPointEntityNameMap [ entityName ] = dataPointProxy;
				}
			}
		}
		catch ( TA_Base_Core::ScadaProxyException & )
		{
			// do nothing
		}
		catch ( ... )
		{
			// do nothing
		}

		return dataPointProxy;

		FUNCTION_EXIT;
	}


	TA_Base_Bus::DataPointProxySmartPtr * CtaScada::getDataPointProxyWithEntityData ( TA_Base_Core::IEntityDataPtr entity )
	{
		FUNCTION_ENTRY("getDataPointProxyWithEntityData");
		
		// stop multi threads accessing this list
		TA_Base_Core::ThreadGuard guard( m_externalDataPointEntityNameMapLock );
		
		TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
		
		try
		{
			std::string entityName = entity->getName();
			// check if we already have a proxy matching the specified entity name
			std::map < std::string, TA_Base_Bus::DataPointProxySmartPtr * >::iterator itr =	m_externalDataPointEntityNameMap.find ( entityName );
			
			// if find one
			if ( m_externalDataPointEntityNameMap.end() != itr )
			{
				// get the reference to the proxy
				dataPointProxy = (*itr).second;
			}
			// if find none
			else
			{				
				TA_Base_Core::DataPointEntityDataPtr dpEntityData =
                    boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(entity);
				if(NULL == dpEntityData.get())
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "IEntityData -> DataPointEntityData, cast failour.");
				}
				else
				{
					// create the new proxy
					DataPointProxySmartPtr *dataPointProxy = new DataPointProxySmartPtr();
					TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy ( dpEntityData, *this, *dataPointProxy );
					
					// save the proxy for further reference
					m_externalDataPointEntityNameMap [ entityName ] = dataPointProxy;
				}
			}
		}
		catch ( TA_Base_Core::ScadaProxyException & )
		{
			// do nothing
		}
		catch ( ... )
		{
			// do nothing
		}
		
		return dataPointProxy;
		
		FUNCTION_EXIT;
	}


	/**
	* Convert the specified value as a string to TA_Base_Bus::DpValue for the specified DataPoint
	*
	* @param    dataPoint   The data point to be used in the conversion process
	* @param    value		The value as a string to be converted to TA_Base_Bus::DpValue
	*
	*/
	TA_Base_Bus::DpValue CtaScada::stringToValue(TA_Base_Bus::DataPoint & dataPoint, const std::string & value)
	{
		TA_Base_Bus::DpValue returnValue(dataPoint.getDataPointDataType(), dataPoint.getEnumLabelsMap(), dataPoint.getBooleanLabels());

		switch ( dataPoint.getDataPointDataType() )
		{
			case TA_Base_Bus::DPT_INT16_TYPE :
			case TA_Base_Bus::DPT_INT32_TYPE :
			case TA_Base_Bus::DPT_INT16S_TYPE :
			case TA_Base_Bus::DPT_INT32S_TYPE :
			case TA_Base_Bus::DPT_UINT16_TYPE :
			case TA_Base_Bus::DPT_UINT32_TYPE :
			case TA_Base_Bus::DPT_IEEE32_TYPE :
				{
					std::istringstream floatStream ( value );
					double floatValue = 0;

					if ( false == (floatStream >> floatValue && floatStream.eof()) )
					{
						std::string text = "Error: " + value + " is not a valid value for \"" + dataPoint.getDataPointName() + "\".";
						LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "stringToValue() - %s", text.c_str() );

						std::ostringstream message;
						message << "String: {" << value << "} cannot be converted to an integer";
						TA_THROW (TA_Base_Core::CtaScadaInvalidConfigurationException(message.str().c_str()));
					}

					returnValue.setFloat ( floatValue, dataPoint.getPrecision() );	
				}
				break;
			case TA_Base_Bus::DPT_ENUM_TYPE :
				{
						unsigned long enumValue;

						std::stringstream convertValue;
						convertValue << value;	
						convertValue >> enumValue;

						returnValue.setEnum( enumValue);
				}
				break;
			case TA_Base_Bus::DPT_BOOL_TYPE :
				{
					if ( "true" == value || "True" == value || "TRUE" == value || "1" == value )
					{
						// true boolean value
						returnValue.setBoolean ( true );
					}
					else if ( "false" == value ||  "False" == value || "FALSE" == value || "0" == value )
					{
						returnValue.setBoolean ( false );
					}
				}
				break;
			case TA_Base_Bus::DPT_TEXT_TYPE :
				{
					returnValue.setText ( value );
				}
			default:
				break;
		}

		return returnValue;
	}


	void CtaScada::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");

		// check the update type
		switch ( updateType )
		{
			// only interested in value/quality
			case ConfigAvailable:
			case ConfigStateUpdate:
			case ValueStateUpdate:
            case AlarmSummaryUpdate://limin++, TD20740
            case AckSummaryUpdate:  //limin++, TD20740
            case QualitySummaryUpdate:
			{
				try
				{
					// get the entity associated with the specified entity key from database
					TA_Base_Core::IEntityData * dbEntityData = 0;
					dbEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity ( entityKey );

					// if able to get the database entity
					if ( 0 != dbEntityData )
					{
						// get the name associated with the entity key
						std::string entityName = dbEntityData->getName();
						
						// get the proxy from the list
						TA_Base_Bus::DataPointProxySmartPtr * dataPointProxy = 0;
						dataPointProxy = getDataPointProxyWithEntityName ( entityName );

						// only proceed if we found a proxy associated with the update event
						if ( 0 != dataPointProxy )
						{
							std::vector < ItaScadaExternalDataPointUpdateObserver * > * listOfProxyObservers = 0;

							{
								// stop multi threads accessing this list
								TA_Base_Core::ThreadGuard guard( m_proxyNameUpdateHandlerMapLock );

								// check if there are observers registered against the specified proxy
								std::map < std::string, std::vector < ItaScadaExternalDataPointUpdateObserver * > * >::iterator itr = m_proxyNameUpdateObserverMap.find ( entityName );

								// if there are observers to be called
								if ( m_proxyNameUpdateObserverMap.end() != itr )
								{
									// get the list of the observers associated with the specified proxy
									listOfProxyObservers = (*itr).second;
								}
								// else do nothing
							}

							// for each of the observers in the list
							std::vector < ItaScadaExternalDataPointUpdateObserver * >::iterator observerItr;
							for ( observerItr = listOfProxyObservers->begin();
								  observerItr != listOfProxyObservers->end();
								  observerItr++ )
							{
								// callback with the updated value/quality
								(*observerItr)->onExternalDataPointUpdate ( 
									(*dataPointProxy)->getEntityName(), 
									(*dataPointProxy)->getValue(), 
									( ( TA_Base_Core::ITA_QUALITY_GOOD > (*dataPointProxy)->getStatus() ) ? TA_Base_Core::ITA_QUALITY_BAD : TA_Base_Core::ITA_QUALITY_GOOD ) );
							}
						}
						// else do nothing
					}

					delete dbEntityData;
					dbEntityData = 0;			
				}
				catch ( ... )
				{
					// do nothing
				}

				break;
			}

			// do nothing for all other types
			default:
				break;
		}

		FUNCTION_EXIT;
	}

    TA_Base_Bus::DataPoint * CtaScada::getDataPoint ( unsigned long entityKey )
    {
	    try
	    {
            TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );
            std::map <unsigned long, TA_Base_Bus::DataPoint*>::iterator  it = m_internalDataPointEntityKeyMap.find(entityKey);
            if ( it != m_internalDataPointEntityKeyMap.end() )
            {
                return (*it).second;
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                    "Somewhere is trying to access non-existing datapoint with entity key [%u].", entityKey);
            }
	    }
	    catch (...)
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getDataPoint() - Exception thrown while retrieving datapoint");
	    }
        //if can not find the particular datapoint with the entity key, return NULL
	    return NULL;
    }

    void CtaScada::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
    {
		FUNCTION_ENTRY("getAllLocalDataPoints");

		TA_Base_Core::ThreadGuard guard( m_internalDataPointEntityKeyMapLock );
		FUNCTION_EXIT;
        dataPointList = m_internalDataPointEntityKeyMap;
    }

    
    TA_Base_Bus::DataPointFactory* CtaScada::getDataPointFactory()
    {
        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        return m_dataPointFactory;
    }
}
