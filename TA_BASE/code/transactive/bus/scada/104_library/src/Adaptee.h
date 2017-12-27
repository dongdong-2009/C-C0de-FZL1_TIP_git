#ifndef __ADAPTEE_H__
#define __ADAPTEE_H__
//#include "Iec104Event.h"
#include "ConnectThread.h"
#include "FepDataStruct.h"
#include "ProtocolBase.h"
#include "Btype.h"
class IEC104_Event_Handler;
class esca::CEvent;
/**
 * @class CAdaptee
 *Play the role of adaptation and conversion
 * the main function of the oringinal system and the variables of the main function are packeted by the CLASS CAdaptee
 *
 */
class CAdaptee 
{
public:
	CAdaptee();
	
	~CAdaptee();
	
	void setWorkMode(int mode);
	
	int   reset();
	
	virtual int start_main(IEC104_Event_Handler* handler);
	
	int chooseWorkThread(const b_ui8 *data,b_ui8 len);
	int chooseConnectLinkInGrp(b_ui8 grpID);
	int chooseConnectLinkNotInGrp(b_ui8 grpID);
	void changeWorkThread(int newLinkID,int causeOfChange);
	int init();

	void killALLConnectThread();

	void resetLink();

	void PostMessage(b_ui32,void*);
	
private:
	void GetMessage(b_ui32&,void*&);
	
private:
	CChannel*m_ade_channel;
	ConnectThread* m_ade_conthr;
	b_ui8 m_workMode;
	b_ui8 m_currentWorkIndex;
	
	esca::CSemaphore m_messageSem;
	esca::CMutex m_messageMutex;
	std::queue<esca::CMessage> m_messages;
};

#endif
