/**
  * The source code in this file is the property of 
  * Combuilder and is not for redistribution
  * in any form.
  *
  * Source:   $File: IRequestListener.h $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/06/10 14:17:48 $
  * Last modified by:  $Noel R. Tajanlangit $
  * 
  * This file contains common structures, enumeration and constants
  */

#ifndef __COMMON_DEFINITION_H_INCLUDED__
#define __COMMON_DEFINITION_H_INCLUDED__

#include "Poco/Net/MessageHeader.h"
#include "boost/shared_ptr.hpp"
#include <string>
#include <map>
#include <vector>

using Poco::Net::MessageHeader;

#define RPARAM_CONFIG_FILE	"ConfigFile"

// Constant Request Types
#define	REQ_SUBSCRIBE		"SUBSCRIBE"
#define	REQ_UNSUBSCRIBE		"UNSUBSCRIBE"
#define	REQ_NOTIFY			"NOTIFY"

// Constant Header Names
#define	HEAD_HOST			"Host"
#define HEAD_SID			"SID"
#define HEAD_NT				"NT"
#define HEAD_TIMEOUT		"Timeout"
#define HEAD_CALLBACK		"Call-Back"
#define HEAD_CONTENT_LENGHT	"Content-Lenght"

// Constant Notification Types
#define GENA_BIRTH			"gena:birth"
#define GENA_DEATH			"gena:death"
#define GENA_UPDATE			"gena:update"

// String Delimiters
#define QUESTION_DELIMETER	"?"
#define AND_DELIMETER		"&"
#define SLASH_DELIMETER		"/"
#define ASSIGN_DELIMETER	"="

#define DEFAULT_RESUBSCIPTION 20000 // Millisecond precision (20 seconds)
#define DEFAULT_SUBSCRIPTION_TIMEOUT 60 // Second precision (20 seconds)
#define DEFAULT_RESOLVE_TIMEOUT 20000 // Millisecond precision (20 seconds)

// XML Constant
#define ROOT_ELEMENT "Envelope"
#define BODY_ELEMENT "Body"

#define CLS_ATTRIB	"cls"
#define OBJ_ATTRIB	"obj"

// XML Configuration Constants

#define ROOT_CONFIG				"Config"
#define SERVER_ELEM				"Server"
#define CLIENT_ELEM				"Client"
#define BRIDGE_ELEM				"Bridge"

#define NAME_ELEM				"Name"
#define IPADDRESS_ELEM			"IPAddress"
#define PORT_ELEM				"Port"
#define HTTP_KEEP_ALIVE_ELEM	"HTTPKeepAlive"
#define HTTP_TIMEOUT_ELEM		"HTTPTimeOut"
#define SUBSCRIPTION_TO_ELEM	"SubscriptionTO"
#define RESUBSCRIPTION_ELEM		"Resubscription"
#define CALLBACK_ELEM			"Callback"
#define SOURCE_SERVER_ELEM		"SourceServer"
#define CALLBACK_ELEM			"Callback"
#define SERVER_URL_ELEM			"ServerURL"
#define SUBSCRIBE_TOPIC_ELEM	"SubcribeTopic"
#define TOPIC_ELEM				"Topic"

#define ID_ATTR					"ID"
#define ENABLE_ATTR				"Enabled"
#define MAX_RETRY_ATTR			"MaxRetry"
#define TIMEOUT_ATTR			"TimeOut"
#define KEEPALIVE_ATTR			"KeepAlive"
#define NAME_ATTR				"Name"
#define URL_ATTR				"URL"
#define NOTIFICATION_ATTR		"Notification"


enum ServerState
{
	Active = 0,
	Passive,
	Disconnected,
	Unknown
};

enum ConnectionStatus
{
	StatusGood = 0,
	StatusBad,      //Single connection is in bad status 
	StatusBad_ALL,  //All connections are in bad status, include the notification one.
	                //Need update all data-points to bad quality status and the FEP status to disconnected status with good quality.
	StatusUnknown
};

enum GENARequestStatus
{
	GENA_OK = 1, // Same with HTTP_OK
	GENA_SID_NOT_FOUND,
	GENA_BAD_REQUEST,
	GENA_PASSIVE_MODE,
	GENA_NETWORK_FAILURE,
	GENA_UNKNOWN_ERROR
};

// Gena Server Params
struct GenaServerParams
{
	unsigned long entityKey;
	std::string serverName;
	std::string ipAddress;
	unsigned int serverPort;
	bool keepAlive;
	unsigned long timeout;
	unsigned long subscriptionTimeOut;

	GenaServerParams()
		: keepAlive(true)
		, timeout(60000) // 60 Seconds Default
		, subscriptionTimeOut(DEFAULT_SUBSCRIPTION_TIMEOUT)
	{}
};

typedef boost::shared_ptr<GenaServerParams> GenaServerParamsPtr;

// Gena Client Source Server
struct GenaClientSource
{
	std::string serverURL;
	
	int maxRetry;
	
	unsigned long timeout;
	
	bool keepAlive;
};

typedef boost::shared_ptr<GenaClientSource> GenaClientSourcePtr;

// Gena Client Params
struct GenaClientParams 
{
	// Agent Entity key
	unsigned long entityKey;

	// Resubscribe time Interval
	unsigned long nResubscribeInterval;

	//Server information: ex. 192.168.0.1:8080
	std::vector< GenaClientSourcePtr > sourceServers; 

	// Call back server information
	GenaServerParams callbackInfo;

	GenaClientParams()
		: nResubscribeInterval(DEFAULT_RESUBSCIPTION)
	{}

};

typedef boost::shared_ptr<GenaClientParams> GenaClientParamsPtr;

// Gena Bridge
struct GenaBridgeParams
{
	// Bridge Agent Entity Key
	unsigned long entityKey;

	// Server Parameters
	GenaServerParamsPtr serverParams;
	
	// Client Parameters
	GenaClientParamsPtr clientParams;
};

typedef boost::shared_ptr<GenaBridgeParams> GenaBridgeParamsPtr;

// Generic Request structure
// struct GenaRequest
// {
// 	// Contains the type of request
// 	std::string method;
// 
// 	// URI - For topic and filters
// 	std::string uri;
// 
// 	// Contains message Header Information
// 	MessageHeader msgHeader;
// 
// 	// Contains message body
// 	std::string msgContent;
// }; //GenaRequest

struct GenaResponse
{
	// Response status
	GENARequestStatus status;

	// Contains message Header Information
	MessageHeader msgHeader;

	// Contains message body
	std::string msgContent;
};

// Client Information structure
struct SubscriberInfo
{
	// Subscriber ID
	std::string subID;
	// Topic
	std::string topic;
	// Topic URI
	std::string topicURI;
	// Topic Filters
	std::string filters;
	// Target Server
	std::string host;
	// Gena Notification: Birth, Death and Notify
	std::string notification;
	// Callback Timeout
	std::string timeout;
	// Callback IP Address
	std::string callback;

	SubscriberInfo() 
		: subID("")
	{}

}; //SubscriberInfo

typedef boost::shared_ptr<SubscriberInfo> SubscriberInfoPtr;
typedef std::map<std::string, SubscriberInfoPtr > T_SubscriptionInfoMap;

struct TopicUpdate 
{
	// Topic URI
	std::string topicURI;		

	// birth,update,death
	std::string updateType;	

	// update content
	std::string msgContent;	
};

// Topic Filter Set
typedef std::map<std::string, std::string > T_TopicFilterSet;

struct Attribute; // Forward declaration

// Attribute shared pointer
typedef boost::shared_ptr<Attribute> AttributePtr;

// Map for attribute name and sub attribute
typedef std::multimap<std::string, AttributePtr > T_AttribValueMap;

// Attribute definition
struct Attribute
{
	// Attribute Name
	std::string name;
	// Attribute Value
	std::string value;
	// Contains Sub Attributes
	T_AttribValueMap subAttribute;
};

// Message
struct MessageDetail 
{
	std::string objID;

	std::string topicName;

	T_AttribValueMap attribValue;
};

// Client Topic
struct GenaClientTopic
{
	std::string topic;
	std::string topicUrl;
	std::string notification;
};


enum SubscriberStatus
{
	InitialSubscribe = 0,
	RenewSubscribe,
	UnSubscribe,
	StopSubscribe,
	ResetSubscribe
};

typedef std::vector<GenaClientTopic > T_ClientTopicList;

enum SyncState{SyncReq = 0, SyncBegin, SyncEnd};

struct TopicSycnInfo 
{
	std::string topicName;

	SyncState state;

	time_t lastBirthMsg;
};

typedef boost::shared_ptr<TopicSycnInfo> TopicSycnInfoPtr;
#endif //__COMMON_DEFINITION_H_INCLUDED__