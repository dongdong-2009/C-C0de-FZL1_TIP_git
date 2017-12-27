

#include "core/utilities/src/DebugUtil.h"
#include "XmlUtility.h"
#include <sstream>
#include "Poco/DOM/Attr.h"
#include "Poco/DOM/ProcessingInstruction.h"
#include "Poco/DOM/NodeList.h"
#include "GENATopicData.h"

using namespace Poco::XML;


namespace TA_Base_Bus
{
	void XmlUtility::attribToNode(Document& doc, Element& parentElem, boost::shared_ptr<Attribute > objAttrib)
	{
		AutoPtr<Element> pChild = doc.createElement(objAttrib->name);
		if (objAttrib->subAttribute.size() == 0 )
		{	// Simpliest operation
			AutoPtr<Text> pText = doc.createTextNode(objAttrib->value);
			pChild->appendChild(pText);
		}
		else
		{	// Do the recursive crap
			T_AttribValueMap::iterator itr = objAttrib->subAttribute.begin();
			for (itr; itr!=objAttrib->subAttribute.end(); ++itr)
			{
				attribToNode(doc, *pChild.get(), itr->second);
			}
		}

		parentElem.appendChild(pChild);
	}

	/*
	* detailToXml
	*/
	std::string XmlUtility::detailToXml(const MessageDetail& objDetail)
	{
		AutoPtr<Document> pDoc = new Document;
		ProcessingInstruction* procInst = pDoc->createProcessingInstruction("xml", "version=\"1.0\"");
		pDoc->appendChild(procInst);

		AutoPtr<Element> pRoot = pDoc->createElement(ROOT_ELEMENT);
		pDoc->appendChild(pRoot);

		AutoPtr<Element> pBody = pDoc->createElement(BODY_ELEMENT);
		Attr* pAttrClass = pDoc->createAttribute(CLS_ATTRIB);
		Attr* pAttrObjID = pDoc->createAttribute(OBJ_ATTRIB);

		pAttrClass->setValue(objDetail.topicName);
		pAttrObjID->setValue(objDetail.objID);

		pBody->setAttributeNode(pAttrClass);
		pBody->setAttributeNode(pAttrObjID);

		pRoot->appendChild(pBody);
		T_AttribValueMap::const_iterator itr = objDetail.attribValue.begin();
		for (itr; itr!=objDetail.attribValue.end(); ++itr)
		{
			attribToNode(*pDoc.get(), *pBody.get(), itr->second);
		}

		std::stringstream strTemp;
		DOMWriter writer;
		writer.setNewLine("\n");
		writer.setOptions(XMLWriter::PRETTY_PRINT);
		writer.writeNode(strTemp, pDoc);

		return strTemp.str();
	}

	void XmlUtility::nodeToAttrib(Poco::XML::Node* pNode, boost::shared_ptr<Attribute> objParentAttrib)
	{
		boost::shared_ptr<Attribute> pChild( new Attribute);
		pChild->name = pNode->nodeName();
		objParentAttrib->subAttribute.insert(T_AttribValueMap::value_type(convertToUpperCase(pChild->name), pChild));
		
		if (!pNode->hasChildNodes())
		{
			pChild->value = convertToUpperCase(pNode->nodeValue());
		}
		/*else if (pNode->hasChildNodes() && pNode->firstChild()->nodeType() != Node::ELEMENT_NODE)
		{
			pChild->value = convertToUpperCase(pNode->firstChild()->nodeValue());
		}*/
		else
		{
			Poco::XML::Node* pTemp = pNode->firstChild();	
			while (pTemp)
			{
				if (pTemp->nodeName() != "#text")
					nodeToAttrib(pTemp, pChild);
				else
					pChild->value = convertToUpperCase(pTemp->nodeValue());

				pTemp = pTemp->nextSibling();
			}
		}
	}

	/*
	* xmlToDetail
	*/
	MessageDetail* XmlUtility::xmlToDetail(const std::string& xmlData)
	{
		FUNCTION_ENTRY("xmlToDetail");
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugMsg, "Raw XmlData : %s", xmlData.c_str());

		MessageDetail* pRetVal = new MessageDetail();
		Poco::XML::DOMParser parser;
		Poco::AutoPtr<Poco::XML::Document> pDoc = parser.parseString(xmlData); 

		Poco::XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ELEMENT);
		Poco::XML::Node* pNode =it.root()->getNodeByPath(ROOT_ELEMENT); // Envelope
		pNode = pNode->getNodeByPath(BODY_ELEMENT); // Body
		if (pNode->hasAttributes()) // should always be true
		{
			Poco::XML::NamedNodeMap* attributes = pNode->attributes();
			for(unsigned int i = 0; i < attributes->length(); i++)
			{
				Poco::XML::Node* attribute = attributes->item(i);
				if (attribute->nodeName() == CLS_ATTRIB)
				{
					pRetVal->topicName = convertToUpperCase(attribute->nodeValue());
				}
				else if (attribute->nodeName() == OBJ_ATTRIB)
				{
					pRetVal->objID = attribute->nodeValue();
				}
			}
		}
		pNode = pNode->firstChild(); // Should get the first topic attribute
		while (pNode)
		{
			if (pNode->nodeName() == "#text")
			{
				pNode = pNode->nextSibling(); 
				continue;
			}

			boost::shared_ptr<Attribute> pChild( new Attribute);
			pChild->name = pNode->nodeName();
			pRetVal->attribValue.insert(T_AttribValueMap::value_type(convertToUpperCase(pChild->name), pChild));
			if (!pNode->hasChildNodes())
			{
				pChild->value = convertToUpperCase(pNode->nodeValue());
			}
			else if (pNode->childNodes()->length() == 1 && pNode->firstChild()->nodeType() != Poco::XML::Node::ELEMENT_NODE)
			{
				pChild->value = convertToUpperCase(pNode->firstChild()->nodeValue());
			}
			else
			{
				Poco::XML::Node* pTemp = pNode->firstChild();	
				while (pTemp)
				{
					if (pTemp->nodeName() != "#text")
						nodeToAttrib(pTemp, pChild);

					pTemp = pTemp->nextSibling();
				}
			}
			pNode = pNode->nextSibling();
		}

		FUNCTION_EXIT;
		return pRetVal;
	}
}