/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DataPointEntityData.h $
  * @author HoaVu
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2016/12/30 17:34:07 $
  * Last modified by: $Author: wang.wang $
  *
  * DataPointEntityData provides read-only access to DataPoint entity defined in
  * the database.
  */

#ifndef DATAPOINT_ENTITY_DATA_H
#define DATAPOINT_ENTITY_DATA_H

#include <map>

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
	class DataPointParameterValuePair;
};

namespace TA_Base_Core
{
    class DataPointEntityData : public EntityData
    {
	public:

		DataPointEntityData();
        DataPointEntityData ( unsigned long key );

        virtual ~DataPointEntityData();

		//
		// operations required by IEntityData
		//

        /**
         * clone
         *
         * Returns a pointer to a new RTUEntityData object that has been initialised with the
         * specified key.
         *
         * @param key The key with which to construct the object.
         */
        static IEntityData* clone ( unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

		//
		// operations specific to StationSubSystemEntityData
		//

        /**
         * getOMFlag
         *
         * Returns the string indicating if the data point is operational,
		 * maintenant or both.  Possible values are: "O" for operational,
		 * "M" for maintenance, "B" for both and "N" for none.
         *
         * @return The OMF flag of this DataPoint entity as a std::string
         */
		const std::string  getOMFlag();

        /**
         * setOMFlag
         *
         * Set the the string indicating if the data point is operational,
		 * maintenant or both.
		 *
		 * @param newName  The new OMF name of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOMFlag ( const std::string & newName );

        /**
         * getDataPointType
         *
         * Returns the type of the DataPoint entity (i.e. "MeteredDataPoint" ).
         *
         * @return The type of this DataPoint entity as a std::string
         */
		const std::string  getDataPointType();

        /**
         * setDataPointType
         *
         * Set the type of the DataPoint entity to the specified type
		 *
		 * @param newType  The new type of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setDataPointType ( const std::string & newType );

        /**
         * getDataPointTypeShortName
         *
         * Returns the short name of the type, e.g. "AI" or "DI"
         *
		*/
		const std::string  getDataPointTypeShortName();

        /**
         * setDataPointTypeShortName
         *
         * Set the short name of the type, e.g. "AI" or "DI"
		 *
		 * @param newShortName  The new short name for the datapoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setDataPointTypeShortName ( const std::string & newShortName );

        /**
         * getDataType
         *
         * Returns the data type of the DataPoint entity (i.e. "IEEE_32" ).
         *
         * @return The data type of this DataPoint entity as a std::string
         */
		const std::string  getDataType();

        /**
         * setDataType
         *
         * Set the data type of the DataPoint entity to the specified type
		 *
		 * @param newType  The new data type of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setDataType ( const std::string & newType );

        /**
         * getHelpURL
         *
         * Returns the Help URL of the DataPoint entity
         *
         * @return The Help URL of this DataPoint entity as a std::string
         */
		const std::string  getHelpURL();

        /**
         * setHelpURL
         *
         * Set the Help URL of the DataPoint entity to the specified URL
		 *
		 * @param newURL  The new help URL of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHelpURL ( const std::string & newURL );

        /**
         * getEngineeringNote
         *
         * Returns the engineering note of the DataPoint entity
         *
         * @return The engineering note of this DataPoint entity as a std::string
         */
		const std::string  getEngineeringNote();

        /**
         * setEngineeringNote
         *
         * Set the engineering note of the DataPoint entity to the specified note
		 *
		 * @param newNote  The new engineering note of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEngineeringNote ( const std::string & newNote );

        /**
         * getFalseAlarmMessage
         *
         * Returns the false alarm message of the DataPoint entity
         *
         * @return The false alarm message of this DataPoint entity as a std::string
         */
		const std::string  getFalseAlarmMessage();

        /**
         * setFalseAlarmMessage
         *
         * Set the false alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new false alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setFalseAlarmMessage ( const std::string & newMsg );

        /**
         * getTrueAlarmMessage
         *
         * Returns the true alarm message of the DataPoint entity
         *
         * @return The true alarm message of this DataPoint entity as a std::string
         */
		const std::string  getTrueAlarmMessage();

        /**
         * setTrueAlarmMessage
         *
         * Set the true alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new true alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setTrueAlarmMessage ( const std::string & newMsg );

        /**
         * getLoLoAlarmMessage
         *
         * Returns the low low alarm message of the DataPoint entity
         *
         * @return The low low alarm message of this DataPoint entity as a std::string
         */
		const std::string  getLoLoAlarmMessage();

        /**
         * setLoLoAlarmMessage
         *
         * Set the low low alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new low low alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoLoAlarmMessage ( const std::string & newMsg );

        /**
         * getLoAlarmMessage
         *
         * Returns the low alarm message of the DataPoint entity
         *
         * @return The low alarm message of this DataPoint entity as a std::string
         */
		const std::string  getLoAlarmMessage();

        /**
         * setLoAlarmMessage
         *
         * Set the low alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new low alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoAlarmMessage ( const std::string & newMsg );

        /**
         * getHiAlarmMessage
         *
         * Returns the high alarm message of the DataPoint entity
         *
         * @return The high alarm message of this DataPoint entity as a std::string
         */
		const std::string  getHiAlarmMessage();

        /**
         * setHiAlarmMessage
         *
         * Set the high alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new high alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiAlarmMessage ( const std::string & newMsg );

        /**
         * getHiHiAlarmMessage
         *
         * Returns the high high alarm message of the DataPoint entity
         *
         * @return The high high alarm message of this DataPoint entity as a std::string
         */
		const std::string  getHiHiAlarmMessage();

        /**
         * setHiHiAlarmMessage
         *
         * Set the high high alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new high high alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiHiAlarmMessage ( const std::string & newMsg );

        /**
         * getEngineeringUnits
         *
         * Returns the engineering unit of the DataPoint entity
         *
         * @return The engineering unit of this DataPoint entity as a std::string
         */
		const std::string  getEngineeringUnits();

        /**
         * setEngineeringUnits
         *
         * Set the engineering unit of the DataPoint entity to the specified message
		 *
		 * @param newEngUnit  The new engineering unit of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEngineeringUnits ( const std::string & newEngUnit );

        /**
         * getTextEngineeringUnit
         *
         * Returns the text engineering unit of the DataPoint entity
         *
         * @return The text engineering unit of this DataPoint entity as a std::string
         */
		const std::string  getTextEngineeringUnit();

        /**
         * setTextEngineeringUnit
         *
         * Set the text engineering unit of the DataPoint entity to the specified message
		 *
		 * @param newEngUnit  The new text engineering unit of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setTextEngineeringUnit ( const std::string & newEngUnit );

        /**
         * getFalseLabel
         *
         * Returns the label for the false value of the DataPoint entity
         *
         * @return The label for the false value of this DataPoint entity as a std::string
         */
		const std::string  getFalseLabel();

        /**
         * setFalseLabel
         *
         * Set the label for the false value of the DataPoint entity to the specified message
		 *
		 * @param newEngUnit  The new label for the false value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setFalseLabel ( const std::string & newEngUnit );

        /**
         * getTrueLabel
         *
         * Returns the label for the true value of the DataPoint entity
         *
         * @return The label for the true value of this DataPoint entity as a std::string
         */
		const std::string  getTrueLabel();

        /**
         * setTrueLabel
         *
         * Set the label for the true value of the DataPoint entity to the specified message
		 *
		 * @param newEngUnit  The new label for the true value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setTrueLabel ( const std::string & newEngUnit );

        /**
         * DataPointEntityData::getViewAssociation
         *
         * Returns the view associated with the DataPoint entity
         *
         * @return The view associated with this DataPoint entity as a std::string
         */
		const std::string  getViewAssociation();

        /**
         * setViewAssociation
         *
         * Set the view associated with the DataPoint entity
		 *
		 * @param newViewAssociation  The new view associated with the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setViewAssociation ( const std::string & newViewAssociation );

        /**
         * getEntityAssociation
         *
         * Returns the entities associated with the DataPoint entity
         *
         * @return The list of entities associated with this DataPoint entity as a std::string
         */
		const std::string  getEntityAssociation();

        /**
         * setEntityAssociation
         *
         * Set the list of entities associated with the DataPoint entity
		 *
		 * @param newEntityAssociation  The new list of entities associated with the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEntityAssociation ( const std::string & newEntityAssociation );

        /**
         * getLaunchExpression
         *
         * Returns the launch expression to be used as interlocking mechanism
		 * before a command is executed
         *
         */
		const std::string  getLaunchExpression();

        /**
         * setLaunchExpression
         *
         * Set launch expression associated with the DataPoint entity to a new expression
		 *
		 * @param newExpression  The new launch expression for the data point
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLaunchExpression ( const std::string & newExpression );

        /**
         * getCalculatedExpression
         *
         * Returns the mathematical expression to be calculated as input to the
		 * Calculated DataPoint
         *
         */
		const std::string  getCalculatedExpression();

        /**
         * setCalculatedExpression
         *
         * Set the mathematical expression associated with the Calcualted DataPoint
		 * to a new expression
		 *
		 * @param newExpression  The new mathematical expression for the Calculated data point
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setCalculatedExpression ( const std::string & newExpression );

        /**
         * getABEExpression
         *
         * Returns the ABE expression to be used by the Answer Back Error Checking mechanism
         *
         */
		const std::string  getABEExpression();

		/**
		* getABEExpression
		*
		* Returns the ABE expression to be used by the Answer Back Error Checking mechanism
		*
		*/
		const std::string  getTrueRccExpression();

		/**
		* getABEExpression
		*
		* Returns the ABE expression to be used by the Answer Back Error Checking mechanism
		*
		*/
		const std::string  getFalseRccExpression();

        /**
         * setABEExpression
         *
         * Set ABE expression associated with the DataPoint entity to a new expression
		 *
		 * @param newExpression  The new ABE expression for the data point
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setABEExpression ( const std::string & newExpression );

        /**
         * getTrueAlarmMMS
         *
         * Returns the MMS condition for the datapoint's true alarm
         *
         */
		const std::string  getTrueAlarmMMS();

        /**
         * setTrueAlarmMMS
         *
         * Set the MMS condition for the datapoint's true alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's true alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setTrueAlarmMMS ( const std::string & newValue );

        /**
         * getFalseAlarmMMS
         *
         * Returns the MMS condition for the datapoint's false alarm
         *
         */
		const std::string  getFalseAlarmMMS();

        /**
         * setFalseAlarmMMS
         *
         * Set the MMS condition for the datapoint's false alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's false alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setFalseAlarmMMS ( const std::string & newValue );

        /**
         * getLoAlarmMMS
         *
         * Returns the MMS condition for the datapoint's low alarm
         *
         */
		const std::string  getLoAlarmMMS();

        /**
         * setLoAlarmMMS
         *
         * Set the MMS condition for the datapoint's low alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's low alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoAlarmMMS ( const std::string & newValue );

        /**
         * getLoLoAlarmMMS
         *
         * Returns the MMS condition for the datapoint's low low alarm
         *
         */
		const std::string  getLoLoAlarmMMS();

        /**
         * setLoLoAlarmMMS
         *
         * Set the MMS condition for the datapoint's low low alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's low low alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoLoAlarmMMS ( const std::string & newValue );

        /**
         * getHiAlarmMMS
         *
         * Returns the MMS condition for the datapoint's high alarm
         *
         */
		const std::string  getHiAlarmMMS();

        /**
         * setHiAlarmMMS
         *
         * Set the MMS condition for the datapoint's high alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's high alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiAlarmMMS ( const std::string & newValue );

        /**
         * getHiHiAlarmMMS
         *
         * Returns the MMS condition for the datapoint's high high alarm
         *
         */
		const std::string  getHiHiAlarmMMS();

        /**
         * setHiHiAlarmMMS
         *
         * Set the MMS condition for the datapoint's high high alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's high high alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiHiAlarmMMS ( const std::string & newValue );

        /**
         * getPrecision
         *
         * Returns the floating point precision of the DataPoint entity
         *
         * @return the floating point precision of this DataPoint entity
         */
		int getPrecision();

        /**
         * setPrecision
         *
         * Set floating point precision of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new floating point precision of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setPrecision ( int newValue );

        /**
         * getUpdateTimeLimit
         *
         * Returns the update time limit associated with the DataPoint entity
         *
         * @return the update time limit of this DataPoint entity
         */
		int getUpdateTimeLimit();

        /**
         * setUpdateTimeLimit
         *
         * Set the update time limit of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new update time limit of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setUpdateTimeLimit ( int newValue );

        /**
         * getABETimeoutPeriodSeconds
         *
         * Returns the ABE timeout period associated with the DataPoint entity
         *
         * @return the ABE timeout period of this DataPoint entity
         */
		int getABETimeoutPeriodSeconds();

        /**
         * setABETimeoutPeriodSeconds
         *
         * Set the ABE timeout period of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new ABE timeout period of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setABETimeoutPeriodSeconds ( int newValue );

        /**
         * getMaxTextLength
         *
         * Returns the maximum number of chars that a Text DataPoint can handle
         *
         * @return the maximum number of chars that a Text DataPoint can handle
         */
		int getMaxTextLength();

        /**
         * setMaxTextLength
         *
         * Set the maximum number of chars that a Text DataPoint can handle
		 *
		 * @param newValue  The new maximum number of chars of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setMaxTextLength ( int newValue );

        /**
         * getScanTimeFactor
         *
         * Returns the scan time factor associated with the DataPoint entity
         *
         * @return the scan time factor of this DataPoint entity
         */
		unsigned int getScanTimeFactor();

        /**
         * setScanTimeFactor
         *
         * Set the scan time factor of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new scan time factor of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setScanTimeFactor ( unsigned int newValue );

        /**
         * DataPointEntityData::getFalseAlarmDelay
         *
         * Returns the false alarm delay associated with the DataPoint entity
         *
         * @return the false alarm delay of this DataPoint entity
         */
		unsigned long getFalseAlarmDelay();

        /**
         * setFalseAlarmDelay
         *
         * Set the false alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new false alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setFalseAlarmDelay ( unsigned long newValue );

        /**
         * getTrueAlarmDelay
         *
         * Returns the true alarm delay associated with the DataPoint entity
         *
         * @return the true alarm delay of this DataPoint entity
         */
		unsigned long getTrueAlarmDelay();

        /**
         * setTrueAlarmDelay
         *
         * Set the true alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new true alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setTrueAlarmDelay ( unsigned long newValue );

        /**
         * getLoLoAlarmDelay
         *
         * Returns the low low alarm delay associated with the DataPoint entity
         *
         * @return the low low alarm delay of this DataPoint entity
         */
		unsigned long getLoLoAlarmDelay();

        /**
         * setLoLoAlarmDelay
         *
         * Set the low low alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new low low alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoLoAlarmDelay ( unsigned long newValue );

        /**
         * getLoAlarmDelay
         *
         * Returns the low alarm delay associated with the DataPoint entity
         *
         * @return the low alarm delay of this DataPoint entity
         */
		unsigned long getLoAlarmDelay();

        /**
         * setLoAlarmDelay
         *
         * Set the low alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new low alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoAlarmDelay ( unsigned long newValue );

        /**
         * getHiAlarmDelay
         *
         * Returns the high alarm delay associated with the DataPoint entity
         *
         * @return the high alarm delay of this DataPoint entity
         */
		unsigned long getHiAlarmDelay();

        /**
         * setHiAlarmDelay
         *
         * Set the high alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new high alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiAlarmDelay ( unsigned long newValue );

        /**
         * getHiHiAlarmDelay
         *
         * Returns the high high alarm delay associated with the DataPoint entity
         *
         * @return the high high alarm delay of this DataPoint entity
         */
		unsigned long getHiHiAlarmDelay();

        /**
         * setHiHiAlarmDelay
         *
         * Set the high high alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new high high alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiHiAlarmDelay ( unsigned long newValue );

        /**
         * getFalseAlarmSeverity
         *
         * Returns the fasle alarm severity associated with the DataPoint entity
         *
         * @return the fasle alarm severity of this DataPoint entity
         */
		int getFalseAlarmSeverity();

        /**
         * setFalseAlarmSeverity
         *
         * Set the fasle alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new fasle alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setFalseAlarmSeverity ( int newValue );

        /**
         * getTrueAlarmSeverity
         *
         * Returns the true alarm severity associated with the DataPoint entity
         *
         * @return the true alarm severity of this DataPoint entity
         */
		int getTrueAlarmSeverity();

        /**
         * setTrueAlarmSeverity
         *
         * Set the true alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new true alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setTrueAlarmSeverity ( int newValue );

        /**
         * getLoLoAlarmSeverity
         *
         * Returns the low low alarm severity associated with the DataPoint entity
         *
         * @return the low low alarm severity of this DataPoint entity
         */
		int getLoLoAlarmSeverity();

        /**
         * setLoLoAlarmSeverity
         *
         * Set the low low alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new low low alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoLoAlarmSeverity ( int newValue );

        /**
         * getLoAlarmSeverity
         *
         * Returns the low alarm severity associated with the DataPoint entity
         *
         * @return the low alarm severity of this DataPoint entity
         */
		int getLoAlarmSeverity();

        /**
         * setLoAlarmSeverity
         *
         * Set the low alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new low alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLoAlarmSeverity ( int newValue );

        /**
         * getHiAlarmSeverity
         *
         * Returns the high alarm severity associated with the DataPoint entity
         *
         * @return the high alarm severity of this DataPoint entity
         */
		int getHiAlarmSeverity();

        /**
         * setHiAlarmSeverity
         *
         * Set the high alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new high alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiAlarmSeverity ( int newValue );

        /**
         * getHiHiAlarmSeverity
         *
         * Returns the high high alarm severity associated with the DataPoint entity
         *
         * @return the high high alarm severity of this DataPoint entity
         */
		int getHiHiAlarmSeverity();

        /**
         * setHiHiAlarmSeverity
         *
         * Set the high high alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new high high alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setHiHiAlarmSeverity ( int newValue );

        /**
         * getEngineeringLimitHigh
         *
         * Returns the engineering limit high value associated with the DataPoint entity
         *
         * @return the engineering limit high value of this DataPoint entity
         */
		double getEngineeringLimitHigh();

        /**
         * setEngineeringLimitHigh
         *
         * Set the engineering limit high value of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new engineering limit high value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEngineeringLimitHigh ( double newValue );

        /**
         * getEngineeringLimitLow
         *
         * Returns the engineering limit low value associated with the DataPoint entity
         *
         * @return the engineering limit low value of this DataPoint entity
         */
		double getEngineeringLimitLow();

        /**
         * setEngineeringLimitLow
         *
         * Set the engineering limit low value of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new engineering limit low value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEngineeringLimitLow ( double newValue );

        /**
         * getRawLimitHigh
         *
         * Returns the raw limit high value associated with the DataPoint entity
         *
         * @return the raw limit high value of this DataPoint entity
         */
		double getRawLimitHigh();

        /**
         * setRawLimitHigh
         *
         * Set the raw limit high value of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new raw limit high value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setRawLimitHigh ( double newValue );

        /**
         * getRawLimitLow
         *
         * Returns the raw limit low value associated with the DataPoint entity
         *
         * @return the raw limit low value of this DataPoint entity
         */
		double getRawLimitLow();

        /**
         * setRawLimitLow
         *
         * Set the raw limit low value of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new raw limit low value of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setRawLimitLow ( double newValue );

        /**
         * getZeroValueThreshold
         *
         * Returns the zero value threshold associated with the DataPoint entity
         *
         * @return the zero value threshold of this DataPoint entity
         */
		double getZeroValueThreshold();

        /**
         * setZeroValueThreshold
         *
         * Set the zero value threshold of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new zero value threshold of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setZeroValueThreshold ( double newValue );

        /**
         * getUpdateThreshold
         *
         * Returns the update threshold value associated with the DataPoint entity
         *
         * @return the update threshold of this DataPoint entity
         */
		double getUpdateThreshold();

        /**
         * setUpdateThreshold
         *
         * Set the update threshold of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new update threshold of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setUpdateThreshold ( double newValue );

        /**
         * getAlarmThreshold
         *
         * Returns the alarm threshold value associated with the DataPoint entity
         *
         * @return the alarm threshold of this DataPoint entity
         */
		double getAlarmThreshold();

        /**
         * setAlarmThreshold
         *
         * Set the alarm threshold of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new alarm threshold of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setAlarmThreshold ( double newValue );

        /**
         * getEnableHelpURL
         *
         * Returns indication if the data point Help URL function is enabled
         *
         * @return True to indicate Help URL is enabled
         */
		bool getEnableHelpURL();

        /**
         * setEnableHelpURL
         *
         * Enable/Disable the help URL of the data point
		 *
		 * @param newIndication  indication if the data point help URL is enabled
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEnableHelpURL ( bool newIndication );

		//Using for IEC104 Protocol 
		/**
		* gettPScadaFlag
		*
		* Returns PScada Flag 
		*
		* @return True to indicate This Data Point is a PScada Data Point
		*/
		bool getPScadaFlag();

		/**
		* setPScadaFlag
		*
		* Set the PScada Flag for this Data Point
		*
		* @param newPScadaFlag indication the flag value
		*
		* @return True if the operation is successfull, false otherwise
		*/
		bool setPScadaFlag( bool newPScadaFlag );

		/**
		* getRelatedInputPoint
		*
		* Get the Related Input Data Point key for this Data Point
		*
		* @return the value if the operation is successfull, 0 otherwise
		*/
		unsigned long getRelatedInputPoint();
        
        /**
		* getRelatedWfPoint
		*
		* Get the Related WF(\CE\E5\B7\C0) Data Point key for this Data Point
		*
		* @return the value if the operation is successfull, 0 otherwise
		*/
		unsigned long getRelatedWfPoint();

        /**
         * getEnableEngineeringNote
         *
         * Returns indication if the data point Engineering Note function is enabled
         *
         * @return True to indicate Engineering Note is enabled
         */
		bool getEnableEngineeringNote();

        /**
         * setEnableEngineeringNote
         *
         * Enable/Disable the Engineering Note of the data point
		 *
		 * @param newIndication  indication if the data point Engineering Note is enabled
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEnableEngineeringNote ( bool newIndication );

        /**
         * getEnableTrending
         *
         * Returns indication if the data point trending function is enabled
         *
         * @return True to indicate trending is enabled
         */
		bool getEnableTrending();

        /**
         * setEnableEngineeringNote
         *
         * Enable/Disable the trending of the data point
		 *
		 * @param newIndication  indication if the data point trending is enabled
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEnableTrending ( bool newIndication );

        /**
         * getEngineeringUnitsEnabled
         *
         * Returns indication if the data point engineering units function is enabled
         *
         * @return True to indicate engineering units function is enabled
         */
		bool getEngineeringUnitsEnabled();

        /**
         * setEngineeringUnitsEnabled
         *
         * Enable/Disable the engineering units function of the data point
		 *
		 * @param newIndication  indication if the data point engineering units function is enabled
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setEngineeringUnitsEnabled ( bool newIndication );

        /**
         * getAllowWrite
         *
         * Returns indication if the data point is writeable
         *
         * @return True to indicate if the data point is writeable
         */
		bool getAllowWrite();

        /**
         * setAllowWrite
         *
         * Indicate if the data point is writeable
		 *
		 * @param newIndication  indication if the data point is writeable
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setAllowWrite ( bool newIndication );

        /**
         * getDisplayOrder
         *
         * Returns the display order of the data point
         *
         */
		long getDisplayOrder();

        /**
         * setDisplayOrder
         *
         * Set the data point's display order to the new specified value.
		 *
		 * @param newOrder  the new display order assigned to the data point
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setDisplayOrder ( long newOrder );

		/**
		* getBitWidth
		*
		* Returns the bit width of the data point, for ai.
		*
		*/
		long getBitWidth();

		/**
		* setBitWidth
		*
		* Set the data point's bit width to the new specified value.
		*
		* @param bitWidth  the new bit width assigned to the data point
		*
		* @return True if the operation is successfull, false otherwise
		*/
		bool setBitWidth ( long bitWidth );

		/**
		* getIsBigEndian
		*
		* Returns wheather bit sequence is bigEndian,this parameter is used by mutibit datapoint. 
		*
		*/

		bool getIsBigEndian();

		/**
		* setIsBigEndian
		*
		* set the datapoint's bit sequence to isBigEndian 
		*
		*/
		bool setIsBigEndian(bool isBigEndian);
        /**
         * getUpdatePriority
         *
         * Returns the update priority level of the data point.  Zero (0) is returned
		 * if the parameter is not defined.
         *
         */
		long getUpdatePriorityLevel();

        /**
         * setUpdatePriority
         *
         * Set the data point's update priority level to the new specified value.
		 *
		 * @param newOrder  the new display order assigned to the data point
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setUpdatePriorityLevel ( long newPriority );

        /**
         * getLogChanges
         *
         * Returns an indication if the data point value is to logged when it is
		 * changed.
         *
         * @return True to indicate if the data point value is to be logged
         */
		bool getLogChanges();

        /**
         * setLogChanges
         *
         * Indicate if the data point value is to logged
		 *
		 * @param newIndication  indication if the data point value is
		 *						 to be logged
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setLogChanges ( bool newIndication );

        /**
         * getWordSwapIndication
         *
         * Returns an indication if the data point's 32-bit value has the higher
		 * and lower 16-bit words swapped.  True for swapping, false for none.
         *
         */
		bool getWordSwapIndication();

        /**
         * getByteSwapIndication
         *
         * Returns an indication if the data point's 16-bit value has the higher
		 * and lower 8-bit words swapped.  True for swapping, false for none.
         *
         */
		bool getByteSwapIndication();

        /**
         * setWordSwapIndication
         *
         * Indicate if the data point's 32 bit value has the higher and lower
		 * 16-bit words swapped.
		 *
		 * @param newIndication  indication if the data point's 32-bit value has
		 *						 the higher and lower 16-bit words swapped.
		 *						 True for swapping and false for none.
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setWordSwapIndication ( bool newIndication );

        /**
         * setByteSwapIndication
         *
         * Indicate if the data point's 16 bit value has the higher and lower
		 * 8-bit words swapped.
		 *
		 * @param newIndication  indication if the data point's 16-bit value has
		 *						 the higher and lower 8-bit words swapped.
		 *						 True for swapping and false for none.
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setByteSwapIndication ( bool newIndication );

        /**
         * getRolloverConstant
         *
         * Returns the Rollover Constant value of the Metered data point
         *
         */
		double getRolloverConstant();

        /**
         * setRolloverConstant
         *
         * Set the Rollover Constant value of the Metered data point to the new
		 * specified value.
		 *
		 * @param newValue  the new value to be saved
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setRolloverConstant ( double newValue );

        /**
         * getMeterFactor
         *
         * Returns the Meter Factor value of the Metered data point
         *
         */
		double getMeterFactor();

        /**
         * setMeterFactor
         *
         * Set the Meter Factor value of the Metered data point to the new
		 * specified value.
		 *
		 * @param newValue  the new value to be saved
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setMeterFactor ( double newValue );

        /**
         * getMIDataPointPKey()
         *
         * Returns the pkey of the MI DataPoint associated with the Metered
		 * DataPoint
         *
         * @return The name of the the MI DataPoint
         */
		unsigned long getMIDataPointPKey();

        /**
         * setMIDataPointPKey
         *
         * Set the pkey of the MI DataPoint associated with the Metered
		 * DataPoint to the specified name
		 *
		 * @param newName  The new pkey for the MI DataPoint associated
		 *				   with the Metered DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setMIDataPointPKey ( unsigned long newValue );

        /**
         * getOverScaleHiAlarmDelay
         *
         * Returns the over scale high alarm delay associated with the DataPoint entity
         *
         * @return the over scale high alarm delay of this DataPoint entity
         */
		unsigned long getOverScaleHiAlarmDelay();

        /**
         * setOverScaleHiAlarmDelay
         *
         * Set the over scale high alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new over scale high alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleHiAlarmDelay ( unsigned long newValue );

        /**
         * getOverScaleHiAlarmSeverity
         *
         * Returns the over scale high alarm severity associated with the DataPoint entity
         *
         * @return the over scale high alarm severity of this DataPoint entity
         */
		int getOverScaleHiAlarmSeverity();

        /**
         * setOverScaleHiAlarmSeverity
         *
         * Set the over scale high alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new over scale high alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleHiAlarmSeverity ( int newValue );

        /**
         * getOverScaleHiAlarmMessage
         *
         * Returns the over scale high alarm message of the DataPoint entity
         *
         * @return The over scale high alarm message of this DataPoint entity as a std::string
         */
		const std::string  getOverScaleHiAlarmMessage();

        /**
         * setOverScaleHiAlarmMessage
         *
         * Set the over scale high alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new over scale high alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleHiAlarmMessage ( const std::string & newMsg );

        /**
         * getOverScaleHiAlarmMMS
         *
         * Returns the MMS condition for the datapoint's over scale high alarm
         *
         */
		const std::string  getOverScaleHiAlarmMMS();

        /**
         * setOverScaleHiAlarmMMS
         *
         * Set the MMS condition for the datapoint's over scale high alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's over scale high alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleHiAlarmMMS ( const std::string & newValue );

        /**
         * getOverScaleLoAlarmDelay
         *
         * Returns the over scale low alarm delay associated with the DataPoint entity
         *
         * @return the over scale low alarm delay of this DataPoint entity
         */
		unsigned long getOverScaleLoAlarmDelay();

        /**
         * setOverScaleLoAlarmDelay
         *
         * Set the over scale low alarm delay of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new over scale low alarm delay of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleLoAlarmDelay ( unsigned long newValue );

        /**
         * getOverScaleLoAlarmSeverity
         *
         * Returns the over scale low alarm severity associated with the DataPoint entity
         *
         * @return the over scale low alarm severity of this DataPoint entity
         */
		int getOverScaleLoAlarmSeverity();

        /**
         * setOverScaleLoAlarmSeverity
         *
         * Set the over scale low alarm severity of this DataPoint entity to the specified value
		 *
		 * @param newValue  The new over scale low alarm severity of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleLoAlarmSeverity ( int newValue );

        /**
         * getOverScaleLoAlarmMessage
         *
         * Returns the over scale low alarm message of the DataPoint entity
         *
         * @return The over scale low alarm message of this DataPoint entity as a std::string
         */
		const std::string  getOverScaleLoAlarmMessage();

        /**
         * setOverScaleLoAlarmMessage
         *
         * Set the over scale low alarm message of the DataPoint entity to the specified message
		 *
		 * @param newMsg  The new over scale low alarm message of the DataPoint
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleLoAlarmMessage ( const std::string & newMsg );

        /**
         * getOverScaleLoAlarmMMS
         *
         * Returns the MMS condition for the datapoint's over scale low alarm
         *
         */
		const std::string  getOverScaleLoAlarmMMS();

        /**
         * setOverScaleLoAlarmMMS
         *
         * Set the MMS condition for the datapoint's over scale low alarm
		 *
		 * @param newValue  The new MMS condition for datapoint's over scale low alarm
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setOverScaleLoAlarmMMS ( const std::string & newValue );


		bool getIsDpParameterCreatedInDb ( const std::string & parameterName ) ;


		/**
		* getSpecialAlarmType
		*
		* Returns the Special Alarm Type of the DataPoint entity
		*
		* @return the Special Alarm Type of this DataPoint entity as a std::string
		*/
		const std::string  getSpecialAlarmType();

		/**
		* getEventMessage
		*
		* Returns the Event Message of the DataPoint entity
		*
		* @return the Event Message of the DataPoint entity as a std::string
		*/
		std::string getEventMessage();
    protected:


    private:

        DataPointEntityData ( const DataPointEntityData & obj );
		DataPointEntityData & operator= ( const DataPointEntityData &);
    
    private:

        /**
         * setParameterValues
         *
         * Set the specified parameters of the DataPoint entity to the specified value
		 *
		 * @param parameters The map of new values for specified parameters
		 *					 The parameters map is in the form of Name->Value
         *
         * @return True if the operation is successfull, false otherwise
         */
		bool setParameterValues ( std::map < std::string, std::string > & parameters );


        /**
         * isNumericalString
         *
         * Returns true if the specified string is a numerical string, false
		 * otherwise.
		 *
		 * @param str The string to be checked
         *
         */
		bool isNumericalString ( const std::string & str );


        /**
         * removeSpaces
         *
         * Returns a string that has all white-space characters removed
		 *
		 * @param originalStr The string to be checked
         *
         */
		std::string removeSpaces ( const std::string & originalStr );


		std::string getStringData ( const std::string & parameterName );
		bool getBooleanData ( const std::string & parameterName );
		int getIntegerData ( const std::string & parameterName );
		unsigned long getUnsignedLongData ( const std::string & parameterName );
		double getRealData  ( const std::string & parameterName );

		bool m_firstTimeReload;

    private:
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

	public:

		static const std::string DATAPOINT_TYPE;
		static const std::string DATAPOINT_TYPE_SHORT_NAME;
		static const std::string DATAPOINT_DATA_TYPE;
		static const std::string DATAPOINT_HELP_URL;
		static const std::string DATAPOINT_VIEW_ASSOCIATION;
		static const std::string DATAPOINT_ENTITY_ASSOCIATION;
		static const std::string DATAPOINT_LAUNCH_CONDITION;
		static const std::string DATAPOINT_ABE_EXPRESSION;
		static const std::string DATAPOINT_PRECISION;
		static const std::string DATAPOINT_UPDATE_TIME_LIMIT;
		static const std::string DATAPOINT_ABE_TIMEOUT_PERIOD_SECONDS;
		static const std::string DATAPOINT_SCAN_TIME_FACTOR;
		static const std::string DATAPOINT_ENGINEERING_NOTE;
		static const std::string DATAPOINT_ENGINEERING_UNITS;
		static const std::string DATAPOINT_TEXT_ENGINEERING_UNIT;
		static const std::string DATAPOINT_FALSE_ENGINEERING_UNIT;
		static const std::string DATAPOINT_TRUE_ENGINEERING_UNIT;
		static const std::string DATAPOINT_FALSE_ALARM_MESSAGE;
		static const std::string DATAPOINT_TRUE_ALARM_MESSAGE;
		static const std::string DATAPOINT_LOLO_ALARM_MESSAGE;
		static const std::string DATAPOINT_LO_ALARM_MESSAGE;
		static const std::string DATAPOINT_HI_ALARM_MESSAGE;
		static const std::string DATAPOINT_HIHI_ALARM_MESSAGE;
		static const std::string DATAPOINT_FALSE_ALARM_DELAY;
		static const std::string DATAPOINT_TRUE_ALARM_DELAY;
		static const std::string DATAPOINT_LOLO_ALARM_DELAY;
		static const std::string DATAPOINT_LO_ALARM_DELAY;
		static const std::string DATAPOINT_HI_ALARM_DELAY;
		static const std::string DATAPOINT_HIHI_ALARM_DELAY;
		static const std::string DATAPOINT_FALSE_ALARM_SEVERITY;
		static const std::string DATAPOINT_TRUE_ALARM_SEVERITY;
		static const std::string DATAPOINT_LOLO_ALARM_SEVERITY;
		static const std::string DATAPOINT_LO_ALARM_SEVERITY;
		static const std::string DATAPOINT_HI_ALARM_SEVERITY;
		static const std::string DATAPOINT_HIHI_ALARM_SEVERITY;
		static const std::string DATAPOINT_FALSE_ALARM_MMS;
		static const std::string DATAPOINT_TRUE_ALARM_MMS;
		static const std::string DATAPOINT_LOLO_ALARM_MMS;
		static const std::string DATAPOINT_LO_ALARM_MMS;
		static const std::string DATAPOINT_HI_ALARM_MMS;
		static const std::string DATAPOINT_HIHI_ALARM_MMS;
		static const std::string DATAPOINT_LOLO_ALARM_VALUE;
		static const std::string DATAPOINT_LO_ALARM_VALUE;
		static const std::string DATAPOINT_HI_ALARM_VALUE;
		static const std::string DATAPOINT_HIHI_ALARM_VALUE;
		static const std::string DATAPOINT_ENGINEERING_LIMIT_HIGH;
		static const std::string DATAPOINT_ENGINEERING_LIMIT_LOW;
		static const std::string DATAPOINT_RAW_LIMIT_HIGH;
		static const std::string DATAPOINT_RAW_LIMIT_LOW;
		static const std::string DATAPOINT_ZERO_VALUE_THRESHOLD;
		static const std::string DATAPOINT_UPDATE_THRESHOLD;
		static const std::string DATAPOINT_ALARM_THRESHOLD;
		static const std::string DATAPOINT_ENABLE_HELP_URL;
		static const std::string DATAPOINT_ENABLE_ENGINEERING_NOTE;
		static const std::string DATAPOINT_ENABLE_TRENDING;
		static const std::string DATAPOINT_ENGINEERING_UNITS_ENABLED;
		static const std::string DATAPOINT_ALLOW_WRITE;
		static const std::string DATAPOINT_SERVICE_STATE;
		static const std::string DATAPOINT_DISPLAY_ORDER;
		static const std::string DATAPOINT_UPDATE_PRIORITY;
		static const std::string DATAPOINT_LOG_CHANGES;
		static const std::string DATAPOINT_ROLLOVER_CONSTANT;
		static const std::string DATAPOINT_METER_FACTOR;
		static const std::string DATAPOINT_MI_DATAPOINT_PKEY;
		static const double FAULTY_VALUE;
		static const std::string DATAPOINT_MAX_TEXT_LENGTH;
		static const std::string DATAPOINT_LAST_OPERATOR_SESSION_ID;
		static const std::string DATAPOINT_OMFLAG;
        static const std::string DATAPOINT_WORD_SWAP;
        static const std::string DATAPOINT_BYTE_SWAP;
		static const std::string DATAPOINT_BIT_WIDTH;
		static const std::string DATAPOINT_BIT_SEQUENCE;
		static const std::string DATAPOINT_CALCULATED_EXPRESSION;
		static const std::string DATAPOINT_MMS_REPORT_TYPE;
		static const std::string DATAPOINT_MMS_REPORT_PERIOD;
		static const std::string DATAPOINT_MMS_REPORT_TIME;
		static const std::string DATAPOINT_MMS_NONE_STRING;;
		static const std::string DATAPOINT_MMS_AUTO_STRING;
		static const std::string DATAPOINT_MMS_SEMI_STRING;

		static const std::string DATAPOINT_OH_ALARM_VALUE;	// DpOverScaleHi
		static const std::string DATAPOINT_OH_ALARM_DELAY;
		static const std::string DATAPOINT_OH_ALARM_SEVERITY;
		static const std::string DATAPOINT_OH_ALARM_MESSAGE;
		static const std::string DATAPOINT_OH_ALARM_MMS;
	
		static const std::string DATAPOINT_OL_ALARM_VALUE; // DpOverScaleLo
		static const std::string DATAPOINT_OL_ALARM_DELAY;
		static const std::string DATAPOINT_OL_ALARM_SEVERITY;
		static const std::string DATAPOINT_OL_ALARM_MESSAGE;
		static const std::string DATAPOINT_OL_ALARM_MMS;

		//TD15446
		static const std::string DATAPOINT_ENTITYKEY;
		static const std::string DATAPOINT_HI_ALARM_ENABLED;
		static const std::string DATAPOINT_HIHI_ALARM_ENABLED;
		static const std::string DATAPOINT_OH_ALARM_ENABLED;
		static const std::string DATAPOINT_LO_ALARM_ENABLED;
		static const std::string DATAPOINT_LOLO_ALARM_ENABLED;
		static const std::string DATAPOINT_OL_ALARM_ENABLED;
		static const std::string DATAPOINT_TRUE_ALARM_ENABLED;
		static const std::string DATAPOINT_FALSE_ALARM_ENABLED;
		
	    static const std::string GLOBAL_MMS_INHIBITED_DATAPOINT;    // Raymond Pau++ Bug #798 (TD13100)

        static const std::string DATAPOINT_EVENT_MESSAGE;   // Raymond Pau++ Bug #802 (TD7194)

		static const std::string DATAPOINT_PSCADA_FLAG;					// Using for IEC 104 Protocol
		static const std::string DATAPOINT_RELATED_INPUT_DATAPOINT;		// Using for IEC 104 Protocol
		static const std::string DATAPOINT_TRUE_RCC;
		static const std::string DATAPOINT_FALSE_RCC;
        
        static const std::string DATAPOINT_RELATED_WF_DATAPOINT;        // Using for WF

		static const std::string DATAPOINT_SPECIAL_ALARM_TYPE;			//Using for Special Alarm, such as FireAlarm

    };

    typedef boost::shared_ptr<DataPointEntityData> DataPointEntityDataPtr;

} //close namespace TA_Base_Core

#endif
