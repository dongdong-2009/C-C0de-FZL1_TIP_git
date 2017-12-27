/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventSerializeHelper.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_SERIALIZE_HELPER_H_INCLUDED__
#define __EVENT_SERIALIZE_HELPER_H_INCLUDED__

#include <stdio.h>
#include <vector>

namespace TA_Base_Core
{
	struct EvSzItemInfo
	{
		unsigned long ulTotalDatalen;

		unsigned long ulDataVersion;

		unsigned short ulAssetNamelen;

		unsigned short ulDescriptionlen;

		unsigned short ulEvTypeNamelen;

		unsigned short ulValuelen;

		unsigned short ulOperatorNamelen;

		unsigned short ulAlarmCommentlen;

		EvSzItemInfo() :
		ulTotalDatalen(0), ulDataVersion(0), ulAssetNamelen(0),
			ulDescriptionlen(0), ulEvTypeNamelen(0),  ulValuelen(0),
			ulOperatorNamelen(0), ulAlarmCommentlen(0) {}
	};

	typedef std::vector<EvSzItemInfo> T_EvSzItemInfoList;

	//List Header Info
	struct EvItemlistInfo
	{
		unsigned long ulListSize;

		unsigned long ulDataVersion;

		EvItemlistInfo() :
			ulListSize(0), ulDataVersion(0)
		{}
	};

	const int EV_SIZEOF_ITEM_INFO			= sizeof(EvSzItemInfo);
	const int EV_SIZEOF_ITEM_LIST_INFO		= sizeof(EvItemlistInfo);
}



#endif //__EVENT_SERIALIZE_HELPER_H_INCLUDED__