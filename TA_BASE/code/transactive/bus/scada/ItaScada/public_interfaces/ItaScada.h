/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/ItaScada/public_interfaces/ItaScada.h $
  * @author  Sean Liew
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2015/01/19 17:43:23 $
  * Last modified by : $Author: CM $
  *
  * Implementation of the Interface ItaScada
  *
  */

#if !defined(ItaScada_C10999AE_6706_454f_957E_2D8742F8C94E__INCLUDED_)
#define ItaScada_C10999AE_6706_454f_957E_2D8742F8C94E__INCLUDED_

#include <vector>
#include "bus/scada/common_library/src/DpValue.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/types/public_types/TypeDefs.h"
#include "core/types/public_types/Constants.h"

namespace TA_Base_Bus
{
    class ItaScadaExternalDataPointUpdateObserver;
    class DataPoint;

    /**
     * Interface to allow the outsourcer to access basic SCADA datapoint
     * funcationality.
     * @created 04-Mar-2005 10:25:54 AM
     * @version 1.0
     * @updated 31-Mar-2005 01:00:49 PM
     */
    class ItaScada{

    public:
        //TD15690 Mintao
		/**
		* Find a reference to the DataPoint entity whose address matched with the specified entity
		* address.  Returns a non empty pointer to the DataPoint entity if the operation is successful
		* or an empty pointer if the operation is failed.
		*
		* @param    entityAddress    The entity address to be used as the search criteria.
		*
		*/
		virtual TA_Base_Bus::DataPoint * findDataPointWithEntityAddress ( const std::string & entityAddress ) = 0;

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
        virtual void getLocalDataPointStateByUniqueAddress(const std::string& address, std::string& returnedValue,TA_Base_Core::EDataPointQuality& returnedQuality) = 0;
        

	    /**
	      * * > getAllLocalDataPoints
	      * 
	      * return all the local datapoints maintained by the ctascada, it is caller's responsibility to delete them
	      * 
	      * @return std::vector <TA_Base_Bus::DataPoint 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::map < unsigned long, TA_Base_Bus::DataPoint * > getAllLocalDataPoints() = 0;
        //Mintao++
        //TD13910

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
	    virtual void setExternalDataPointState(const std::string& name, const std::string& value, const std::string& sessionId = "") =0;
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
            virtual void getExternalDataPointState(const std::string& name, std::string& returnedValue, TA_Base_Core::EDataPointQuality& returnedQuality) =0;
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
	    virtual void setLocalDataPointStateByName(const std::string& name, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) =0;
		virtual void setLocalDataPointStateByName(const std::string& name, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
		virtual void setLocalDataPointStateByName(const std::string& name, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
		virtual void setLocalDataPointStateByName(const std::string& name, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
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
	    virtual void setLocalDataPointStateByAddress(const std::string& address, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false) =0;
		virtual void setLocalDataPointStateByAddress(const std::string& address, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false) = 0;
		virtual void setLocalDataPointStateByAddress(const std::string& address, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false) = 0;
		virtual void setLocalDataPointStateByAddress(const std::string& address, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp, bool substr = false) = 0;
	    /**
	     * Update the quality flag of all local datapoints owned by this application.
	     * 
	     * Throws CtaScadaWriteException if the write operation fails.
	     * @param quality    quality
	     * 
	     */
	    virtual void setAllLocalDataPointQualities(TA_Base_Core::EDataPointQuality quality) =0;
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
		virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const std::string& value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
	    virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const double value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
	    virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const unsigned long value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
	    virtual void setAllLocalDataPointStateMatchingAddress(const std::string& addressSubString, const bool value, TA_Base_Core::EDataPointQuality quality, time_t timestamp) = 0;
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
	    virtual void setAllLocalDataPointQualitiesMatchingAddress(const std::string& addressSubString, TA_Base_Core::EDataPointQuality quality, bool substr = true) =0;
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
	    virtual TA_Base_Bus::DpValue registerForExternalDataPointUpdateAndGetState(const std::string& name, ItaScadaExternalDataPointUpdateObserver* observer, TA_Base_Core::EDataPointQuality& returnedQuality) =0;


		/**
		  * registerForExternalDataPointUpdateAndGetState
		  * 
		  * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
		  * Note: no need to delete the entity, the Proxy will do it.
		  * 
		  * @return virtual TA_Base_Bus::DpValue 
		  * @param : const TA_Base_Core::IEntityData* entity
		  * @param : ItaScadaExternalDataPointUpdateObserver* observer
		  * @param : TA_Base_Core::EDataPointQuality& returnedQuality
		  * 
		  * @exception <exceptions> Optional
		  */
	    virtual TA_Base_Bus::DpValue registerForExternalDataPointUpdateAndGetState(TA_Base_Core::IEntityDataPtr entity, ItaScadaExternalDataPointUpdateObserver* observer, TA_Base_Core::EDataPointQuality& returnedQuality) =0;
		
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
		virtual void registerForExternalDataPointUpdate(const std::string& name, ItaScadaExternalDataPointUpdateObserver* observer)=0;
		
		/**
		  * registerForExternalDataPointUpdate
		  * 
		  * Register an observer for the notification of updates to external datapoints.
		  * Note: no need to delete the entity, the Proxy will do it.
		  * 
		  * @return virtual void 
		  * @param : const TA_Base_Core::IEntityData* entity
		  * @param : ItaScadaExternalDataPointUpdateObserver* observer
		  * 
		  * @exception <exceptions> Optional
		  * Throws CtaScadaInvalidConfigurationException if the datapoint is not locatable.
		  */
		virtual void registerForExternalDataPointUpdate(TA_Base_Core::IEntityDataPtr entity, ItaScadaExternalDataPointUpdateObserver* observer)=0;
    };
}
#endif // !defined(ItaScada_C10999AE_6706_454f_957E_2D8742F8C94E__INCLUDED_)