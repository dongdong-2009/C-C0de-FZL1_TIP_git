/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This class implements the sound interface declared in the IDL.
  * It receives requests from SoundManager clients and passes
  * them on to the CategoryManager class as necessary.
  *
  */

#ifndef SOUNDINTERFACE_H
#define SOUNDINTERFACE_H

#include "Resource.h"      
#include "bus/sound/sound_client/src/SoundManagerCP.h"

#include "CategoryManager.h"


class ATL_NO_VTABLE SoundInterface : 
	public CComObjectRootEx<CComMultiThreadModel>,
	public CComCoClass<SoundInterface, &CLSID_SoundInterface>,
	public IDispatchImpl<ISoundInterface, &IID_ISoundInterface, &LIBID_SOUNDMANAGERLib>,
	public CProxyISoundInterface< SoundInterface >,
	public IConnectionPointContainerImpl<SoundInterface>,
	public CProxy_ISoundInterfaceEvents< SoundInterface >,
	public ISupportErrorInfo
{

public:

	/**
	  * Constructor
	  */
	SoundInterface()
	{
	}


	DECLARE_REGISTRY_RESOURCEID(IDR_SOUNDINTERFACE)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(SoundInterface)
		COM_INTERFACE_ENTRY(ISoundInterface)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	END_COM_MAP()

    // ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	// ISoundInterface
	STDMETHOD(setPriority)(BSTR category, int priority);
	STDMETHOD(setSilence)(BSTR category, VARIANT_BOOL isSilenced);
	STDMETHOD(setSilenceAll)(VARIANT_BOOL isSilenced);
	STDMETHOD(joinCategories)(BSTR primaryCategory, BSTR secondaryCategory);
	STDMETHOD(isSilenced)(BSTR category, VARIANT_BOOL* isSilenced);
	STDMETHOD(isAllSilenced)(VARIANT_BOOL* isSilenced);

	// Set up the connection point stuff.
	BEGIN_CONNECTION_POINT_MAP(SoundInterface)
		CONNECTION_POINT_ENTRY(IID_ISoundInterface)
		CONNECTION_POINT_ENTRY(DIID__ISoundInterfaceEvents)
	END_CONNECTION_POINT_MAP()

	// Make the interface a singleton.
	DECLARE_NOT_AGGREGATABLE(SoundInterface)
	DECLARE_CLASSFACTORY_SINGLETON(SoundInterface)

private:

	/**
	  * reportError
	  *
	  * Creates an IErrorInfo object when an error occurs. Returns an HRESULT
	  * value that indicates the error.
	  *
	  * @param errorCode    An code indicating the type of error that occurred.
	  * @param errorString  A text description of the error.
	  *
	  * @return An HRESULT that can be passed to the client to indicate the
	  *		    error.
	  *
	  */
	STDMETHOD(reportError)(long errorCode, LPSTR errorString);

	/**
	  * The CategoryManager that will handle client requests.
	  */
	TA_Base_App::CategoryManager m_categoryManager;
};

#endif // SOUNDINTERFACE_H
