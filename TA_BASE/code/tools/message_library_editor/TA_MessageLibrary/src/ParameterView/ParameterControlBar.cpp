#include "..\stdAfx.h"

#include "..\TA_MessageLibrary.h"
#include "..\UserMessages.h"
#include "..\MainFrm.h"
#include "..\XMLObjects\XMLDefines.h"
#include "..\MessageGrid\MessageGridDoc.h"
#include "..\GlobalFunctions.h"

#include "ParameterControlBar.h"

//IMPLEMENT_DYNAMIC(ParameterControlBar, cdxCDynamicBarDlg);

ParameterControlBar::ParameterControlBar(CWnd* pParent)
	:cdxCDynamicBarDlg(ParameterControlBar::IDD, pParent)
{
}

ParameterControlBar::~ParameterControlBar(void)
{
}

BEGIN_MESSAGE_MAP(ParameterControlBar, cdxCDynamicBarDlg)
	//{{AFX_MSG_MAP(ParameterControlBar)
	ON_MESSAGE(UM_PARAM_REFRESH, OnRefreshParameters)
	ON_NOTIFY(SimpleBrowser::NavigateComplete2,IDC_PARAMCTRLBAR_BROWSER, OnNavigateComplete2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL ParameterControlBar::OnInitDialog() 
{
	cdxCDynamicBarDlg::OnInitDialog();

	m_browserCtrl.CreateFromControl(this, IDC_PARAMCTRLBAR_BROWSER);

	AddSzControl(m_browserCtrl,mdResize,mdResize);

	m_browserCtrl.NavigateResource(IDR_BASE_PARAMETER);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT ParameterControlBar::OnRefreshParameters(WPARAM wParam, LPARAM lParam)
{
	ULONG ulMessageID = (UINT)lParam;
	CString sParam, sDescription, sType, sQuery, sDescColour, sTemp, sHtml;
	
//	MainFrame* pMainFrame = (MainFrame*)AfxGetMainWnd();
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());
	MSXML2::IXMLDOMNodePtr spParam = NULL;
	MSXML2::IXMLDOMNodePtr spChild = NULL;
	_variant_t vtValue;

	MSHTML::IHTMLDocument3Ptr pIDoc = m_browserCtrl.GetDocument3();
	MSHTML::IHTMLElementPtr spParamList = pIDoc->getElementById(_T("ParamList"));

	if (spParamList == NULL)
		return 0;

	
	if(ulMessageID <= 0)
	{
		sHtml = _T("<STRONG><FONT face=\"Arial\" size=\"2\" color=\"#0000FF\">Message Parameters</FONT></STRONG><br><FONT face=\"Arial\" size=\"2\">Select a message from the grid to display its parameters</FONT>");
	}
	else
	{
		sQuery.Format(XPATH_MSG_PARAMS_USING_ID, ulMessageID);
		MSXML2::IXMLDOMNodeListPtr spParams = pDoc->m_messageXMLObj.m_spMessageRoot->selectNodes((LPCTSTR)sQuery);

		sHtml = (_T("<TABLE id=\"Table1\" cellSpacing=\"1\" cellPadding=\"1\" width=\"100%\" border=\"1\">")); //SL
		sHtml += _T("<TR><TD width=\"1\" bgColor=\"silver\" colSpan=\"1\" rowSpan=\"1\" style=\"WIDTH: 1px\"><STRONG><FONT face=\"Arial\" size=\"2\">Order</FONT></STRONG></TD>"); //SL
		sHtml += _T("<TD width=\"100%\" bgColor=\"silver\" colSpan=\"\" rowSpan=\"\"><STRONG><FONT face=\"Arial\" size=\"2\">Parameter</FONT></STRONG></TD></TR>"); //SL
		
		for(long l = 0, lCount = spParams->Getlength(); l < lCount; l++)
		{
			vtValue.Clear();
			spParam = spParams->Getitem(l);
			vtValue = spParam->GetnodeTypedValue();
			sParam = vtValue.bstrVal;

			sQuery.Format(XPATH_PARAM_USING_NAME, sParam);
			spParam = pDoc->m_messageXMLObj.m_spParamRoot->selectSingleNode((LPCTSTR)sQuery);

			if(spParam != NULL)
			{
				spChild = spParam->selectSingleNode(_T("@") XMLNODE_PARAM_TYPE); //SL
				vtValue.Clear();
				vtValue = spChild->GetnodeTypedValue();
				sType = vtValue.bstrVal;

				spChild = spParam->selectSingleNode(XMLNODE_PARAM_DESC);
				vtValue.Clear();
				vtValue = spChild->GetnodeTypedValue();
				sDescription = vtValue.bstrVal;

				sDescColour = _T("#0000ff"); //SL
			}
			else
			{
				sType.LoadString(IDS_PARAMS_UNKNOWN_TYPE);
				sDescription.LoadString(IDS_PARAMS_MISSING_PARAM);
				sDescColour = _T("#ff0000"); //SL
			}
			// add table row
			sTemp.Format(_T("<TR><TD width=\"1\" style=\"WIDTH: 1px\"><FONT face=\"Arial\" size=\"2\">{%d}</FONT></TD><TD width=\"183\"><P><FONT face=\"Arial\" size=\"2\">%s<BR>"), l, sParam); //SL
			sHtml += sTemp;
			sTemp.Format(_T("<FONT color=\"%s\" size=\"1\"><FONT color=\"#00cc00\">%s </FONT>%s</FONT></FONT></P></TD></TR>"), sDescColour, sType, sDescription); //SL
			sHtml += sTemp;
		}
		
		if(l == 0)
			sHtml += _T("<TR><TD width=\"1\" style=\"WIDTH: 1px\"><FONT face=\"Arial\" size=\"2\"></FONT></TD><TD width=\"183\"><P><FONT face=\"Arial\" size=\"2\">No parameters for this message</FONT></P></TD></TR>"); //SL

		sHtml += _T("</TABLE>");//SL
	}

	BSTR bstrTable = sHtml.AllocSysString();
	spParamList->put_innerHTML(bstrTable);
	SysFreeString(bstrTable);

	return 1;
}

void ParameterControlBar::OnNavigateComplete2(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnRefreshParameters(NULL, NULL);

	*pResult = 0;
}


