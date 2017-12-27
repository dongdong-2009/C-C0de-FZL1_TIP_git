/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: GENARequest.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This class contains the interface for processing the request.
  */

#ifndef __GENA_REQUEST_H_INCLUDED__
#define __GENA_REQUEST_H_INCLUDED__

#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/HTTPRequest.h"
#include "boost/shared_ptr.hpp"
#include <string>

using Poco::Net::HTTPRequest;

namespace TA_Base_Bus
{
	class GENARequest
	{
	public:
		/*
		* Constructor
		*/
		GENARequest(const std::string& method) 
			: m_method(method)
		{};

		/*
		* Constructor
		*/
		GENARequest(const std::string& method, const std::string& topicURI)
			: m_method(method), m_topicURI(topicURI)
		{}

		/*
		* Virtual Destructor
		*/
		virtual ~GENARequest() {};

		/*
		* getMethod
		* @purpose: retrieves the type of Method
		*
		* @return: the method otherwise it returns empty
		*/
		std::string getMethod() { return m_method; };		
		
		/*
		* setMethod
		* @purpose: set the type of Method
		*
		* @return: none
		*/
		void setMethod(const std::string& method) { m_method = method; };



		std::string getRealMethod() { return m_realMethod; };
		
		/*
		* setRealMethod
		* @purpose: set real method for identify renew-subscriber/subscriber
		*
		* @return: none
		*/
	    void setRealMethod(const std::string& realMethod) { m_realMethod=realMethod; };

		/*
		* setTopicURI
		* @purpose: set the URI of topic
		*
		* @return: none
		*/
		void setTopicURI(const std::string& topicURI) { m_topicURI = topicURI; };

		/*
		* getTopicURI
		* @purpose: retrieves the URI of topic
		*
		* @return: the method otherwise it returns empty
		*/
		std::string getTopicURI() { return m_topicURI; };
		
		/*
		* getMethod
		* @purpose: retrieves the host information
		*
		* @return: the host data otherwise it returns empty
		*/
		std::string getHost() { return m_host; };

		/*
		* setHost
		* @purpose: sets the host information
		*
		* @return: none
		*/
		void setHost(const std::string& host) { m_host = host; };

		/*
		* setRequestContent
		* @purpose: Constructs the HTTP Request and returns the 
		*
		* @return: an instance of the HTTP Request
		*/
		void setContent(const std::string& requestContent) { m_dataStream = requestContent; }

		/*
		* getRequestContent
		* @purpose: retrieves the content data
		*
		* @return: returns the content data
		*/
		std::string& getContent() { return m_dataStream; };
	
		/*
		* generateHttpRequest
		* @purpose: Constructs the HTTP Request based on the available data
		*
		* @return: an instance of the HTTP Request
		*/
		HTTPRequest* generateHttpRequest() 
		{
			// Create the request
			Poco::Net::HTTPRequest* objRetVal = new Poco::Net::HTTPRequest(m_method, m_topicURI, Poco::Net::HTTPMessage::HTTP_1_1);
			Poco::Net::NameValueCollection::ConstIterator itr = m_msgHeader.begin();
			objRetVal->setKeepAlive(true);
			for (itr; itr!=m_msgHeader.end(); itr++)
			{
				// Add custom header
				objRetVal->add(itr->first, itr->second);
			}             			 
			return objRetVal;
		}

		/*
		* getHeaderValue
		* @purpose: retrieves the header value or the given header name
		*
		* @return: the header value otherwise an empty string
		*/
		std::string getHeaderValue(const std::string& name)
		{
			Poco::Net::NameValueCollection::ConstIterator itr = m_msgHeader.find(name);
			if (itr != m_msgHeader.end())
			{
				return m_msgHeader[name];
			}
			return "";
		}

		/*
		* setRequestHeader
		* @purpose: Replace the existing header information in the request effectively 
		*			overriding all header information
		*
		* @return: none
		*/
		void setRequestHeader(Poco::Net::MessageHeader msgHeader)
		{
			m_msgHeader = msgHeader;
		}

	protected:
		void setHeader(const std::string& name, const std::string& value)
		{	
			if (m_msgHeader.has(name))
				m_msgHeader.set(name, value);
			else
				m_msgHeader.add(name, value);
		}

	private:
		// Contains the type of request
		std::string m_method; // Notify / Subscribe / Unsubscribe

		std::string m_realMethod; // Notify / Subscribe / Unsubscribe

		// Topic URI
		std::string m_topicURI;

		// Host
		std::string m_host;

		// Stream data // xml data stream
		std::string m_dataStream;

		// Contains message Header Information
		Poco::Net::MessageHeader m_msgHeader;
	};

	typedef boost::shared_ptr<GENARequest> GENARequestPtr;
}

#endif //__GENA_REQUEST_H_INCLUDED__