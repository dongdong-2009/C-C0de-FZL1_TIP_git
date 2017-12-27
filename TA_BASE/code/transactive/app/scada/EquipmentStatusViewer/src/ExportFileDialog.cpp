/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ExportFileDialog.cpp $
 * @author:  Bart Golab
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Report export file save dialog. Forces the operator to save the file into 
 * the configured export directory.
 *
 */

#include "StdAfx.h"
#include "Resource.h"
#include "CustomDefines.h"
#include "ExportFileDialog.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CExportFileDialog

IMPLEMENT_DYNAMIC(CExportFileDialog, CFileDialogArial)

CExportFileDialog::CExportFileDialog() :
    CFileDialogArial(FALSE, ".csv", NULL, 
                     OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, 
                     "CSV (Comma delimited) (*.csv)|*.csv||"),
    m_exportDirectory("")
{
    // Validate the configured export directory.
    CString exportDir(TA_Base_Core::RunParams::getInstance().get(RPARAM_EXPORTDIR).c_str());

    if (exportDir.Right(1) == "/" || exportDir.Right(1) == "\\")
    {
        //  exportDir += "."; // FindFile() will need this  //  replaced 18/10/05
        exportDir = exportDir.Left( exportDir.GetLength()-1);
    }
    
    /*** TD 12051 18/10/05 alanb
         The following "directory-exists" test only works for local directories.
         The ESV GDN states that the CSV file is exported to a shared directory
         on the statistical PC that has an address such as \\sta-occ-1\ExportESV.

         This test can be replaced with shell function SHPathPrepareForWrite()
         which is good for networked or mounted directories ... but is not 
         accessible through the cots\VisualCPP header files.

    CFileFind exportDirSearch;

    if (exportDirSearch.FindFile(exportDir) == TRUE &&
        exportDirSearch.FindNextFile() == FALSE && 
        exportDirSearch.IsDirectory() == TRUE)
    {
        m_exportDirectory = exportDirSearch.GetFilePath();
    }

    exportDirSearch.Close(); 
    /***/

    m_exportDirectory = exportDir;

    // Initialise file dialog attributes.
	m_ofn.lpstrTitle = "Save Report As";
    m_ofn.lpstrInitialDir = m_exportDirectory;   
}


CExportFileDialog::~CExportFileDialog()
{
}


CString CExportFileDialog::getExportDirectory() const
{
    return m_exportDirectory;
}


BEGIN_MESSAGE_MAP(CExportFileDialog, CFileDialogArial)
	//{{AFX_MSG_MAP(CExportFileDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CExportFileDialog::OnFileNameOK()
{
    // Make sure the operator saves to the right target directory.
	if ( getExportDirectory().CompareNoCase( GetFolderPath()) != 0 )
	{   
        //if the directory is set to NULL in the configuration editor
        //it means the statis pc is down, op can export file to local directory.
        //otherwise, not allow to save at different location
        if (!getExportDirectory().IsEmpty()) 
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << m_exportDirectory;
            userMsg.showMsgBox(IDS_UE_083005);
        
            return TRUE;
        }
	}

    return CFileDialogArial::OnFileNameOK();
}
