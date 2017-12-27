// AlarmListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AlarmListCtrl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_PAGE_SIZE 20
/////////////////////////////////////////////////////////////////////////////
// CAlarmListCtrl

CAlarmListCtrl::CAlarmListCtrl() : m_iCurrentPage(0), m_iTotalPage(1), m_iPageSize(MAX_PAGE_SIZE)
{
	InitializeCriticalSection(&m_sync);
}

CAlarmListCtrl::~CAlarmListCtrl()
{
}


BEGIN_MESSAGE_MAP(CAlarmListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CAlarmListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(HDN_ITEMCLICK, OnItemclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmListCtrl message handlers

void CAlarmListCtrl::Initialize()
{
	
	InsertColumn(0, _T("Alarm ID"), LVCFMT_LEFT, 100);
	InsertColumn(1, _T("SourceTime"), LVCFMT_LEFT, 110);
	InsertColumn(2, _T("Asset"), LVCFMT_LEFT, 100);	
	InsertColumn(3, _T("Severity"), LVCFMT_LEFT, 40);
	InsertColumn(4, _T("Description"), LVCFMT_LEFT, 150);
	InsertColumn(5, _T("AlarmValue"), LVCFMT_LEFT,100);
	InsertColumn(6, _T("Location"), LVCFMT_LEFT, 50);	
	InsertColumn(7, _T("AlarmAcknowledgeBy"), LVCFMT_LEFT, 60);
	InsertColumn(8, _T("AckTime"), LVCFMT_LEFT, 110);
	InsertColumn(9, _T("MMSJob"), LVCFMT_LEFT, 110);

	InsertColumn(10, _T("AlarmType"), LVCFMT_LEFT, 100);
	InsertColumn(11, _T("SystemKey"), LVCFMT_LEFT, 100);
	InsertColumn(12, _T("SubSystemKey"), LVCFMT_LEFT, 100);
   
	

	ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	SetItemCount(m_iPageSize);
}

void CAlarmListCtrl::AddAlarm(AlarmDetail *pItem)
{
	EnterCriticalSection(&m_sync);

	if(pItem)
	{
		bool find=false;
		
	 
		ALARM_MAP::iterator iterMap=m_AlarmMap.find(pItem->alarmID);
        if (iterMap !=m_AlarmMap.end())
	    {
		    //find=true;
			ALARM_LIST::iterator iter=m_AlarmList.begin();
			for (iter;iter!=m_AlarmList.end();)
			{
				AlarmDetail *ItemPtr=*iter;
				if (ItemPtr->alarmID==pItem->alarmID)
				{
					find=true;
					ItemPtr->ackTime=pItem->ackTime;
					ItemPtr->alarmAcknowledgeBy=pItem->alarmAcknowledgeBy.c_str();
					ItemPtr->alarmDescription=pItem->alarmDescription.c_str();
					ItemPtr->alarmSeverity=pItem->alarmSeverity;
					ItemPtr->alarmValue=pItem->alarmValue.c_str();
					ItemPtr->closeTime=pItem->closeTime;
					ItemPtr->mmsState=pItem->mmsState;
					ItemPtr->state=pItem->state;				
					ItemPtr->locationId=pItem->locationId.c_str();
					if (pItem->state==AlarmClosed)
					{
						m_AlarmList.erase(iter);	
						m_AlarmMap.erase(ItemPtr->alarmID);
						delete ItemPtr;
						ItemPtr = NULL;
					}
					break;
				}
				else
				{
					iter++;
				}
			}
	    }



		if (!find)
		{
			m_AlarmList.push_back(pItem);
			m_AlarmMap.insert(ALARM_MAP::value_type(pItem->alarmID.c_str(),pItem->alarmID.c_str()));
			if(m_AlarmList.size() > (m_iTotalPage * m_iPageSize))
			{
				if(m_iCurrentPage == m_iTotalPage - 1)
				{
					m_iCurrentPage++;
				}

				m_iTotalPage++;
			}
		}
	}

	LeaveCriticalSection(&m_sync);
}
void CAlarmListCtrl::Clear()
{
	EnterCriticalSection(&m_sync);

 	for(int i = 0; i < m_AlarmList.size(); ++i)
 	{
 		delete m_AlarmList[i];
 		m_AlarmList[i] = NULL;
 	}

 	m_AlarmList.clear();
	m_AlarmMap.clear();

	m_iCurrentPage = 0;
	m_iTotalPage = 1;

	LeaveCriticalSection(&m_sync);	
}


const char* CAlarmListCtrl::timeFromater(unsigned long tm)
{
	CTime time(tm);	 

	m_formatStr= "%d/%m/%Y-%H:%M:%S";

	CString str = time.Format(m_formatStr.GetBuffer(0));
	
	strncpy(m_timebuf, str.GetBuffer(0), sizeof(m_timebuf) - 1);	 
	return m_timebuf;
}

void CAlarmListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	//Create a pointer to the item
	LV_ITEM* pItem= &(pDispInfo)->item;
	
	//Which item number?
	int itemid = pItem->iItem + m_iPageSize * m_iCurrentPage;

	int iCount = GetAlarmCount();
	if(itemid >= GetAlarmCount())
	{
		*pResult = 0;
		return;
	}

// 	InsertColumn(0, _T("Alarm ID"), LVCFMT_LEFT, 100);
// 	InsertColumn(1, _T("SourceTime"), LVCFMT_LEFT, 120);
// 	InsertColumn(2, _T("Asset"), LVCFMT_LEFT, 100);	
// 	InsertColumn(3, _T("Severity"), LVCFMT_LEFT, 50);
// 	InsertColumn(4, _T("Description"), LVCFMT_LEFT, 200);
// 	InsertColumn(5, _T("AlarmValue"), LVCFMT_LEFT,100);
// 	InsertColumn(6, _T("Location"), LVCFMT_LEFT, 150);	
// 	InsertColumn(7, _T("AlarmAcknowledgeBy"), LVCFMT_LEFT, 60);
// 	InsertColumn(8, _T("AckTime"), LVCFMT_LEFT, 120);
// 	InsertColumn(9, _T("CloseTime"), LVCFMT_LEFT, 120);
// 	
	//Do the list need text information?
	if (pItem->mask & LVIF_TEXT)
	{
		std::string text;
		//Which column?

 
		if(pItem->iSubItem == 0)  
		{
			text = m_AlarmList[itemid]->alarmID;
		}
		else if (pItem->iSubItem == 1)
		{
			text=timeFromater(m_AlarmList[itemid]->sourceTime.time);
		}
		else if (pItem->iSubItem == 2)
		{
			text = m_AlarmList[itemid]->assetName;
		}	
		else if (pItem->iSubItem == 3)
		{
			CString str;
			str.Format("%d", m_AlarmList[itemid]->alarmSeverity);
			text = str;
		}
		else if (pItem->iSubItem == 4)
		{
			//Text is slogan
			text = m_AlarmList[itemid]->alarmDescription;
		}
		else if (pItem->iSubItem == 5)
		{
			text = m_AlarmList[itemid]->alarmValue;
		}

		else if (pItem->iSubItem == 6)
		{
			text =getMappingStationCode( m_AlarmList[itemid]->locationId );
		}
		
		else if (pItem->iSubItem == 7)
		{
			//Text is slogan
			text = m_AlarmList[itemid]->alarmAcknowledgeBy;
		}
		else if (pItem->iSubItem == 8)
		{
			//Text is slogan
			//std::stringstream ss;			 
			//ss<<m_AlarmList[itemid]->ackTime;           
			if (m_AlarmList[itemid]->ackTime!=0)
			{		
			   text=timeFromater(m_AlarmList[itemid]->ackTime);
			}
			 
		}
		else if (pItem->iSubItem == 9)
		{
          if (m_AlarmList[itemid]->mmsState==MMS_NONE)
		  {
			text="NONE";
		  }
		  if (m_AlarmList[itemid]->mmsState==MMS_SEMI)
		  {
			  text="MMS_SEMI";
		  }
		  if (m_AlarmList[itemid]->mmsState==MMS_AUTO)
		  {
			  text="MMS_AUTO";
		  }
		  if (m_AlarmList[itemid]->mmsState==MMS_JR_SENT)
		  {
			  text="MMS_JR_SENT";
		  }
		  if (m_AlarmList[itemid]->mmsState==MMS_ALM_SENT)
		  {
			  text="MMS_ALM_SENT";
		  }
		 
		  if (m_AlarmList[itemid]->mmsState==MMS_INHIBIT)
		  {
			  text="MMS_INHIBIT";
		  }
		  if (m_AlarmList[itemid]->mmsState==MMS_FAILED)
		  {
			  text="MMS_FAILED";
		  }
	
			
			
			
			
		}
		else if (pItem->iSubItem == 10)
		{
			text = m_AlarmList[itemid]->alarmType;
		}
		else if (pItem->iSubItem == 11)
		{
			text = m_AlarmList[itemid]->systemkey;
		}
		else if (pItem->iSubItem == 12)
		{
			text = m_AlarmList[itemid]->subsytemkey;
		}
		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, text.c_str(), pItem->cchTextMax);
	}
	
	//Do the list need image information?
	if( pItem->mask & LVIF_IMAGE) 
	{

	}

	*pResult = 0;
}



void CAlarmListCtrl::OnItemclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
//	MessageBox("click item");
	*pResult = 0;
}

int CAlarmListCtrl::GetAlarmCount()
{
	return m_AlarmList.size();
}

int CAlarmListCtrl::GetCurrentPage()
{
	return m_iCurrentPage;
}

int CAlarmListCtrl::GetTotalPage()
{
	return m_iTotalPage;
}

BOOL CAlarmListCtrl::NextPage()
{

	if(m_iCurrentPage < m_iTotalPage - 1)
	{
		m_iCurrentPage++;
		return TRUE;
	}
	return FALSE;
}

std::string CAlarmListCtrl::getMappingStationCode(std::string& code)
{
	   if (code=="DT01")   return	"BKP";
		if (code=="DT02")   return	"SSW";
		if (code=="DT03")   return	"HVW";
		if (code=="DT05")   return	"BTW";
		if (code=="DT06")   return	"BLM";
		if (code=="DT07")   return	"SAV";
		if (code=="DT08")   return	"DCS";
		if (code=="DT09")   return	"BTN";
		if (code=="DT10")   return	"STV";
		if (code=="DT11")   return	"NEW";
		if (code=="DT12")   return	"LTI";
		if (code=="DT13")   return	"RCR";
		if (code=="DT14")   return	"BGS";
		if (code=="DT15")   return	"PMN";
		if (code=="DT16")   return	"BFT";
		if (code=="DT17")   return	"LMK";
		if (code=="DT18")   return	"CRS";
		if (code=="DT19")   return	"CNT";
		if (code=="DT20")   return	"RVV";
		if (code=="DT21")   return	"BCL";
		if (code=="DT22")   return	"ABT";
		if (code=="DT24")   return	"KLB";
		if (code=="DT25")   return	"MTR";
		if (code=="DT26")   return	"MPS";
		if (code=="DT27")   return	"UBI";
		if (code=="DT28")   return	"KKB";
		if (code=="DT29")   return	"BDT";
		if (code=="DT30")   return	"BDR";
		if (code=="DT31")   return	"TPW";
		if (code=="DT32")   return	"TPC";
		if (code=="DT33")   return	"TPE";
		if (code=="DT34")   return	"UPC";
		if (code=="DT35")   return	"XPO";
		return code;
}

BOOL CAlarmListCtrl::PrevPage()
{
	if(m_iCurrentPage > 0)
	{
		m_iCurrentPage--;
		return TRUE;
	}
	return FALSE;
}

AlarmDetail* CAlarmListCtrl::GetAlarmSelected()
{
	AlarmDetail* pAlarm = NULL;

	EnterCriticalSection(&m_sync);
	
	POSITION pos = GetFirstSelectedItemPosition();
	int iIndex = GetNextSelectedItem(pos);

	iIndex += m_iCurrentPage * m_iPageSize;

	if(iIndex < m_AlarmList.size())
	{
		pAlarm = new AlarmDetail;
		memcpy(pAlarm, m_AlarmList[iIndex], sizeof(AlarmDetail));
	}

	LeaveCriticalSection(&m_sync);	

	return pAlarm;
}
