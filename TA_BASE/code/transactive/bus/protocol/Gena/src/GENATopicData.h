/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENATopicData.h  $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for GENATopicData
  * Usage: class is used in receiving and updating 
  */

#ifndef __GENA_TOPIC_DATA_H_INCLUDED__
#define __GENA_TOPIC_DATA_H_INCLUDED__

#include "CommonDef.h"
#pragma once

namespace TA_Base_Bus
{
	class GENATopicData
	{
	public:

		/* 
		 * Copy Constructor
		 * @Param: rhs - instance of the GENATopicData to copy
		 */
		GENATopicData(const GENATopicData& rhs);

		/* 
		 * Constructor
		 * @Param: objID - Unique Identifier for this topic
		 * @Param: topicName - topic name
		 */
		GENATopicData(const std::string& objID, const std::string& topicName); 

		/* 
		 * Constructor
		 *
		 * @Param: xmlData - raw data in xml format
		 */
		GENATopicData(const std::string& xmlData);

		/* 
		 * Destructor
		 */
		~GENATopicData();
		
		/* 
		 * getTopicName
		 * @purpose: use to retrieve the topic name
		 *
		 * @return: std::string
		 */
		std::string getTopicName();

		/* 
		 * getObjectID
		 * @purpose: use to retrieve the Unique Identifier for this object
		 *
		 * @return: std::string
		 */
		std::string getObjectID();

		/* 
		 * getAttributes
		 * @purpose: get all the attribute of this topic object
		 *
		 * @return: a map of the all the attributes in this object
		 */
		T_AttribValueMap getAttributes();

		/* 
		 * setAttributes
		 * @purpose: get all the attribute of this topic object
		 *
		 * @return: a map of the all the attributes in this object
		 */
		void setAttributes(T_AttribValueMap attributes);

		/* 
		 * getAttributeNames
		 * @purpose: get all the attribute names of this topic object
		 *
		 * @return: std::string
		 */
		std::vector<std::string > getAttributeNames();

		/* 
		 * getAttribute
		 * @purpose: use to retrieve the topic name
		 *
		 * @return: std::string
		 */
		std::vector<AttributePtr> getAttribute(const std::string& attrbName);
		std::vector<AttributePtr> getAttributesByURI(const std::string& uri);
		/* 
		 * getAttributeValue
		 * @purpose: get the value the attribute by URI
		 * @param: URI 
		 *
		 * @return: value of the attribute queried otherwise returns empty string
		 */
		std::string getAttributeValue(const std::string& uri);

		/* 
		 * setAttributeValue
		 * @purpose: set the value of an attribute if attribute name exist, otherwise it will add a new one.
		 * @param: uri - path of the attribute to set. ex: test/test1/target
		 * @param: value - value of the attribute
		 *
		 * @return: true if set attribute is successful otherwise it will return false
		 */
		bool setAttributeValue(const std::string& uri, const std::string& value);

		/*
		* setUpdateType
		*/
		void setUpdateType(const std::string& updateType) { m_updateType = updateType; };
		
		/*
		* getUpdateType
		*/
		std::string getUpdateType() { return m_updateType; };

		/*
		* setTopicURI
		*/
		void setTopicURI(const std::string& topicURI) { m_topicURI = topicURI; };
		
		/*
		* getTopicURI
		*/
		std::string getTopicURI() { return m_topicURI; };


		std::string getDataStream();

	private:
		boost::shared_ptr<Attribute> _find_create(T_AttribValueMap& attributeMap, const std::string& attributeName);
	private:
		boost::shared_ptr<MessageDetail > m_msgDetail;
		std::string m_updateType;
		std::string m_topicURI;
	};

	// Type define shared pointer
	typedef boost::shared_ptr<GENATopicData> GENATopicDataPtr;

	// Type define vector of Gena Topic shared pointer
	typedef std::vector<GENATopicDataPtr> T_GENATopicDataList;

	std::string convertToUpperCase(const std::string& inputStr);

} //TA_Base_Bus

#endif //__GENA_MESSAGE_H_INCLUDED__
