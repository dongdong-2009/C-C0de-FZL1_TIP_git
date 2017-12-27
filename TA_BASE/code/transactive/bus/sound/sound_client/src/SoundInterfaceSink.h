/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/sound/sound_client/src/SoundInterfaceSink.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifndef SOUNDINTERFACESINK_H
#define SOUNDINTERFACESINK_H

#include <atlbase.h>
// CComModule _Module; // cancel for update VC6 to vs2005
#include <atlcom.h>
#include <afx.h>

#include "bus/sound/sound_client/src/SoundManager.h"
#include "bus/sound/sound_client/src/SoundManager_i.c"
#include "bus/sound/sound_client/src/ISoundObserver.h"


namespace TA_Base_Bus
{

	// Info for the events handlers.
	_ATL_FUNC_INFO onCategorySilencedInfo = {CC_STDCALL, VT_EMPTY, 2, {VT_BSTR, VT_BOOL}};
	_ATL_FUNC_INFO onAllSilencedInfo = {CC_STDCALL, VT_EMPTY, 1, {VT_BOOL}};


	// The class implementing the sink.
	class SoundInterfaceSink : public IDispEventSimpleImpl<1, SoundInterfaceSink, &DIID__ISoundInterfaceEvents>
	{
	public:

		SoundInterfaceSink(ISoundInterface* pSoundInterface, ISoundObserver* observer)
		{
			m_pSoundInterface = pSoundInterface;
			m_pSoundInterface->AddRef();
			// Attach to [default, source]
			DispEventAdvise((IUnknown*)m_pSoundInterface);
			
			m_observer = observer;
		}
		
		
		virtual ~SoundInterfaceSink()
		{
			// Detach from source.
			//m_pSoundInterface->Release();
			DispEventUnadvise((IUnknown*)m_pSoundInterface);
			m_pSoundInterface->Release();
		}
		
		
		// Event handlers
		virtual void __stdcall onCategorySilenced(BSTR category, VARIANT_BOOL isSilenced)
		{
			USES_CONVERSION;
			CString cat(category);
			
			if (m_observer != NULL)
			{
				m_observer->handleCategorySilenced(cat.GetBuffer(0), (isSilenced == VARIANT_TRUE));
			}
			
			SysFreeString(category);
		}
		

		virtual void __stdcall onAllSilenced(VARIANT_BOOL isSilenced)
		{
			USES_CONVERSION;
			if (m_observer != NULL)
			{
				m_observer->handleAllSilenced(isSilenced == VARIANT_TRUE);
			}
		}


		// IDispEventSimpleImpl<> needs a sink map
		BEGIN_SINK_MAP(SoundInterfaceSink)
			SINK_ENTRY_INFO(1, DIID__ISoundInterfaceEvents, 1, onCategorySilenced, &onCategorySilencedInfo)
			SINK_ENTRY_INFO(1, DIID__ISoundInterfaceEvents, 2, onAllSilenced,	   &onAllSilencedInfo)
		END_SINK_MAP()


	private:
		ISoundInterface* m_pSoundInterface;
		ISoundObserver* m_observer;
	};

} // namespace TA_Base_Bus


#endif // SOUNDINTERFACESINK_H
