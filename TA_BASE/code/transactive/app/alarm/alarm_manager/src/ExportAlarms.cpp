/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "stdafx.h"
#include "app/alarm/alarm_manager/src/ExportAlarms.h"
#include "app/alarm/alarm_manager/src/globals.h"
#include "bus/mfc_extensions/src/file_dialog_no_folder_change/FileDialogArialDisableFolders.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
const std::string CExportAlarms::EXPORTALARMS_LOCALPATH = "c:\\";

BOOL CExportAlarms::InitInstance()
{
	return TRUE;
}

int CExportAlarms::ExitInstance()
{
	return CWinThread::ExitInstance();
}

IMPLEMENT_DYNCREATE(CExportAlarms, CWinThread)
BEGIN_MESSAGE_MAP(CExportAlarms, CWinThread)
	//{{AFX_MSG_MAP(CExportAlarms)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(START_EXPORT,saveNow)
END_MESSAGE_MAP()

void CExportAlarms::saveNow(WPARAM wParam, LPARAM lParam)
{
	t_ParamsToReceive* params = (t_ParamsToReceive*)(lParam);

	if (params == NULL)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,"Could not export. Data passed to export thread was null.");
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040030);
		return;
	}

    m_alarmList = params->alarmList;

    // ++ AZ: TD13826 - wrong default path
    if (params->defaultExportPath == "")
    {
        // default to desktop instead of my documents folder
        TCHAR buff[MAX_PATH];
        //if (SHGetSpecialFolderPath(NULL, buff, CSIDL_DESKTOPDIRECTORY, FALSE))
		//TD17888 marc_bugfix
		if (SHGetSpecialFolderPath(NULL, buff, CSIDL_PERSONAL, FALSE))
        {
            m_defaultExportPath = buff;
        }
    }
    else
    {
        m_defaultExportPath = params->defaultExportPath;
    }
    // AZ: TD13826 ++
	m_imageStringVector = params->pImageStringVector;
	delete params;
    this->OnSave();

	AfxEndThread(0,TRUE);
}

void CExportAlarms::OnSave()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"Export was selected");

	// Allocate storage for szSelections

	CString fileExtension = ".csv";
	CString fileName = "未命名";
	// Show print dialog for user to choose a file name
	TA_Base_Bus::FileDialogArialDisableFolders dlg(FALSE,            // We want a Save dialog not an Open one
					fileExtension,           // The default file extension
					fileName,       // This is the initial path and file name
					OFN_HIDEREADONLY | // Hide the readonly check box
					OFN_OVERWRITEPROMPT | // Prompt to ask the user if they want to overwrite existing files
					OFN_PATHMUSTEXIST | // Check that the path entered is correct and warn if it is not
					OFN_NOCHANGEDIR,
					"CSV (用逗号分开的) (*.csv)|*.csv||",	// Only allow them to save in csv format
					AfxGetMainWnd());			  // Parent window is the actual alarm viewer
	
	// Open the file read-only. Create and set size to 0. Will overwrite existing files.
	CString pathName =  m_defaultExportPath+fileName;
	if (pathName.IsEmpty()) pathName = EXPORTALARMS_LOCALPATH.c_str();
	CFile outFile;
	// Check the path and file name given for availability
	if ( 0 == outFile.Open(pathName, CFile::modeCreate | CFile::modeReadWrite) )
	{
        TCHAR buff[MAX_PATH];
		//The path and file name is not valid or available
		//Display a message that it is invalid and set the path to local drive
		std::string msg = dlg.GetPathName() + ".\n";
		//msg += "Set and save to Local Directory";
		msg += "设置并保存为本地路径";
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << std::string( msg );
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040034);

		//dlg.m_ofn.lpstrInitialDir = CString(EXPORTALARMS_LOCALPATH.c_str());

        //if (SHGetSpecialFolderPath(NULL, buff, CSIDL_DESKTOPDIRECTORY, FALSE))
		//TD17888 marc_bugfix
		if (SHGetSpecialFolderPath(NULL, buff, CSIDL_PERSONAL, FALSE))
        {
            dlg.m_ofn.lpstrInitialDir = buff;
        }


	}
	else
	{
		dlg.m_ofn.lpstrInitialDir = m_defaultExportPath;
		// remove the file if it is created
		if (outFile != NULL) 
		{	
			outFile.Close();
			CFile::Remove(pathName);
		}
	}

	if (dlg.DoModal() != IDOK)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,"Export was canceled");
		return;
	}

	try
	{
		if ( outFile.Open(dlg.GetPathName(),CFile::modeCreate | CFile::modeWrite) )
		{

			// Variables used repeatedly to write each piece of information to the file.
			std::string textToWrite;

			// Output the date as a heading
			COleDateTime currTime = COleDateTime::GetCurrentTime();
			// TD12474 ++
		    //textToWrite = currTime.Format("%d %B %Y %H:%M");
 			TA_Base_Core::DateFormat dateFormat;
			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec1a);
			textToWrite = currTime.Format(dateFormatStr.c_str());
			// ++ TD12474 

			textToWrite += "\n\n";
			outFile.Write(textToWrite.c_str(),textToWrite.size());
			std::string headerToWrite;
			// Output the column headings to the file
			CHeaderCtrl* pHeaderCtrl = m_alarmList->GetHeaderCtrl();
			unsigned int colCount = pHeaderCtrl->GetItemCount();
			HDITEM HeaderItem; 
			HeaderItem.mask = HDI_TEXT;
			TCHAR  lpBuffer[256];
			HeaderItem.pszText = lpBuffer;
			HeaderItem.cchTextMax = 256;
			//memorise the comment column index
			unsigned int commentCol = 0;
			bool foundComment = false;

			for(unsigned int i=0; i<colCount; i++)
			{
				pHeaderCtrl->GetItem(i, &HeaderItem);
				std::string headerItem = HeaderItem.pszText;
				if (headerItem.compare("C") == 0) 
				{
					commentCol = i;
					foundComment = true;
				}

				headerToWrite += headerItem;

				if(i != colCount-1)
				{
					headerToWrite += ",";
				}
				else
				{
					headerToWrite += "\n";
				}
			}

			//textToWrite = "Ack,State,Severity,Entity,System,Region,Description,Time Logged,Time Created,Alarm Type,Comment\n";
			outFile.Write(headerToWrite.c_str(),headerToWrite.size());

			std::string text;
			for (int index = 0; index <  m_alarmList->GetItemCount(); ++index)
			{
				// Now build up line of information
				std::string alarmItem;
				for(unsigned int i=0; i<colCount; i++)
				{
					text = m_alarmList->GetItemText(index,i);
					if (text.empty() == true)
					{
						// check whether the column is image
						LVITEM item;
						item.mask = LVIF_IMAGE;
						item.iItem = index;
						item.iSubItem = i;
						if (m_alarmList->GetItem(&item))
						{
							if (item.iImage >= 0 && m_imageStringVector!=NULL && m_imageStringVector->size()>0 )
								text = (*m_imageStringVector)[item.iImage];
							else
								text = "-"; // unknown string
						}
						else
						{
							text = "-"; // unknown
						}
					}

					// if this is the comment col, get the alarm comment if any and append the text
					if (foundComment && i == commentCol && text.compare("C") == 0)
					{
						text = dynamic_cast<TA_Base_Bus::AlarmListCtrl*>(m_alarmList)->getAlarmComment(index);
					}

					if(!text.empty())
					{
						convertText(text);
					}
					alarmItem += text;
					
					if(i != colCount-1)
					{
						alarmItem += ",";
					}
					else
					{
						alarmItem += "\n";
					}
				}
				outFile.Write(alarmItem.c_str(),alarmItem.size());
			}

			outFile.Flush();
			outFile.Close();
		}
		else // the path and file are not valid
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << std::string( dlg.GetPathName() );
			UINT selectedButton = userMsg.showMsgBox(IDS_UI_040034);

			return;
		}
	}
	catch (CFileException)
	{
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << dlg.GetPathName();
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040034);

		return;
	}

	// Display success message box
	TA_Base_Bus::TransActiveMessage userMsg;
	UINT selectedButton = userMsg.showMsgBox(IDS_UI_040031);
}

void CExportAlarms::convertText(std::string& text)
{
	// First check for comma. If there is none then the string can remain the same
	if (std::string::npos == text.find(','))
	{
		return;
	}

	// Ok now we have established there is a comma we must change any existing double quotes to "".
	std::string::size_type pos = text.find('\"');
	while (pos != std::string::npos)
	{
		text.insert(pos,"\"");
		pos = text.find('\"',pos+2);
	}

	// Now add quotes at either end
	text.insert(0,"\"");
	text.insert(text.size(),"\"");
}

