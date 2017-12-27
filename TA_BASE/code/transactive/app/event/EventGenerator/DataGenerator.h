#pragma once

#include "core/event/src/EventItem.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/message/types/MessageTypes.h"
#include "core\message\src\NameValuePair.h"
#include "core\message\types\DataPointAlarms_MessageTypes.h"

#include <vector>
#include <string>

class CDataGenerator
{
	typedef std::vector<std::string> param_vec;
	typedef param_vec* param_vec_ptr;
	typedef TA_Base_Core::MessageType mt;
	typedef mt* mt_ptr;
	typedef struct s_msg
	{
		param_vec_ptr p_param;
		mt_ptr	p_mt;
	}* msg_ptr;
	typedef struct s_entity
	{
		std::string name;
		unsigned long subsys;
		unsigned long entity_key;
		unsigned long entity_type;
		unsigned long location;

	}* entity_ptr;
	typedef std::vector<msg_ptr> msg_vec;
	typedef std::vector<entity_ptr> entity_vec;

public:
	~CDataGenerator(void);

	static CDataGenerator& getInstance();
	static void releaseInstance();	
	void initialize(std::vector<std::string>& vi);
	TA_Base_Core::EventItem* createRandomEvent();
	void setSessionID(std::string strSessionID);
	void setEventType(std::string strEventType);
	void setPrefix(std::string strPrefix);

	std::string getSessionID();
	void populateEventTypes(CComboBox& cboEventTypes);
	void resetSeqNumber();
private:
	CDataGenerator(void);
	param_vec_ptr getParams(const std::string& paramstr);
	std::string getValueFromParameters(const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& description);
	std::string _formatTime(long ulTime);
	unsigned long getCurSeqNumber();

	static CDataGenerator* m_pInstance;
	static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
	TA_Base_Core::NonReEntrantThreadLockable m_threadLock;

	typedef std::map<std::string, unsigned long> SeverityMap;
	SeverityMap m_severities;
	msg_vec m_autoalarm;
	entity_vec m_allentity;

	unsigned long m_ulEventSize;
	unsigned long m_ulEntitySize;

	std::string m_strSessionID;
	int m_nEventTypeIdx;
	std::string m_strPrefix;

	unsigned long m_ulCurrSeqNum;
};
