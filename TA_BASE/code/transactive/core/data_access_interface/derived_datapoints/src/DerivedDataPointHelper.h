/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointHelper.h $
  * @author HoaVu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * DerivedDataPointHelper accesses the parameters associated with a
  * Derived DataPoint entity in the database.
  *
  */


#ifndef DERIVED_DATAPOINT_HELPER_H
#define DERIVED_DATAPOINT_HELPER_H

#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointConfigCache.h"


namespace TA_Base_Core
{
    class DerivedDataPointHelper
    {
    public:

        DerivedDataPointHelper();
        virtual ~DerivedDataPointHelper();

        /**
         * getInputAssociations
         *
         * Get the list of parameters of an input entity associated with the specified
         * Derived DataPoint's primary key.  The name of the parameters are the
         * defined under the section of SC_INPUT_ASSOCIATION database table
         *
         * @param ddpPkey  The primary key of the Derived DataPoint entity
         *
         * @return A vector of InputAssociation data objects. Empty if the data cannot be accessed.
         *
         */
        InputAssociationList getInputAssociations ( unsigned long ddpPkey );


        /**
         * getDerivedStates
         *
         * Get the list of parameters of a derived state associated with the specified
         * Derived DataPoint's primary key.  The name of the parameters are the
         * defined under the section of SC_DERIVED_STATE database table
         *
         * @param ddpPkey  The primary key of the Derived DataPoint entity
         *
         * @return A vector of DerivedState data objects. Empty if the data cannot be accessed.
         *
         */
        DerivedStateList getDerivedStates ( unsigned long ddpPkey );


        /**
         * getDerivedOutputAssociations
         *
         * Get the list of parameters of an output entity associated with the specified
         * DerivedState's primary key.  The name of the parameters are the
         * defined under the section of SC_DERIVED_OUTPUT_ASSOCIATION database table
         *
         * @param derivedStatePkey  The primary key of the DerivedState
         *
         * @return A vector of OutputAssociation data objects. Empty if the data cannot be accessed.
         *
         */
        OutputAssociationList getDerivedOutputAssociations ( unsigned long derivedStatePkey );


        /**
         * deleteDerivedDataPoint
         *
         * This will delete all derived data point details for the entity key passed in
         *
         * @param unsigned long - The pkey of the datapoint entity
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void deleteDerivedDataPoint( unsigned long ddpKey);


        /**
         * addNew
         *
         * This will add a new derived data point to the database
         *
         * @param unsigned long - The derived data point entity key
         * @param InputAssociationMap& - All the input associations to insert into the database. Once each of
         *                               these is inserted their keys should be set.
         * @param StateMap& - All the derived states to insert into the database. Once each of
         *                    these is inserted their keys should be set.
         * @param OutputAssociationMap& - All the output associations to insert into the database. Once each of
         *                                these is inserted their keys should be set.
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void addNew(unsigned long ddpKey,
                    IConfigDerivedDataPoint::InputAssociationMap& inputs,
                    IConfigDerivedDataPoint::StateMap& states,
                    IConfigDerivedDataPoint::OutputAssociationMap& outputs );


        /**
         * modifyExisting
         *
         * This will update a derived data point in the database. It will delete any old entries that are no
         * longer used, update existing and add any new entries required for all three tables.
         *
         * @param unsigned long - The derived data point entity key
         * @param InputAssociationMap& - All the input associations to insert/update in the database.
         *                               For new items their keys should be set after they have been inserted.
         * @param StateMap& - All the derived states to insert/update in the database.
         *                    For new items their keys should be set after they have been inserted.
         * @param OutputAssociationMap& - All the output associations to insert/update in the database.
         *                                For new items their keys should be set after they have been inserted.
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void modifyExisting(unsigned long ddpKey,
                            IConfigDerivedDataPoint::InputAssociationMap& inputs,
                            IConfigDerivedDataPoint::StateMap& states,
                            IConfigDerivedDataPoint::OutputAssociationMap& outputs );

    private:

        DerivedDataPointHelper ( const DerivedDataPointHelper & obj ){};
        DerivedDataPointHelper & operator= ( const DerivedDataPointHelper & );


        /**
         * modifyInputs
         *
         * This method performs the following on the database:
         *   Select all inputs where the key is not one we have stored and then delete them
         *   Update all inputs that have keys
         *   Add all inputs that are new and set their keys
         *
         * @param unsigned long - The derived data point pkey
         * @param InputAssociationMap& - The list of inputs that will need to all be in the database once
         *                               this method has finished
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void modifyInputs(unsigned long ddpKey, IConfigDerivedDataPoint::InputAssociationMap& inputs);


        /**
         * removeOutputs
         *
         * This method performs the following on the database:
         *   Select all outputs where the key is not one we have stored and then delete them
         *
         * @param unsigned long - The derived data point pkey
         * @param OutputAssociationMap& - The list of outputs that will need to all be in the database once
         *                               this method has finished. We use this to determine which outputs
         *                               no longer need to be in the database
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void removeOutputs(unsigned long ddpKey, IConfigDerivedDataPoint::OutputAssociationMap& outputs);


        /**
         * modifyStates
         *
         * This method performs the following on the database:
         *   Select all states where the key is not one we have stored and then delete them
         *   Update all states that have keys
         *   Add all states that are new and set their keys
         *
         * @param unsigned long - The derived data point pkey
         * @param StateMap& - The list of states that will need to all be in the database once
         *                    this method has finished
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void modifyStates(unsigned long ddpKey, IConfigDerivedDataPoint::StateMap& states);


        /**
         * modifyOutputs
         *
         * This method performs the following on the database:
         *   Add all the outputs that are new and set their keys
         *   Update all outputs that have keys
         *
         * @param unsigned long - The derived data point pkey
         * @param OutputAssociationMap& - The list of outputs that will need to all be in the database once
         *                                this method has finished
         * @param StateMap& - This list of states is used to retrieve keys for inserting the output objects.
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void modifyOutputs(unsigned long ddpKey,
                           IConfigDerivedDataPoint::OutputAssociationMap& outputs,
                           IConfigDerivedDataPoint::StateMap& states);


        /**
         * getNextPkey
         *
         * This will retrieve the next primary key for a SCADA item
         *
         * @return unsigned long - The next primary key
         * 
         * @exception DataException - If the next key could not be determined
         */
        unsigned long getNextPkey();

    };
} // closes TA_Base_Core

#endif // !defined(DERIVED_DATAPOINT_HELPER_H)

