/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/SsaParamHelper.h $
  * @author:  Chris DeWolfe
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * This class abstracts a generic parameter - one that can be specifed at compile time to
  * hold a specific data type. It is to be used in conjuction with the EntityData class.
  *
  * Concepts of default, min, and max values, and validation are supported,
  *
  * The intended usage is that new parameters can be added to a TransAcive agent or GUI
  * with a few lines of code. Specifcally, you define a const for the param name, then
  * add an instance of this class to the map of the EntityData implimentation - see the
  * CTOR of SystemStatusAgentEntityData for an example.
  *
  * At the time of writing all I needed was int and std::string data types, hence that's
  * all that is implimented. Adding support for other data types should be trivial.
  *
  *
  * TODO: We need to decide where we are going to handle default values, as well as data
  * validation for params. I have implimented this in this class but I do not report
  * errors (other than logging) because no one would thank me for messing with the framework 
  * at this point in time. Some day it would be good if the configuration editor reported
  * when you try to set an out of range value.
  * I think that default values and range checking should be defined in the code not config...
  *
  * Should probably add mechanism to get default and range values
  */


#if !defined(AFX_SSAPARAMHELPER_H__DD4C1872_F0FD_44FB_B8D5_C070DD1601A__INCLUDED_)
#define AFX_SSAPARAMHELPER_H__DD4C1872_F0FD_44FB_B8D5_C070DD1601A__INCLUDED_

#include<string>

namespace TA_Base_Core
{

	class SsaParamHelper
	{

	public:

		//used to specify the data type of the parameter
		enum paramType {INT, STRING};

		/**
		* SsaParamHelper
		*
		* PRECONDITION: (minValue <= defaultValue <= maxValue) or 
		*BEHAVIOUR OF THIS METHOD IS UNDEFINED
		*
		* Use this CTOR if you want a param with an int data type
		*@param name: the name of the param
		*@param defaultValue: the value that this param is initialized with
		*@param minValue: the minimum value that this param can be set to
		*@param maxValue: the maximum value that this param can be set to
		*
		*/
		SsaParamHelper( const std::string& name, int defaultValue,
			int minValue, int maxValue);

		/**
		* SsaParamHelper
		*
		* PRECONDITION: if stringCanBeEmpty == false then defaultValue must not be "" or 
		*BEHAVIOUR OF THIS METHOD IS UNDEFINED
		*
		*  Use this CTOR if you want a param with an std::string data type
		*@param name: the name of the param
		*@param defaultValue: the value that this param is initialized with
		*@param stringCanBeEmpty: specifies if the value of this param can be set
		*to "" - default is false
		*/
		SsaParamHelper( const std::string& name, const std::string& defaultValue,
			bool stringCanBeEmpty = false);

		/**
		* ~SsaParamHelper
		*
		*DTOR don't do nuthin'
		*/
		virtual ~SsaParamHelper(){}

		/**
		* getDataType
		*
		*@return the data type of the parameter - note that data type is const, i.e.
		* it will not change at runtime
		*/
		paramType getDataType() {return m_type;}

		/**
		* getIntValue
		*
		* PRECONDITION: getDataType() must return SsaParamHelper::INT or 
		*BEHAVIOUR OF THIS METHOD IS UNDEFINED
		*
		*@return the current value
		*/
		unsigned int getIntValue();

		/**
		* setIntValue
		*
		* PRECONDITION: getDataType() must return SsaParamHelper::INT or 
		*BEHAVIOUR OF THIS METHOD IS UNDEFINED
		*
		*Set the current value. If the value specified is not in the valid range
		* the default value will be set and a error will be logged (DebugError).
		* See comments at top of class
		*
		*/
		void setIntValue( int newValue );

		/**
		* getStringValue
		*
		* PRECONDITION: getDataType() must return SsaParamHelper::STRING or 
		*BEHAVIOUR OF THIS METHOD IS UNDEFINED
		*
		*@return the current value
		*/
		std::string getStringValue();

		/**
		* setStringValue
		*
		* PRECONDITION: getDataType() must return SsaParamHelper::STRING or 
		*BEHAVIOUR OF THIS METHOD IS UNDEFINED
		*
		*Set the current value. If the value specified is not in the valid range
		* the default value will be set and a error will be logged (DebugError).
		* See comments at top of class
		*
		*/
		void setStringValue(const std::string& newValue);


		/**
		* setLoaded
		*
		* set the flag that indicates if the data is "dirty" or not
		*
		*/		
		void setLoaded(bool state) {m_isLoaded = state;}

		/**
		* isLoaded
		*
		*@return the flag that indicates if the data is "dirty" or not
		*
		*/	
		bool isLoaded(){return m_isLoaded;}

		/**
		* loadDefault
		*
		* set the params value to the default value supplied in the CTOR
		*
		*/	
		void loadDefault();

	private:
		const std::string m_name;
		const paramType m_type;

		//used only when data type is int
		struct intStuff
		{
			int value;
			int defaultValue;
			int min;
			int max;

		} m_intParam;

		//used only when data type is std::string
		struct stringStuff
		{
			std::string value;
			std::string defaultValue;
			bool canBeEmpty;

		} m_stringParam;

		bool m_isLoaded;
		
	};

} //end namespace TA_Base_Core
#endif // !defined(AFX_SSAPARAMHELPER_H__DD4C1872_F0FD_44FB_B8D5_C070DD1601A__INCLUDED_)
