/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GenaMessage.cpp  $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class Implementation for GenaMessage
  */


#include "XmlUtility.h"
#include "GENATopicData.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "boost/algorithm/string.hpp"

namespace TA_Base_Bus
{
	std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			if (!item.empty())
				elems.push_back(item);
		}
		return elems;
	}


	std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	std::string convertToUpperCase(const std::string& inputStr)
	{
		std::string strTmp;
		strTmp = inputStr;
		boost::to_upper(strTmp);
		return strTmp;
	}

	GENATopicData::GENATopicData(const std::string& objID, const std::string& topicName)
	{
		FUNCTION_ENTRY("Constructor");

		m_msgDetail.reset(new MessageDetail());
		m_msgDetail->objID = objID;
		m_msgDetail->topicName = convertToUpperCase(topicName);

		FUNCTION_EXIT;
	}

	boost::shared_ptr<Attribute> AttributeDeepCopy(boost::shared_ptr<Attribute> objToCopy)
	{
		boost::shared_ptr<Attribute> pRetVal(new Attribute);
		pRetVal->name = convertToUpperCase(objToCopy->name);
		pRetVal->value = objToCopy->value;

		if (pRetVal->subAttribute.size() != 0)
		{	
			T_AttribValueMap::iterator itr = objToCopy->subAttribute.begin();
			for (itr; itr!=objToCopy->subAttribute.end(); itr++)
			{
				boost::shared_ptr<Attribute> pTemp = AttributeDeepCopy(itr->second);
				pRetVal->subAttribute.insert(T_AttribValueMap::value_type(pTemp->name, pTemp));
			}
		}

		return pRetVal;
	}

	GENATopicData::GENATopicData(const GENATopicData& rhs)
	{
		FUNCTION_ENTRY("Constructor");
		m_updateType = rhs.m_updateType;
		m_topicURI = rhs.m_topicURI;

		// Perform a deep copy
		m_msgDetail.reset(new MessageDetail);
		m_msgDetail->objID = rhs.m_msgDetail->objID;
		m_msgDetail->topicName = convertToUpperCase( rhs.m_msgDetail->topicName);

		T_AttribValueMap::iterator itr = rhs.m_msgDetail->attribValue.begin();
		for (itr; itr!=rhs.m_msgDetail->attribValue.end(); itr++)
		{
			boost::shared_ptr<Attribute> pTemp = AttributeDeepCopy(itr->second);
			m_msgDetail->attribValue.insert(T_AttribValueMap::value_type(pTemp->name, pTemp));
		}
		
		FUNCTION_EXIT;
	}

	GENATopicData::GENATopicData(const std::string& xmlData)
	{
		FUNCTION_ENTRY("Constructor");
		m_msgDetail.reset( XmlUtility::xmlToDetail(xmlData)); 
		FUNCTION_EXIT;
	}

	GENATopicData::~GENATopicData()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}

	std::string GENATopicData::getTopicName()
	{
		FUNCTION_ENTRY("getTopicName");
		FUNCTION_EXIT;
		return m_msgDetail->topicName;
	}

	std::string GENATopicData::getObjectID()
	{
		FUNCTION_ENTRY("getObjectID");
		FUNCTION_EXIT;
		return m_msgDetail->objID;
	}

	T_AttribValueMap GENATopicData::getAttributes()
	{
		FUNCTION_ENTRY("getAttributes");
		FUNCTION_EXIT;
		return m_msgDetail->attribValue;
	}

	void GENATopicData::setAttributes(T_AttribValueMap attributes)
	{
		FUNCTION_ENTRY("setAttributes");
		m_msgDetail->attribValue = attributes;
		FUNCTION_EXIT;
	}

	std::vector<std::string > GENATopicData::getAttributeNames()
	{
		FUNCTION_ENTRY("getAttributeNames");
		std::vector<std::string > vecRetVal;
		
		T_AttribValueMap::iterator itr = m_msgDetail->attribValue.begin();
		for (itr; itr!=m_msgDetail->attribValue.end(); ++itr)
		{
			vecRetVal.push_back(itr->first);
		}

		FUNCTION_EXIT;
		return vecRetVal;
	}

	std::vector<AttributePtr> GENATopicData::getAttribute(const std::string& attrbName)
	{
		FUNCTION_ENTRY("getAttribute");
		std::vector<AttributePtr> pRetVal;
		
		T_AttribValueMap::iterator itr = m_msgDetail->attribValue.begin();
		for (itr; itr!=m_msgDetail->attribValue.end(); itr++)
		{
			if (itr->first == attrbName)
			{
				pRetVal.push_back(itr->second);
			}
		}

		FUNCTION_EXIT;
		return pRetVal;
	}

	
	std::vector<AttributePtr> GENATopicData::getAttributesByURI(const std::string& uri)
	{
		FUNCTION_ENTRY("getAttribute");
		std::vector<AttributePtr> pRetVal;

		std::vector<std::string> vectemp = split(uri, '/') ;

		bool bFirst = true;
		boost::shared_ptr<Attribute> pTemp;
		if (vectemp.size() > 1)
		{
			for (int idx=0; idx<vectemp.size() - 1; idx++)
			{
				if (bFirst)
				{
					T_AttribValueMap::iterator itFind = m_msgDetail->attribValue.find(vectemp[idx]);
					if (itFind != m_msgDetail->attribValue.end())
						pTemp = itFind->second;
					else {
						pTemp.reset();
						break;
					}
					bFirst = false;
				}
				else
				{
					T_AttribValueMap::iterator itFind = pTemp->subAttribute.find(vectemp[idx]);
					if (itFind != pTemp->subAttribute.end())
						pTemp = itFind->second;
					else {
						pTemp.reset();
						break;
					}
				}
			}

			if (pTemp.get() != 0)
			{	// The attributes if there are.
				T_AttribValueMap::iterator itr = pTemp->subAttribute.begin();
				for (itr; itr!=pTemp->subAttribute.end(); itr++)
				{
					if (itr->first == vectemp[vectemp.size() - 1])
					{
						pRetVal.push_back(itr->second);
					}
				}
			}
		}
		else
		{
			// The attributes if there are.
			T_AttribValueMap::iterator itr = m_msgDetail->attribValue.begin();
			for (itr; itr!=m_msgDetail->attribValue.end(); itr++)
			{
				if (itr->first == vectemp[vectemp.size() - 1])
				{
					pRetVal.push_back(itr->second);
				}
			}
		}
		
		FUNCTION_EXIT;
		return pRetVal;
	}

	std::string GENATopicData::getAttributeValue(const std::string& uri)
	{
		FUNCTION_ENTRY("getAttributeValue");
		std::string strRetVal = "";

		std::vector<std::string> vectemp = split(uri, '/') ;

		bool bFirst = true;
		boost::shared_ptr<Attribute> pTemp;
		for (int idx=0; idx<vectemp.size(); idx++)
		{
			if (bFirst)
			{
				T_AttribValueMap::iterator itFind = m_msgDetail->attribValue.find(vectemp[idx]);
				if (itFind != m_msgDetail->attribValue.end())
					pTemp = itFind->second;
				else {
					pTemp.reset();
					break;
				}
				bFirst = false;
			}
			else
			{
				T_AttribValueMap::iterator itFind = pTemp->subAttribute.find(vectemp[idx]);
				if (itFind != pTemp->subAttribute.end())
					pTemp = itFind->second;
				else {
					pTemp.reset();
					break;
				}
			}
		}

		if (pTemp.get() != 0)
		{
			strRetVal = pTemp->value;
		}
		
		FUNCTION_EXIT;
		return strRetVal;
	}

	bool GENATopicData::setAttributeValue(const std::string& uri, const std::string& value)
	{
		FUNCTION_ENTRY("setAttributeValue");
		bool bRetVal = true;
		std::vector<std::string> vectemp = split(uri, '/') ;

		bool bFirst = true;
		boost::shared_ptr<Attribute> pTemp;
		for (int idx=0; idx<vectemp.size(); idx++)
		{
			if (bFirst)
			{
				pTemp = _find_create(m_msgDetail->attribValue, vectemp[idx]);
				bFirst = false;
			}
			else
			{
				pTemp = _find_create(pTemp->subAttribute, vectemp[idx]);
			}

// 			if (bFirst)
// 			{
// 				T_AttribValueMap::iterator itFind = m_msgDetail->attribValue.find(vectemp[idx]);
// 				if (itFind != m_msgDetail->attribValue.end())
// 				{
// 					pTemp = itFind->second;
// 					pTemp->value = value;
// 				}	
// 				else 
// 				{
// 					if (vectemp.size() == 1)
// 					{ // name not found so add the attribute
// 						boost::shared_ptr<Attribute> pNew(new Attribute);
// 						pNew->name = vectemp[idx];
// 						pNew->value = value;
// 						m_msgDetail->attribValue[pNew->name] = pNew;
// 					}
// 					else
// 						bRetVal = false;
// 					break;
// 				}
// 				bFirst = false;
// 			}
// 			else
// 			{
// 				T_AttribValueMap::iterator itFind = pTemp->subAttribute.find(vectemp[idx]);
// 				if (itFind != pTemp->subAttribute.end())
// 				{
// 					pTemp = itFind->second;
// 					pTemp->value = value;
// 				}
// 				else 
// 				{
// 					if (idx == (vectemp.size()-1))
// 					{
// 						boost::shared_ptr<Attribute> pNew(new Attribute);
// 						pNew->name = vectemp[idx];
// 						pNew->value = value;
// 						pTemp->subAttribute[pNew->name] = pNew;
// 					}
// 					else
// 						bRetVal = false;
// 					
// 					break;
// 				}
// 			}
		}

		if (pTemp.get() != 0)
		{
			pTemp->value = value;
			bRetVal = true;
		}

		FUNCTION_EXIT;
		return bRetVal;
	}

	std::string GENATopicData::getDataStream()
	{
		return XmlUtility::detailToXml(*(m_msgDetail.get()));
	}

	boost::shared_ptr<Attribute> GENATopicData::_find_create(T_AttribValueMap& attributeMap, const std::string& attributeName)
	{
		FUNCTION_ENTRY("_find_create");
		boost::shared_ptr<Attribute> pRetVal;
		std::string tmp;
		tmp = attributeName;
		boost::to_upper(tmp);
		T_AttribValueMap::iterator itFind = attributeMap.find(tmp);
		if (itFind != attributeMap.end())
		{
			pRetVal = itFind->second;
		}
		else
		{
			pRetVal = boost::shared_ptr<Attribute>(new Attribute());
			pRetVal->name = tmp;
			// Add it to the map
			attributeMap.insert(T_AttribValueMap::value_type(tmp, pRetVal));
		}

		FUNCTION_EXIT;
		return pRetVal;
	}

	//boost::shared_ptr<Attribute> GENATopicData::_create(T_AttribValueMap& attributeMap, const std::string& attributeName)
	//{
	//	FUNCTION_ENTRY("_find_create");
	//	boost::shared_ptr<Attribute> pRetVal;
	//	
	//	pRetVal = boost::shared_ptr<Attribute>(new Attribute());
	//	pRetVal->name = attributeName;
	//	// Add it to the map
	//	attributeMap[attributeName] = pRetVal;
	//	
	//	FUNCTION_EXIT;
	//	return pRetVal;
	//}

} //TA_Base_Bus
