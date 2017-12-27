/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/ConfigPlanDssDpRule.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigPlanDssDpRule is a concrete implementation of IConfigPlanDssDpRule, 
  * which is in turn an implementation
  * of IPlanDssDpRule. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PlanDssDpRules.
  *
  */


#if !defined(ConfigPlanDssDpRule_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigPlanDssDpRule_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/plans/src/IConfigPlanDssDpRule.h"

namespace TA_Base_Core
{
    class IData;
    class PlanDssDpRuleHelper;

    class ConfigPlanDssDpRule : public IConfigPlanDssDpRule
    {

    public:
        /**
         * ConfigPlanDssDpRule (constructor)
         *
         * Constructs a new instance of the ConfigPlanDssDpRule with no id. This is used when
         * creating a *new* PlanDssDpRule - that is, one that does not yet exist in the database.
         */
        ConfigPlanDssDpRule();

        /**
         * ConfigPlanDssDpRule (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PlanDssDpRuleHelper::PlanDssDpRuleHelper(IData*)
         *
         */
        ConfigPlanDssDpRule(unsigned long row, IData& data);

        /**
         * ConfigPlanDssDpRule (constructor)
         *
         * Constructs a new instance of ConfigPlanDssDpRule with the specified id. This is used 
         * when creating a ConfigPlanDssDpRule based around an existing PlanDssDpRule in the database.
         *
         * @param key The key to this PlanDssDpRule in the database.
         */
        ConfigPlanDssDpRule(const unsigned long idKey);


        /**
         * ConfigPlanDssDpRule (copy constructor)
         *
         * @param ConfigPlanDssDpRule& - The PlanDssDpRule to make a copy of.
         */
        ConfigPlanDssDpRule( const ConfigPlanDssDpRule& theConfigPlanDssDpRule);

        
        virtual ~ConfigPlanDssDpRule();
        
    public:

        /**
         * deleteThisObject
         *
         * Removes this PlanDssDpRule from the database. 
         * The calling application MUST then delete this PlanDssDpRule object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PlanDssDpRule was initially loaded from the database
         *      OR     - writePlanDssDpRuleData() has already been called
         *      This PlanDssDpRule has not been deleted
         */
        void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PlanDssDpRule object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PlanDssDpRule has not been deleted
         */
        void applyChanges();

        virtual void setLocationKey(unsigned long key);
        virtual void setTriggerEntityKey(const unsigned long ulTriggerEntityKey);
        virtual void setActivePlanKey(const unsigned long ulActivePlanKey);
        virtual void setMinimumInterval(const unsigned long ulMinimumInterval);
        virtual void setTolerantTimeForActive(const unsigned long ulTolerantTimeForActive);

        virtual void setValidStartTimeInDay(const unsigned long ulValidStartTimeInDay);
        virtual void setValidEndTimeInDay(const unsigned long ulValidEndTimeInDay);

        virtual void setValidSunday(const bool bValidSunday);
        virtual void setValidMonday(const bool bValidMonday);
        virtual void setValidTuesday(const bool bValidTuesday);
        virtual void setValidWednesday(const bool bValidWednesday);
        virtual void setValidThursday(const bool bValidThursday);
        virtual void setValidFriday(const bool bValidFriday);
        virtual void setValidSaturday(const bool bValidSaturday);

        virtual void setActivateValue(const bool bActivateValue);
        virtual void setNeedDeactivate(const bool bNeedDeactivate);
        virtual void setAllowMultiple(const bool bAllowMultiple);
        virtual void setEnable(const bool bEnable);
        virtual void setDenyMultipleStrategy(const int nDenyMultipleStrategy);

        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual unsigned long getTriggerEntityKey();
        virtual unsigned long getActivePlanKey();
        virtual unsigned long getMinimumInterval();
        virtual unsigned long getTolerantTimeForActive();
        virtual unsigned long getValidStartTimeInDay();
        virtual unsigned long getValidEndTimeInDay();
        virtual bool getValidSunday();
        virtual bool getValidMonday();
        virtual bool getValidTuesday();
        virtual bool getValidWednesday();
        virtual bool getValidThursday();
        virtual bool getValidFriday();
        virtual bool getValidSaturday();
        virtual bool getActivateValue();
        virtual bool getNeedDeactivate();
        virtual bool getAllowMultiple();
        virtual bool getEnable();
        virtual int getDenyMultipleStrategy();

		virtual std::string getLocationName();
		virtual std::string getTriggerEntityName();
		virtual std::string getActivePlanName();

         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For Plan DSS rule this is 
         * just the key in a string format e.g. "Plan DSS rule 1".
         *
         * @return The key description for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * setName
         *
         * Not applicable for Plan DSS rule.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the columns. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_PlanDssDpRuleChanges;
        }

       /**
         * getUniqueIdentifier
         *
         * This retrieves the unique identifier for this operator. We cannot use keys for identification
         * in the configuration editor because new operators do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this operator
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }

        /**
         * getDateCreated
         *
         * Returns the date created for this PlanDssDpRule
         *
         * @return The date created for this PlanDssDpRule as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual unsigned long getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PlanDssDpRule.
         *
         * @return The date modified for this PlanDssDpRule as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual unsigned long getDateModified();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the PlanDssDpRule has been changed by the user.
         *
         * @return bool - This will return true if the PlanDssDpRule does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PlanDssDpRule changes is empty then nothing has changed
            return !m_PlanDssDpRuleChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this PlanDssDpRule is new. A new PlanDssDpRule is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PlanDssDpRule has not been applied to the database.
         */
        virtual bool isNew();



        /**
         * invalidate
         *
         * Make the data contained by this PlanDssDpRule as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();

    private:
            
	    /**
         * updatePlanDssDpRuleChanges
         *
         * This updates the map recording the changes to the PlanDssDpRule. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePlanDssDpRuleChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePlanDssDpRuleChanges
         *
         * This updates the map recording the changes to the PlanDssDpRule. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updatePlanDssDpRuleChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment PlanDssDpRule not used so it is made private
		ConfigPlanDssDpRule& operator=(const ConfigPlanDssDpRule&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PlanDssDpRules in the
                                          // Configuration Editor because new PlanDssDpRules do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        PlanDssDpRuleHelper* m_PlanDssDpRuleHelper;


        ItemChanges m_PlanDssDpRuleChanges; // This is a map of all changes made to the PlanDssDpRule so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Base_Core

#endif // !defined(ConfigPlanDssDpRule_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
