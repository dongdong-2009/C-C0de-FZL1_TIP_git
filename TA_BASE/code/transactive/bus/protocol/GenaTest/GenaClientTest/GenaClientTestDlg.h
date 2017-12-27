// GenaClientTestDlg.h : header file
//

#pragma once

#include "../../Gena/src/GenaClient.h"
#include "../../Gena/src/IGENATopicDataStore.h"
#include "../cots/boost/boost_1_39_0/boost/shared_ptr.hpp"
#include <vector>
//#include "afxwin.h"

using namespace TA_Base_Bus;

// CGenaClientTestDlg dialog
class CGenaClientTestDlg : public CDialog, 
	public IGENATopicDataStore
{
// Construction
public:
	CGenaClientTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_GENACLIENTTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//virtual void onNotifyUpdate(const TopicUpdate& topicUpdate);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSubscribe();

private:
	GenaClient* m_pGenaClient;
	std::vector<unsigned long > m_clientSIDs;
public:
	CListBox resultsList;
	CString strTopic;
	CString strClientHost;
	CString strClientPort;
	afx_msg void OnBnClickedBtnUnsubscribe();

// 	virtual void onRecBirthMsg(boost::shared_ptr<GenaMessage> pGenaMgs);
// 	virtual void onRecUpdateMsg(boost::shared_ptr<GenaMessage> pGenaMgs);
// 	virtual void onRecDeathMsg(boost::shared_ptr<GenaMessage> pGenaMgs);

	virtual T_GENATopicDataList getAllData(const std::string& topicName );
	virtual void synchronizeBegin(const std::string& topicName);
	virtual void synchronizeEnd(const std::string& topicName);
	virtual void updateDataImpl(const std::string& topicName, GENATopicDataPtr pData);
	afx_msg void OnBnClickedOk();
};
