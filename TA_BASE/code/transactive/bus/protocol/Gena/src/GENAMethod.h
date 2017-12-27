

#ifndef __GENA_METHOD_H_INCLUDED__
#define __GENA_METHOD_H_INCLUDED__

#include "bus/protocol/Gena/src/GENARequest.h"
#include "bus/protocol/Gena/src/CommonDef.h"
#include <string>

namespace TA_Base_Bus
{
// 	class GENAMethod
// 		: public GENARequest
// 	{
// 	public: // Methods
// 		/*
// 		* Constructor
// 		* @param: method - method type of this request
// 		*/
// 		GENARequest(const std::string& method) 
// 			: GENARequest(method)
// 		{};
// 		
// 		/*
// 		* Constructor
// 		* @param: method - method type this request
// 				  host - host ip address
// 				  topic - topic related to this request
// 		*/
// 		GENARequest(const std::string& method, const std::string topicURI)
// 			: GENARequest(method, topicURI)
// 		{}
// 		
// 		/*
// 		* Destructor
// 		*/
// 		virtual void ~GENARequest() {};
// 
// 	private: // Variables
// 		// Topic name
// 		std::string m_topic;
// 	};


	class GENASubscribeMethod
		: public GENARequest
	{
	public:
		GENASubscribeMethod() 
			: GENARequest(REQ_SUBSCRIBE)
		{}

		GENASubscribeMethod(const std::string topicURI)
			: GENARequest(REQ_SUBSCRIBE, topicURI)
		{}

		~GENASubscribeMethod() {};

		void setTimeOut(const unsigned long ulTimeOut) 
		{ 
			char szTemp[20] = {0};
			sprintf(szTemp, "%d", ulTimeOut);
			setHeader(HEAD_TIMEOUT, szTemp);
		}
		unsigned long getTimeOut()
		{
			std::string temp = getHeaderValue(HEAD_TIMEOUT);
			if (temp.empty())
				return 0;
			
			return atoi(temp.c_str());
		}

		void setCallBackAddress(const std::string& callbackAddress) 
		{ 
			setHeader(HEAD_CALLBACK, callbackAddress); 
		}

		std::string getCallBackAddress() { return getHeaderValue(HEAD_CALLBACK); }

		void setNotifyType(const std::string& notifyType) 
		{
			setHeader(HEAD_NT, notifyType);
		}
		std::string getNotifyType() { return getHeaderValue(HEAD_NT); }

	private:
		
	};


	class GENAUnsubscribeMethod
		: public GENARequest
	{
	public:
		GENAUnsubscribeMethod() 
			: GENARequest(REQ_UNSUBSCRIBE)
		{};

		GENAUnsubscribeMethod(const std::string topicURI)
			: GENARequest(REQ_UNSUBSCRIBE, topicURI)
		{
			setTopicURI(topicURI);
		}

		virtual ~GENAUnsubscribeMethod() {}

		void setSID(std::string sid) 
		{ 
			//char szTemp[20] = {0};
			//sprintf(szTemp, "%d", sid);
			setHeader(HEAD_SID, sid);
		}

		std::string getSID() 
		{
			std::string temp = getHeaderValue(HEAD_SID);
			return temp;
		}

		void setNotifyType(const std::string& notifyType) 
		{
			setHeader(HEAD_NT, notifyType);
		}
		std::string getNotifyType() { return getHeaderValue(HEAD_NT); }
	private:
		
	};

	class GENARenewSubscribeMethod
		: public GENASubscribeMethod
	{
	public:
		GENARenewSubscribeMethod() 
			: GENASubscribeMethod(REQ_SUBSCRIBE)
		{
			setRealMethod("RenewSubscribe");
		};

		GENARenewSubscribeMethod(const std::string topicURI)
			: GENASubscribeMethod(REQ_SUBSCRIBE)
		{
			setTopicURI(topicURI);
			setRealMethod("RenewSubscribe");
		}

		virtual ~GENARenewSubscribeMethod() {}

		void setSID(std::string sid) 
		{ 
			//char szTemp[20] = {0};
			//sprintf(szTemp, "%d", sid);
			setHeader(HEAD_SID, sid);
		}

		std::string getSID() 
		{
			std::string temp = getHeaderValue(HEAD_SID);
			return temp;			 
		}
	
	private:
	
	};

	typedef boost::shared_ptr<GENASubscribeMethod> GENASubscribeMethodPtr;
	typedef boost::shared_ptr<GENAUnsubscribeMethod> GENAUnsubscribeMethodPtr;
	typedef boost::shared_ptr<GENARenewSubscribeMethod> GENARenewSubscribeMethodPtr;
}


#endif //__GENA_METHOD_H_INCLUDED__