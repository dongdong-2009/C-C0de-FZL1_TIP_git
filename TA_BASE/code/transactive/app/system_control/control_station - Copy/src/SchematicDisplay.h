/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/SchematicDisplay.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// SchematicDisplay.h : Declaration of the CSchematicDisplay

#ifndef __SCHEMATICDISPLAY_H_
#define __SCHEMATICDISPLAY_H_

#include "app\system_control\control_station\src\resource.h"       // main symbols
#include "app\system_control\control_station\src\RippleControlStation_i.h"       
#include "app\system_control\control_station\src\RippleControlStationCP.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
// CSchematicDisplay
class ATL_NO_VTABLE CSchematicDisplay : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSchematicDisplay, &CLSID_SchematicDisplay>,
	public IConnectionPointContainerImpl<CSchematicDisplay>,
	public IDispatchImpl<ISchematicDisplay, &IID_ISchematicDisplay, &LIBID_RippleControlStationLib>,
	public CProxy_ISchematicDisplayEvents< CSchematicDisplay >
{
public:
	CSchematicDisplay();

DECLARE_REGISTRY_RESOURCEID(IDR_SCHEMATICDISPLAY)

DECLARE_PROTECT_FINAL_CONSTRUCT()

DECLARE_CLASSFACTORY_SINGLETON(CSchematicDisplay)

BEGIN_COM_MAP(CSchematicDisplay)
	COM_INTERFACE_ENTRY(ISchematicDisplay)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CSchematicDisplay)
CONNECTION_POINT_ENTRY(DIID__ISchematicDisplayEvents)
END_CONNECTION_POINT_MAP()


// ISchematicDisplay
public:
	STDMETHOD(getCurrentUserName)(/*[out, retval]*/ BSTR* userName);
	STDMETHOD(getCurrentLocation)(/*[out, retval]*/ BSTR* location);
	STDMETHOD(getCurrentProfile)(/*[out, retval]*/ BSTR* profile);
	STDMETHOD(getCurrentSessionId)(/*[out, retval]*/ BSTR* sessionId);
	STDMETHOD(displayAtsSchematic)(BSTR filePath, long leftPosition);

private:
    /** 
      * initialise
      *
      * Get the name of the schematic that will be used by the Ats
      *
      */
    void initialise();

    std::string m_templateName;
    static const std::string REPLACEMENT_STR;
};

#endif //__SCHEMATICDISPLAY_H_
