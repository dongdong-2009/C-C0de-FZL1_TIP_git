/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/DefaultRegionAssignmentMatrix.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/03/10 10:15:27 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class stores the static operator-region duty assignments 
  * as configured in the database.
  * The assignments are stored in the form of a two-dimensional matrix,
  * where rows represent operators and columns represent regions.
  * Individual elements of the matrix are boolean values indicating
  * whether the corresponding region is assigned to the corresponding
  * operator.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


DefaultRegionAssignmentMatrix::DefaultRegionAssignmentMatrix() :
    m_assignmentsLoaded(false)
{
    FUNCTION_ENTRY("DefaultRegionAssignmentMatrix()");
    FUNCTION_EXIT;
}


DefaultRegionAssignmentMatrix::~DefaultRegionAssignmentMatrix()
{
    FUNCTION_ENTRY("~DefaultRegionAssignmentMatrix()");
    FUNCTION_EXIT;
}


void DefaultRegionAssignmentMatrix::populate()
{
    FUNCTION_ENTRY("populate()");

    if (m_assignmentsLoaded)
    {
        // Already populated.
        FUNCTION_EXIT;
        return;
    }

    // Get configured regions.
    RegionKeyList regionKeys(DutyDataStore::getDataCache().getRegionKeys());

    // Insert the regions vertically into the matrix.
    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
        addColumn(*regionIt);
    }

    // Get configured operators.
    OperatorKeyList operatorKeys(DutyDataStore::getDataCache().getOperatorKeys());
    
    // Insert the operators horizontally into the matrix.
    for (OperatorKeyList::const_iterator operatorIt = operatorKeys.begin(); operatorIt != operatorKeys.end(); operatorIt++)
    {
        addRow(*operatorIt);

        // For the given operator get assigned regions.
        RegionKeyList assignedRegionKeys(DutyDataStore::getDataCache().getAssociatedRegionKeys(*operatorIt));

        // Update the appropriate elements of the matrix.
        for (RegionKeyList::const_iterator assignedRgnIt = assignedRegionKeys.begin(); assignedRgnIt != assignedRegionKeys.end(); assignedRgnIt++)
        {
            try
            {
                setElement(*operatorIt, *assignedRgnIt, true);
            }
            catch (const TA_Base_Core::MatrixException& ex)
            {
                //TA_THROW(DutyMonitorFrameworkException("Failed to set an element of the default region assignment matrix.", ex.what()));
				TA_THROW(DutyMonitorFrameworkException("权限管理器中位置权限矩阵的默认赋值失败", ex.what()));
            }
        }
    }

    m_assignmentsLoaded = true;

    FUNCTION_EXIT;
}
