/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h $
  * @author Karen Graham
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * This interface provides read-write access to all attributes of a derived datapoint.
  */

#ifndef I_CONFIG_DERIVED_DATA_POINT_H
#define I_CONFIG_DERIVED_DATA_POINT_H

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class ConfigConfiguredDerivedState;
    class ConfigConfiguredInputAssociation;
    class ConfigDerivedOutputAssociation;

    class IConfigDerivedDataPoint
	{

    public:
		virtual ~IConfigDerivedDataPoint(){};
        // The Derived states keyed by what state each object represents
        typedef std::map<int, ConfigConfiguredDerivedState*> StateMap;

        // The input associations keyed by their entity key
        typedef std::map<unsigned long, ConfigConfiguredInputAssociation*> InputAssociationMap;

        // The output association keyed by their state. Can have more than one output association
        // per state.
        typedef std::multimap<int, ConfigDerivedOutputAssociation*> OutputAssociationMap;

        
        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges() =0;

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged() =0;


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew() =0;

        
        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey() =0;


        /**
         * setKey
         *
         * When a new derived datapoint is created it does not have a derived datapoint pkey until
         * that datapoint has been added to the database. So this allows the key to be set later
         *
         * @param unsigned long - The primary key of the datapoint
         */
        virtual void setKey(unsigned long key) =0;


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
        virtual void applyChanges() =0;


        /**
         * deleteThisDataPoint
         *
         * This will delete all data related to this derived data point from the database
         *
         * @exception DatabaseException
         */
        virtual void deleteThisDataPoint() =0;


        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;
       

        /**
         * setNumberOfStates
         *
         * This method is NOT used for 'Derived' datapoints. It is used for combined output and safety
         * output datapoints. These datapoints have a number of states based on what the user enters.
         * Therefore we must have a method to manually set the number of states.
         *
         * @param int - The number of states to create
         */
        virtual void setNumberOfStates(int numberOfStates) =0;

        
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
        virtual const InputAssociationMap& getInputAssociations() =0;


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
        virtual const OutputAssociationMap& getOutputAssociations() =0;


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
        virtual const StateMap& getDerivedStates() =0;


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
        virtual void addInputAssociation(unsigned long entityKey) =0;


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
        virtual void removeInputAssociation(unsigned long entityKey) =0;
 

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
        virtual void addOutputAssociation(int state, unsigned long entityKey) =0;


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
        virtual void removeOutputAssociation(int state, unsigned long entityKey) =0;

	};
}


#endif // I_CONFIG_DERIVED_DATA_POINT_H

