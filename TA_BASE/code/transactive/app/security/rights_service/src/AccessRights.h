/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_service/src/AccessRights.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Implementation for COM interface that returns the access control decision
  */



#ifndef __ACCESSRIGHTS_H_
#define __ACCESSRIGHTS_H_

#include "app/security/rights_service/src/resource.h"       // main symbols

namespace TA_Base_Bus
{
	class RightsLibrary;
}


/////////////////////////////////////////////////////////////////////////////
// CAccessRights
class ATL_NO_VTABLE CAccessRights : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAccessRights, &CLSID_AccessRights>,
	public IDispatchImpl<IAccessRights, &IID_IAccessRights, &LIBID_RippleRightsLib>
{
public:
    /** 
      * CAccessRights
      *
      * Initialise the rights library.  
      *
	  * @exceptions RightsException Unable to initialise the rights library
      *             properly - could be because it couldn't contact the
      *             rights agent.
      */
	CAccessRights();

    /** 
      * ~CAccessRights
      *
      * Perform any necessary cleanup
      *
      */
    ~CAccessRights();


DECLARE_REGISTRY_RESOURCEID(IDR_ACCESSRIGHTS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

DECLARE_CLASSFACTORY_SINGLETON(CAccessRights)

BEGIN_COM_MAP(CAccessRights)
	COM_INTERFACE_ENTRY(IAccessRights)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IAccessRights
public:
	STDMETHOD(isActionPermittedOnResource)(BSTR sessionId , long resourceKey, long actionKey, /*[out, retval]*/ ResponseEnum* response);


    /** 
      * setDebugFile
      *
      * Sets the debug file and level
      *
      * @param file The path and name of the logs
      */
    static void setDebugFile(std::string file);

private:
    /** 
      * retrieveRunParams
      *
      * Retrieves the run params from the process manager.
      *
      */
    void retrieveRunParams();


    // Pointer to actual library that performs the access checks
    TA_Base_Bus::RightsLibrary* m_rightsMgr;

    // The error string returned by the rights library when the rights is not determinate
    static const char* RIGHTS_INDETERMINATE;

    // The error string returned by the rights library when the rights is not applicable
    static const char* RIGHTS_NOT_APPLICABLE;

    // The name of the log file
    static const char* LOG_FILE_NAME;

};

#endif //__ACCESSRIGHTS_H_
