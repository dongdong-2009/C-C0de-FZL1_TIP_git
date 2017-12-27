/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfigDerivedDataPoint.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * This class provides read-write access to all attributes of a derived datapoint.
  */

#ifndef CONFIG_DERIVED_DATA_POINT_H
#define CONFIG_DERIVED_DATA_POINT_H

#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"

namespace TA_Base_Core
{

    class ConfigDerivedDataPoint : public IConfigDerivedDataPoint
	{
    public:


        /**
         * Constructor
         *
         * @param unsigned long- The key of the entity that this derived datapoint represents
         */
        ConfigDerivedDataPoint(unsigned long pkey,bool isWritable);


        /**
         * Constructor
         *
         * Constructs a new derived datapoint
         */
        ConfigDerivedDataPoint();


        /**
         * Copy Constructor
         */
        ConfigDerivedDataPoint(const ConfigDerivedDataPoint&);


        /**
         * Destructor
         */
        virtual ~ConfigDerivedDataPoint();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        bool hasChanged()
        {
            // If the changes list is empty then nothing has changed
            return !m_changes.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        bool isNew()
        {
            return m_isNew;
        }

        
        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        unsigned long getKey()
        {
            return m_key;
        }


        /**
         * setKey
         *
         * When a new derived datapoint is created it does not have a derived datapoint pkey until
         * that datapoint has been added to the database. So this allows the key to be set later
         *
         * @param unsigned long - The primary key of the datapoint
         */
        void setKey(unsigned long key)
        {
            m_key = key;
        }


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        void applyChanges();

	
        /**
         * deleteThisDataPoint
         *
         * This will delete all data related to this derived data point from the database
         *
         * @exception DatabaseException
         */
        void deleteThisDataPoint();

        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();


        /**
         * getInputAssociations
         *
         * This retrieves all input associations related to this derived datapoint.
         *
         * @return const InputAssociationMap& - A reference to the internally stored list of input associations
         *
         * @exception DataException
         * @exception DatabaseException
         */
        const InputAssociationMap& getInputAssociations();


        /**
         * getOutputAssociations
         *
         * This retrieves all output associations related to this derived datapoint.
         *
         * @return const OutputAssociationMap& - A reference to the internally stored list of output associations
         *
         * @exception DataException
         * @exception DatabaseException
         */
        const OutputAssociationMap& getOutputAssociations();


        /**
         * getDerivedStates
         *
         * This retrieves all derived states related to this derived datapoint.
         *
         * @return const StateMap& - A reference to the internally stored list of states
         *
         * @exception DataException
         * @exception DatabaseException
         */
        const StateMap& getDerivedStates();


        /**
         * setNumberOfStates
         *
         * This method is NOT used for 'Derived' datapoints. It is used for combined output and safety
         * output datapoints. These datapoints have a number of states based on what the user enters.
         * Therefore we must have a method to manually set the number of states.
         *
         * @param int - The number of states to create
         */
        void setNumberOfStates(int numberOfStates);


        /**
         * addInputAssociation
         *
         * This will add a new input association for the entity key specified. It will create the
         * object and add it to the list of input associations. This will also add new states
         * as a new input increased the number of derived states.
         *
         * @param unsigned long - The entity key for the entity that is the input association
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void addInputAssociation(unsigned long entityKey);


        /**
         * removeInputAssociation
         *
         * This will remove an input association for the entity key specified. It will delete the
         * object and remove it from the list of input associations. This will also remove any
         * derived states and association outputs that are no longer possible. Reducing the number
         * of inputs reduces the number of possible states.
         *
         * @param unsigned long - The entity key for the entity that is the input association
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void removeInputAssociation(unsigned long entityKey);


        /**
         * addOutputAssociation
         *
         * This will add a new output association for the entity key specified. It will create the
         * object and add it to the list of output associations.
         *
         * @param unsigned long - The entity key for the entity that is the output association
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void addOutputAssociation(int state, unsigned long entityKey);


        /**
         * removeOutputAssociation
         *
         * This will remove an output association for the entity key specified. It will delete the
         * object and remove it from the list of output associations.
         *
         * @param unsigned long - The entity key for the entity that is the output association
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void removeOutputAssociation(int state, unsigned long entityKey);


        /**
         * updateChanges
         *
         * This updates the map recording the changes to the duty. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);


        /**
         * removeChanges
         *
         * This will update the map of recorded changes to remove any changes made to the name specified
         * When a change is added and then later removed there is no need to record all the changes made
         * to it. This method allows the changes to be removed.
         *
         * @param string - The name to be removed
         */
        void removeChanges(const std::string& name);


    private:

        /**
         * reload
         *
         * Reloads all data fresh from the database
         */
        void reload();


        static const std::string INCLUDED;
        static const std::string NOT_INCLUDED;


        // This is used for retriving output association details
        struct StateDetails
        {
            int stateValue;             // The value of the state ie 0, 1, 2 etc
            unsigned long stateKey;     // The pkey in the database of the derived state
        };


        bool m_isValidData;                 // Indicates if the data we have is current or whether it
                                            // should be reloaded from the database on the next request
        bool m_isNew;                       // Indicates if this is a new derived datapoint
        unsigned long m_key;                // The entity key of this derived datapoint
        StateMap m_derivedStates;           // The derived states
        InputAssociationMap m_inputAssociations;    // The input associations
        OutputAssociationMap m_outputAssociations;  // The output assocations

        ItemChanges m_changes;     // This is a map of all changes made to the derived data point so that
                                   // they are recorded and can be sent in audit or online update messages.

        bool m_areStatesSetManually;  // This indicates if the number of states is determined from the number
                                      // of inputs or if the user has set the number of states manually.
        bool m_isWritable;		//jinmin++ This indicates wheather the entity is a output datapoint, if not no need to load output association 
	};
}


#endif // CONFIG_DERIVED_DATA_POINT_H

