// GenaServerTest.cpp : Defines the entry point for the console application.
//


#include "TestPublisher.h"
#include "bus/protocol/Gena/src/GenaServer.h"
#include "bus/protocol/Gena/src/CommonDef.h"
#include "bus/protocol/Gena/src/GENALibrary.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include <Poco/Util/ServerApplication.h>
#include <conio.h>

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/DOM/NodeList.h"
#include "GenaDataStore.h"
#include "bus/protocol/Gena/src/XmlUtility.h"

#define  RPARAM_SIMULATOR_DATA "SimulatorData"

using namespace Poco;
using namespace TA_Base_Bus;

void myNodeToAttrib(Poco::XML::Node* pNode, boost::shared_ptr<Attribute> objParentAttrib)
{	
	boost::shared_ptr<Attribute> pChild( new Attribute);
	pChild->name = pNode->nodeName();
	objParentAttrib->subAttribute.insert(T_AttribValueMap::value_type(pChild->name, pChild));
	if (!pNode->hasChildNodes())
	{
		pChild->value = pNode->nodeValue();
	}
	if (pNode->childNodes()->length() == 1 && pNode->firstChild()->nodeType() != Poco::XML::Node::ELEMENT_NODE)
	{
		pChild->value = pNode->firstChild()->nodeValue();
	}
// 	else if (pNode->childNodes()->length() == 1 
// 		&& pNode->childNodes()->item(0)->nodeName() == "#text")
// 	{
// 		pChild->value = pNode->nodeValue();
// 	}
	else
	{
		Poco::XML::Node* pTemp = pNode->firstChild();	
		while (pTemp)
		{
			if (pTemp->nodeName() != "#text")
				myNodeToAttrib(pTemp, pChild);

			pTemp = pTemp->nextSibling();
		}
	}

	
}

int main(int argc, char* argv[])
{
	std::vector<boost::shared_ptr<TestPublisher> > publishers;
	{
		struct App : Poco::Util::ServerApplication {
			~App() {
				waitForTerminationRequest();
			}
		} app;

		char LogPath[256];
		sprintf(LogPath, "GenaServerTesting.log");
		TA_Base_Core::DebugUtil::getInstance().setFile(LogPath);
		TA_Base_Core::DebugUtil::getInstance().setMaxSize(2000000);
		TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugDebug);

		TA_Base_Core::RunParams::getInstance().parseCmdLine(argc, argv);

		TA_Base_Core::CorbaUtil::getInstance().initialise(8888);
		TA_Base_Core::CorbaUtil::getInstance().activate();

		GenaDataStore* dataStore = new GenaDataStore();

		GENALibrary::getInstance().initialize();
		GENALibrary::getInstance().register_TopicSource(dataStore);
		GENALibrary::getInstance().startService();

		std::string strSimulatorData = "SimulatorData.xml";
		if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_SIMULATOR_DATA))
		{
			strSimulatorData = TA_Base_Core::RunParams::getInstance().get(RPARAM_SIMULATOR_DATA);
		}

		std::ifstream in(strSimulatorData.c_str());
		Poco::XML::InputSource src(in);

		XML::DOMParser parser;
		Poco::AutoPtr<XML::Document> pDoc = parser.parse(&src);

		XML::NodeIterator it(pDoc, Poco::XML::NodeFilter::SHOW_ELEMENT);
		XML::Node* pNode = it.root()->firstChild()->firstChild(); // Should get first topic
		T_GENATopicDataList objTopicDataList;

		while (pNode)
		{
			if (pNode->nodeName() == "Topic")
			{	// Configuration data
				GenaTestTopic objTestTopic;
				objTestTopic.topic = pNode->attributes()->getNamedItem("Name")->nodeValue();
				objTestTopic.topicURI = pNode->attributes()->getNamedItem("URL")->nodeValue();
				objTestTopic.itemCount = atoi(pNode->attributes()->getNamedItem("ItemCount")->nodeValue().c_str());
				objTestTopic.updateInterval = strtoul (pNode->attributes()->getNamedItem("Interval")->nodeValue().c_str(), NULL, 0);
				
				
				XML::Node* pTempNode = pNode->firstChild();
				while(pTempNode)
				{
					if (pTempNode->nodeName() == "Birth")
					{	
						std::string objectID = pTempNode->attributes()->getNamedItem("ObjectID")->nodeValue();
						GENATopicDataPtr ptestData(new GENATopicData(objectID, objTestTopic.topic));
						ptestData->setTopicURI(objTestTopic.topicURI);
						ptestData->setUpdateType(GENA_BIRTH);

						T_AttribValueMap objAttribMap;
						XML::Node* pNodeDetail = pTempNode->firstChild(); // Should get the first topic attribute
						while (pNodeDetail)
						{
							if (pNodeDetail->nodeName() == "#text")
							{
								pNodeDetail = pNodeDetail->nextSibling(); // why does it have garbage
								continue;
							}
							boost::shared_ptr<Attribute> pChild( new Attribute);
							pChild->name = pNodeDetail->nodeName();
							objAttribMap.insert(T_AttribValueMap::value_type(pChild->name, pChild));

							if (!pNodeDetail->hasChildNodes())
							{
								pChild->value = pNodeDetail->nodeValue();
							}
							else if (pNodeDetail->childNodes()->length() == 1 && pNodeDetail->firstChild()->nodeType() != Poco::XML::Node::ELEMENT_NODE)
							{
								pChild->value = pNodeDetail->firstChild()->nodeValue();
							}
							else
							{
								Poco::XML::Node* pTemp = pNodeDetail->firstChild();	
								while (pTemp)
								{
									if (pTemp->nodeName() != "#text")
										myNodeToAttrib(pTemp, pChild);

									pTemp = pTemp->nextSibling();
								}
							}
							pNodeDetail = pNodeDetail->nextSibling();
						}

						ptestData->setAttributes(objAttribMap);
						objTopicDataList.push_back(ptestData);						
						objTestTopic.births.push_back(ptestData);
						//idx++;
					}
					else if (pTempNode->nodeName() == "Update")
					{
						std::string objectID = pTempNode->attributes()->getNamedItem("ObjectID")->nodeValue();
						GENATopicDataPtr ptestData(new GENATopicData(objectID, objTestTopic.topic));
						ptestData->setTopicURI(objTestTopic.topicURI);
						ptestData->setUpdateType(GENA_UPDATE);

						T_AttribValueMap objAttribMap;
						XML::Node* pNodeDetail = pTempNode->firstChild(); // Should get the first topic attribute
						while (pNodeDetail)
						{
							if (pNodeDetail->nodeName() == "#text")
							{
								pNodeDetail = pNodeDetail->nextSibling(); // why does it have garbage
								continue;
							}
							boost::shared_ptr<Attribute> pChild( new Attribute);
							pChild->name = pNodeDetail->nodeName();
							objAttribMap.insert(T_AttribValueMap::value_type(pChild->name, pChild));
							
							if (!pNodeDetail->hasChildNodes())
							{
								pChild->value = pNodeDetail->nodeValue();
							}
							else if (pNodeDetail->childNodes()->length() == 1 && pNodeDetail->firstChild()->nodeType() != Poco::XML::Node::ELEMENT_NODE)
							{
								pChild->value = pNodeDetail->firstChild()->nodeValue();
							}
// 							else if (pNodeDetail->childNodes()->length() == 1 
// 								&& pNodeDetail->childNodes()->item(0)->nodeName() == "#text")
// 							{
// 								pChild->value = pNodeDetail->nodeValue();
// 							}
							else
							{
								Poco::XML::Node* pTemp = pNodeDetail->firstChild();	
								while (pTemp)
								{
									if (pTemp->nodeName() != "#text")
										myNodeToAttrib(pTemp, pChild);

									pTemp = pTemp->nextSibling();
								}
							}
							pNodeDetail = pNodeDetail->nextSibling();
						}

						ptestData->setAttributes(objAttribMap);
						objTestTopic.updates.push_back(ptestData);
					}
					else if (pTempNode->nodeName() == "Death")
					{
						std::string objectID = pTempNode->attributes()->getNamedItem("ObjectID")->nodeValue();
						GENATopicDataPtr ptestData(new GENATopicData(objectID, objTestTopic.topic));
						ptestData->setTopicURI(objTestTopic.topicURI);
						ptestData->setUpdateType(GENA_DEATH);

						objTestTopic.deaths.push_back(ptestData);
					}

					pTempNode = pTempNode->nextSibling();
				}
				
				dataStore->setTopicData(objTestTopic.topicURI, objTopicDataList);

				boost::shared_ptr<TestPublisher> objTrainPubTester (new TestPublisher(objTestTopic));
				objTrainPubTester->start();
			
				publishers.push_back(objTrainPubTester);
			}

			pNode = pNode->nextSibling();
		}

		// Input 
		printf("Switch server mode to active/passive \n");
		printf("a=active \n");
		printf("p=passive \n");
		char currOp = 'a';
		char op;		
		do 
		{
			op = std::getchar();
			if (op == currOp)
				continue;
			
			switch (op)
			{
			case 'a':
				GENALibrary::getInstance().startService();
				break;
			case 'p':
				GENALibrary::getInstance().stopService();
				break;
			}
			currOp = op;
		} while (op != 'x');
	}
	
	GENALibrary::getInstance().stopService();
	
}

