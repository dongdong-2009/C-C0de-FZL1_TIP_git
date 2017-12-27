/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h $
  * @author HoaVu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * ConfiguredInputAssociation provides read-only parameters that are specific
  * to a Derived DataPoint input association in the database
  */


#ifndef CONFIGURED_INPUT_ASSOCIATION_H
#define CONFIGURED_INPUT_ASSOCIATION_H

#include <string>


namespace TA_Base_Core
{
	class ConfiguredInputAssociation
	{
	public:

		ConfiguredInputAssociation ( unsigned long mainEntityKey,
									const std::string & mainEntityName,
									unsigned long inputEntityKey,
									const std::string & inputEntityName,
									const std::string & inputEntityAgentName,
									unsigned long inputEntitySubsystemKey,
									unsigned long inputEntityLocationKey,
									int inputCalculationOrder,
									double predefinedInputValue )
		:
		m_mainEntityKey ( mainEntityKey ),
		m_mainEntityName ( mainEntityName ),
		m_inputEntityKey ( inputEntityKey ),
		m_inputEntityName ( inputEntityName ),
		m_inputEntityAgentName ( inputEntityAgentName ),
		m_inputEntitySubsystemKey ( inputEntitySubsystemKey ),
		m_inputEntityLocationKey ( inputEntityLocationKey ),
		m_inputCalculationOrder ( inputCalculationOrder ),
		m_predefinedInputValue ( predefinedInputValue )
		{
			// do nothing else
		};


		ConfiguredInputAssociation ( unsigned long inputEntityKey,
									const std::string & inputEntityName,
									const std::string & inputEntityAgentName,
									unsigned long inputEntitySubsystemKey,
									unsigned long inputEntityLocationKey )
		:
		m_mainEntityKey ( 0 ),
		m_mainEntityName ( "" ),
		m_inputEntityKey ( inputEntityKey ),
		m_inputEntityName ( inputEntityName ),
		m_inputEntityAgentName ( inputEntityAgentName ),
		m_inputEntitySubsystemKey ( inputEntitySubsystemKey ),
		m_inputEntityLocationKey ( inputEntityLocationKey ),
		m_inputCalculationOrder ( 0 ),
		m_predefinedInputValue ( 0 )
		{
			// do nothing else
		};


		virtual ~ConfiguredInputAssociation()
		{
			// do nothing
		};


        /**
         * getMainEntityKey
         *
         * Get the primary key of the entity that has input entities configured.
         *
         */
		virtual unsigned long getMainEntityKey() const
		{
			return m_mainEntityKey;
		};


        /**
         * getMainEntityName
         *
         * Get the name of the entity that has input entities configured.
         *
         */
		virtual const std::string & getMainEntityName() const
		{
			return m_mainEntityName;
		};


        /**
         * getInputEntityKey
         *
         * Get the primary key of the input entity associated with the main entity
         *
         */
		virtual unsigned long getInputEntityKey() const
		{
			return m_inputEntityKey;
		};


        /**
         * getInputEntityName
         *
         * Get the name of the input entity associated with the main entity
         *
         */
		virtual const std::string & getInputEntityName() const
		{
			return m_inputEntityName;
		};


        /**
         * getInputEntityAgentName
         *
         * Get the agent name of the input entity associated with the main entity
         *
         */
		virtual const std::string & getInputEntityAgentName() const
		{
			return m_inputEntityAgentName;
		};


        /**
         * getInputEntitySubsystemKey
         *
         * Get the subsystem key of the input entity associated with the main entity
         *
         */
		virtual unsigned long getInputEntitySubsystemKey() const
		{
			return m_inputEntitySubsystemKey;
		};


        /**
         * getInputEntityLocationKey
         *
         * Get the location key of the input entity associated with the main entity
         *
         */
		virtual unsigned long getInputEntityLocationKey() const
		{
			return m_inputEntityLocationKey;
		};


        /**
         * getInputCalculationOrder
         *
         * Get the calculation order of the input entity associated with the main entity
         *
         */
		virtual int getInputCalculationOrder() const
		{
			return m_inputCalculationOrder;
		};


        /**
         * getPredefinedInputValue
         *
         * Get the predefined value of the input entity associated with the main entity
         *
         */
		virtual double getPredefinedInputValue() const
		{
			return m_predefinedInputValue;
		};


    protected:
		ConfiguredInputAssociation() {};
		ConfiguredInputAssociation ( const ConfiguredInputAssociation & obj ) {};
		ConfiguredInputAssociation & operator= ( const ConfiguredInputAssociation & );

		unsigned long m_mainEntityKey;
		std::string m_mainEntityName;
		unsigned long m_inputEntityKey;
		std::string m_inputEntityName;
		std::string m_inputEntityAgentName;
		unsigned long m_inputEntitySubsystemKey;
		unsigned long m_inputEntityLocationKey;
		int m_inputCalculationOrder;
		double m_predefinedInputValue;
	};
}

#endif //CONFIGURED_INPUT_ASSOCIATION_H

