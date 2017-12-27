/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: xmlUtility.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains the class declaration for ITopicListener
  */


#ifndef __XML_UTILITY_H_INCLUDED__
#define __XML_UTILITY_H_INCLUDED__

#include "CommonDef.h"
#include <string>

#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/AutoPtr.h" //typedef to Poco::AutoPtr
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/NamedNodeMap.h"

namespace TA_Base_Bus
{
	class XmlUtility
	{
	public:
		
		/*
		* detailToXml
		*/
		static std::string detailToXml(const MessageDetail& objDetail);

		/*
		* xmlToDetail
		*/
		static MessageDetail* xmlToDetail(const std::string& xmlData);

		/*
		* nodeToAttrib
		*/
		static void nodeToAttrib(Poco::XML::Node* pNode, boost::shared_ptr<Attribute> objParentAttrib);
	
		/*
		* attribToNode	
		*/
		static void attribToNode(Poco::XML::Document& doc, Poco::XML::Element& parentElem, boost::shared_ptr<Attribute > objAttrib);

	private:
		XmlUtility() {}
		XmlUtility& operator=(const XmlUtility& rhs);
		XmlUtility(const XmlUtility&);
	};

} //TA_Base_Bus

#endif //__XML_UTILITY_H_INCLUDED__