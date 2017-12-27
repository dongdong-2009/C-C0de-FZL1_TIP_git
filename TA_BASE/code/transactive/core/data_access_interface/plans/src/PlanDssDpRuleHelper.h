/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/PlanDssDpRuleHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * PlanDssDpRuleHelper is an object that is held by PlanDssDpRule and ConfigPlanDssDpRule objects. 
  * It contains all data used by PlanDssDpRules, and manipulation
  * methods for the data. It helps avoid re-writing code for both PlanDssDpRule and ConfigPlanDssDpRule.
  *
  */

#if !defined(PlanDssDpRuleHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PlanDssDpRuleHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include <string>

namespace TA_Base_Core
{
    class IData;

    class PlanDssDpRuleHelper
    {

    public:

        /** 
         * PlanDssDpRuleHelper
         *
         * This constructor creates a new PlanDssDpRuleHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PlanDssDpRuleHelper(const unsigned long key);

        
        /** 
         * PlanDssDpRuleHelper
         *
         * Copy constructor.
         *
         * @param   thePlanDssDpRuleHelper the original PlanDssDpRuleHelper to copy.
         *       
         */
        PlanDssDpRuleHelper( const PlanDssDpRuleHelper& thePlanDssDpRuleHelper);

        /** 
         * PlanDssDpRuleHelper
         *
         * This constructor creates a new object using the input data
         *      which is representative of a row returned from SQL statement
         *
         * @param row the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *
         * @see getBasicQueryData         
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        PlanDssDpRuleHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PlanDssDpRuleHelper
         *
         * This constructor creates a new PlanDssDpRuleHelper for configuration, initialising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writePlanDssDpRuleData() is called for the first time.
         *
         */
        PlanDssDpRuleHelper();


        /** 
         * ~PlanDssDpRuleHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PlanDssDpRuleHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PlanDssDpRule.  A new PlanDssDpRule is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PlanDssDpRule
         */
        virtual bool isNew() const;


        /**
         * getKey
         *
         * Returns the primary key of this PlanDssDpRule.
         *
         * pre:    the data exists in the database
         *
         * @return the primary key 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the key should be an unsigned long)
         *         - NO_VALUE if the PlanDssDpRule record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getLocationKey
         *
         * Returns the Location Key of this PA DVA Message Version.  If this 
         * PA DVA Message Version record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PA DVA Message Version has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Location Key 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        unsigned long getLocationKey();


        /**
         * setLocationKey
         *
         * Sets the Location Key of this PA DVA Message Version. The set values are not written to the database
         * until writePlanDssDpRuleGroupData() is called.  This method should only be used by the 
         * ConfigPlanDssDpRuleGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PA DVA Message Version
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        void setLocationKey(const unsigned long locationKey);

        void setTriggerEntityKey(const unsigned long ulTriggerEntityKey);
        void setActivePlanKey(const unsigned long ulActivePlanKey);
        void setMinimumInterval(const unsigned long ulMinimumInterval);
        void setTolerantTimeForActive(const unsigned long ulTolerantTimeForActive);

        void setValidStartTimeInDay(const unsigned long ulValidStartTimeInDay);
        void setValidEndTimeInDay(const unsigned long ulValidEndTimeInDay);

        void setValidSunday(const bool bValidSunday);
        void setValidMonday(const bool bValidMonday);
        void setValidTuesday(const bool bValidTuesday);
        void setValidWednesday(const bool bValidWednesday);
        void setValidThursday(const bool bValidThursday);
        void setValidFriday(const bool bValidFriday);
        void setValidSaturday(const bool bValidSaturday);

        void setActivateValue(const bool bActivateValue);
        void setNeedDeactivate(const bool bNeedDeactivate);
        void setAllowMultiple(const bool bAllowMultiple);
        void setEnable(const bool bEnable);
        void setDenyMultipleStrategy(const int nDenyMultipleStrategy);

        unsigned long getTriggerEntityKey();
        unsigned long getActivePlanKey();
        unsigned long getMinimumInterval();
        unsigned long getTolerantTimeForActive();
        unsigned long getValidStartTimeInDay();
        unsigned long getValidEndTimeInDay();
        bool getValidSunday();
        bool getValidMonday();
        bool getValidTuesday();
        bool getValidWednesday();
        bool getValidThursday();
        bool getValidFriday();
        bool getValidSaturday();
        bool getActivateValue();
        bool getNeedDeactivate();
        bool getAllowMultiple();
        bool getEnable();
        int getDenyMultipleStrategy();

		std::string getLocationName();
		std::string getTriggerEntityName();
		std::string getActivePlanName();
       
        /**
         * getDateCreated
         *
         * Returns the date created for this PlanDssDpRule.  If this PlanDssDpRule record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PlanDssDpRule has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PlanDssDpRule as a unsigned long.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PlanDssDpRule record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        unsigned long getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PlanDssDpRule.  If this PlanDssDpRule record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PlanDssDpRule has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PlanDssDpRule as a unsigned long.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PlanDssDpRule record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        unsigned long getDateModified();


        /**
         * invalidate
         *
         * Make the data contained by this PlanDssDpRule as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePlanDssDpRuleData() has been called prior to calling
         * this method on a new PlanDssDpRule, as it does not make any sense to invalidate an
         * PlanDssDpRule that has not yet been written to the database.
         *
         * pre:    This PlanDssDpRule was initially loaded from the database
         *         or writePlanDssDpRuleData() has already been called
         */
        void invalidate();


        /**
         * writePlanDssDpRuleData
         * 
         * Write this PlanDssDpRule to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the Location Key should be an unsigned long)
         *          - For new messages, the primary key hasn't been set or is non unique
         *          - NO_VALUE if the PlanDssDpRule record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PlanDssDpRule returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PlanDssDpRule object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePlanDssDpRuleData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePlanDssDpRule
         *
         * Remove this PlanDssDpRule from the database. Once this method has been executed, the
         * PlanDssDpRuleHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PlanDssDpRule was initially loaded from the database
         *         or writePlanDssDpRuleData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException
         *
         */
        void deletePlanDssDpRule();

    protected:

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
	
    private:

        // Made private as it is not used
		PlanDssDpRuleHelper& operator=(const PlanDssDpRuleHelper &);
        
        /**
         * modifyExistingPlanDssDpRule
         *
         * This will update an existing PlanDssDpRule in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PlanDssDpRule object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         */
        void modifyExistingPlanDssDpRule(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPlanDssDpRule
         *
         * This will add a new PlanDssDpRule in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PlanDssDpRule object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         *
         */
        void addNewPlanDssDpRule(); //throw(TA_Core::TransactiveException);
      

        /**
         * getKey (base class pure virtual implementation)
         *
         */
        virtual unsigned long getKey() const;

        /**
         * reload (base class pure virtual implementation)
         *
         */
        void reload();
        void reloadNames();

        void decodeDaysInWeek( const unsigned long ulEncoded );
        unsigned long encodeDaysInWeek();

        void decodeTimeInDay( const unsigned long ulEncoded );
        unsigned long encodeTimeInDay();

    private:

        // The actual key (as returned from database - or set by user)        
        unsigned long m_ulKey;
        unsigned long m_ulLocationKey;
        unsigned long m_ulTriggerEntityKey;
        unsigned long m_ulActivePlanKey;
        unsigned long m_ulMinimumInterval;
        unsigned long m_ulTolerantTimeForActive;

        unsigned long m_ulValidStartTimeInDay;
        unsigned long m_ulValidEndTimeInDay;

        std::string m_strLocationName;
        std::string m_strTriggerEntityName;
        std::string m_strActivePlanName;

        bool m_bValidSunday;
        bool m_bValidMonday;
        bool m_bValidTuesday;
        bool m_bValidWednesday;
        bool m_bValidThursday;
        bool m_bValidFriday;
        bool m_bValidSaturday;

        bool m_bActivateValue;
        bool m_bNeedDeactivate;
        bool m_bAllowMultiple;
        bool m_bEnable;
        int m_nDenyMultipleStrategy;

        unsigned long m_dateCreated;
        unsigned long m_dateModified;
        bool m_isValidData;
        bool m_isNew;
    };
} // closes TA_Core

#endif // !defined(PlanDssDpRuleHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
