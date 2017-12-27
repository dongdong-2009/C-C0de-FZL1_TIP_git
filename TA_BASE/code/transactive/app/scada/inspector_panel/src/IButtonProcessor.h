/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/inspector_panel/src/IButtonProcessor.h $
* @author:  Ripple
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/09/24 11:10:12 $
* Last modified by:  $Author: grace.koh $
*
*/
#ifndef ___IButtonProcessorh__
#define ___IButtonProcessorh__

#pragma warning(disable : 4786)

// This guard is so that this MFC extension can be used in another
// MFC extension DLL
//


//class IListCellButtonProcessor
class IButtonProcessor
{
public: 

	virtual ~IButtonProcessor() {}

	// Process the button click from the ListCellButton
	virtual void processButtonClick(int item, CString strValue) = 0;

};

#endif
