

#ifndef __GENA_EVENT_H_INCLUDED__
#define __GENA_EVENT_H_INCLUDED__

#include "bus/protocol/Gena/src/GENARequest.h"
#include "bus/protocol/Gena/src/CommonDef.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	class GENAEvent
		: public GENARequest
	{
	public:
		/*
		* Constructor
		* @param: method - method type of this request
		*/
		GENAEvent(const std::string& method) 
			: GENARequest(method)
		{};
		
		/*
		* Constructor
		* @param: method - method type this request
				  host - host ip address
				  topic - topic related to this request
		*/
		GENAEvent(const std::string& method, const std::string topicURI)
			: GENARequest(method, "/") //According to ICD, TopicURL is not required.
		{}
		
		/*
		* Destructor
		*/
		virtual ~GENAEvent() {};
		
		/*
		* setNotifyType
		* @purpose: sets the object ID information
		*
		* @return: returns topic information if not set returns empty
		*/
		void setNotifyType(const std::string& notifyType) 
		{
			setHeader(HEAD_NT, notifyType);
		}
		std::string getNotifyType() { return getHeaderValue(HEAD_NT); }

		void setSID(std::string sid) 
		{ 
			 
			setHeader(HEAD_SID, sid);
		}

		std::string getSID() 
		{
			std::string temp = getHeaderValue(HEAD_TIMEOUT);
			return temp;
		}

	private: // Variables
		
	};

	class GENABirthEvent
		: public GENAEvent
	{
	public:
		/*
		* Constructor
		* @param: method - method type of this request
		*/
		GENABirthEvent() 
			: GENAEvent(REQ_NOTIFY)
		{
			setHeader(HEAD_NT, GENA_BIRTH);
		};
		
		/*
		* Constructor
		* @param: method - method type this request
				  host - host ip address
				  topic - topic related to this request
		*/
		GENABirthEvent(const std::string topicURI)
			: GENAEvent(REQ_NOTIFY, topicURI)
		{
			setHeader(HEAD_NT, GENA_BIRTH);
		}
		
		/*
		* Destructor
		*/
		virtual ~GENABirthEvent() {};
	protected:
	private:
	};

	class GENAUpdateEvent
		: public GENAEvent
	{
	public:
		/*
		* Constructor
		* @param: method - method type of this request
		*/
		GENAUpdateEvent() 
			: GENAEvent(REQ_NOTIFY)
		{
			setHeader(HEAD_NT, GENA_UPDATE);
		};
		
		/*
		* Constructor
		* @param: method - method type this request
				  host - host ip address
				  topic - topic related to this request
		*/
		GENAUpdateEvent(const std::string topicURI)
			: GENAEvent(REQ_NOTIFY, topicURI)
		{
			setHeader(HEAD_NT, GENA_UPDATE);
		}
		
		/*
		* Destructor
		*/
		virtual ~GENAUpdateEvent() {};
	protected:
	private:
	};

	class GENADeathEvent
		: public GENAEvent
	{
	public:
		/*
		* Constructor
		* @param: none
		*/
		GENADeathEvent() 
			: GENAEvent(REQ_NOTIFY)
		{
			setHeader(HEAD_NT, GENA_DEATH);
		};
		
		/*
		* Constructor
		* @param: method - method type this request
				  host - host ip address
				  topic - topic related to this request
		*/
		GENADeathEvent(const std::string topicURI)
			: GENAEvent(REQ_NOTIFY, topicURI)
		{
			setHeader(HEAD_NT, GENA_DEATH);
		}
		
		/*
		* Destructor
		*/
		virtual ~GENADeathEvent() {};
	protected:
	private:
	};

	typedef boost::shared_ptr<GENAEvent> GENAEventPtr;
}


#endif //__GENA_EVENT_H_INCLUDED__