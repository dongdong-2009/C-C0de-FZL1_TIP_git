#include "StdAfx.h"
#include "message.h"

Message::Message(void)
: m_ulID(0), m_ulHelpID(0), m_ulMsgBoxStyle(16), m_usMessageType(InfoMessage), 
	m_bObsolete(false), m_bAuditEvent(false) 
{

}

Message::~Message(void)
{
	m_rgsParameters.RemoveAll();
	m_mapMessages.RemoveAll();
}

Message::Message(const Message& msg)
{
	LCID locale;
	CString sMessage;
	POSITION pos = msg.m_mapMessages.GetStartPosition();
	while(pos)
	{
		msg.m_mapMessages.GetNextAssoc(pos, locale, sMessage);
		m_mapMessages.SetAt(locale, sMessage);
	}

	m_ulID = msg.m_ulID;
    m_ResID = msg.m_ResID;
	m_ulHelpID = msg.m_ulHelpID;
	m_ulMsgBoxStyle = msg.m_ulMsgBoxStyle;
	m_usMessageType = msg.m_usMessageType;
	m_rgsParameters.Copy(msg.m_rgsParameters);
	m_bObsolete = msg.m_bObsolete;
	m_bAuditEvent = msg.m_bAuditEvent;
}
