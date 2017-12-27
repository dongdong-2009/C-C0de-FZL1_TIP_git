/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_alias/src/ScadaAliasListColumns.cpp $
  * @author Wu Mintao
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Encapsulates the column specific information for records, to allow the ListView class
  * to be fully generic (interfacing with an arbitrary number of columns as defined in this class).
  * In this way the customisable elements of the list view are more centralised (separate)
  *
  */

#pragma warning (disable : 4284 4786)

#include "app/configuration/config_plugins/scada_alias/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasListColumns.h"
#include "core/utilities/src/TAAssert.h"

ScadaAliasListColumns::ScadaAliasListColumns()
{
    // Set up various column widths
    setColumnParams(COL_OPC_ALIAS,     120,     "OPC Alias",       COMPARE_STRING);
    setColumnParams(COL_ENTITY_NAME,    300,     "Entity Name",          COMPARE_STRING);
    setColumnParams(COL_ATTRIBUTE,       80,    "Attribute",        COMPARE_STRING);
}


const CString ScadaAliasListColumns::getFieldFromRecord(DWORD column, const TA_Base_App::ScadaAliasDatabaseAccessor::ScadaAliasRecord& details)
{
    switch (column)
    {
        case COL_OPC_ALIAS:    
            return details.opc_alias;
        case COL_ENTITY_NAME:           
            return details.entity_name;
        case COL_ATTRIBUTE:         
            return details.attribute;
        default:
            TA_ASSERT(FALSE, "getFieldFromRecord: Unrecognized column");
    }

    return _T("");
}

