/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/app/event/event_viewer/src/ActionCommand.h $
  * @author huirong.luo
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/09/15 8:54:40 $
  * Last modified by: xiangmei.lu
  * 
  * ActionCommand is a flag which can indicate whether an event notification is by a successful result or a failed one.
  *
  */

#if !defined(EA_BD19C618_114E_4423_B1EA_A5A7C2F0435F__INCLUDED_)
#define EA_BD19C618_114E_4423_B1EA_A5A7C2F0435F__INCLUDED_

namespace TA_Base_App
{
	enum ActionCommand
	{
		NoCommand,
		Reconnect
	};
};
#endif // !defined(EA_BD19C618_114E_4423_B1EA_A5A7C2F0435F__INCLUDED_)
