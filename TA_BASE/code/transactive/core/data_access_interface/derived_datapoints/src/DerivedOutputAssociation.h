/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedOutputAssociation.h $
  * @author HoaVu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * DerivedOutputAssociation provides read-only parameters that are specific
  * to a Derived DataPoint output association in the database
  */


#ifndef DERIVED_OUTPUT_ASSOCIATION_H
#define DERIVED_OUTPUT_ASSOCIATION_H

#include <string>


namespace TA_Base_Core
{
    class DerivedOutputAssociation
	{
	public:

		DerivedOutputAssociation ( unsigned long outputEntityKey,
								   const std::string & outputEntityName,
								   const std::string & outputEntityAgentName,
								   int outputValue )
		:
		m_outputEntityKey ( outputEntityKey ),
		m_outputEntityName ( outputEntityName ),
		m_outputEntityAgentName ( outputEntityAgentName ),
		m_outputValue ( outputValue )
		{
			// do nothing else
		};


		DerivedOutputAssociation ( const DerivedOutputAssociation & obj )
		:
		m_outputEntityKey ( obj.getOutputEntityKey() ),
		m_outputEntityName ( obj.getOutputEntityName() ),
		m_outputEntityAgentName ( obj.getOutputEntityAgentName() ),
		m_outputValue ( obj.getOutputValue() )
		{

		};


		virtual ~DerivedOutputAssociation()
		{
			// do nothing
		}


        /**
         * getOutputEntityKey
         *
         * Get the primary key of the output entity associated with a derived state
         *
         */
		virtual unsigned long getOutputEntityKey() const
		{
			return m_outputEntityKey;
		};


        /**
         * getOutputEntityName
         *
         * Get the name of the output entity associated with a derived state
         *
         */
		virtual const std::string & getOutputEntityName() const
		{
			return m_outputEntityName;
		};


        /**
         * getOutputEntityAgentName
         *
         * Get the agent name of the output entity associated with a derived state
         *
         */
		virtual const std::string & getOutputEntityAgentName() const
		{
			return m_outputEntityAgentName;
		};


        /**
         * getOutputValue
         *
         * Get the value to be set to the output entity associated with a derived state
         *
         */
		virtual int getOutputValue() const
		{
			return m_outputValue;
		};


	protected:

		DerivedOutputAssociation() {};
		DerivedOutputAssociation & operator= ( const DerivedOutputAssociation & ) ;

		unsigned long m_outputEntityKey;
		std::string m_outputEntityName;
		std::string m_outputEntityAgentName;
		int m_outputValue;
	};

}

#endif //DERIVED_OUTPUT_ASSOCIATION_H


