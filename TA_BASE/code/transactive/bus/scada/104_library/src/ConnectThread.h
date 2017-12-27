#ifndef __CONNECT_H__
#define __CONNECT_H__
#include "FepDataStruct.h"
#include "Threads.h"


using namespace esca;
/**
* class ConnectThread 
* used to deal with a connection by single thread
*
*/
class CAdaptee;
class ConnectThread : public CThread  
{
public:
	ConnectThread();
	virtual ~ConnectThread();
	
#ifdef WIN32
	//this mothod will be called when the thread starts
	virtual DWORD Main();
#else
	virtual b_ui64 Main();
#endif
	void CheckLink();


	//int setEvent(esca::CEvent *event);

	int setManager(CAdaptee *pManager);
	
	//this mothod will be called when the thread starts
	int run();

	//disconnect the socket,and reset the buffer.
	int reset(b_bool changeLink);
	
	void reconnectLink();
	
	CChannel * GetChannel();
public:
	CChannel *mChannel;
	CAdaptee *m_pManager;
private:
	bool m_resetCurrentLink;
};

#endif
