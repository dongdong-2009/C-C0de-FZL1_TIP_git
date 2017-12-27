#ifndef InspectorPanelEntityData_H_INCLUDED
#define InspectorPanelEntityData_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/InspectorPanelEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// InspectorPanelEntityData.h: interface for the InspectorPanelEntityData class.
//
//////////////////////////////////////////////////////////////////////

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

	class InspectorPanelEntityData : public EntityData 
	{
	public:
		InspectorPanelEntityData();
		InspectorPanelEntityData( unsigned long key );
		virtual ~InspectorPanelEntityData();

		virtual void invalidate();
		virtual std::string getType();
		static std::string getStaticType();
		static IEntityData* clone(unsigned long key);

		virtual bool getShowQuality();
		virtual bool getShowTimestamp();
		virtual int getMaxInstances();

	private:
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string m_maxInstances;
		static const std::string m_showQuality;
		static const std::string m_showTimestamp;
	};
    
    typedef boost::shared_ptr<InspectorPanelEntityData> InspectorPanelEntityDataPtr;
}

#endif //InspectorPanelEntityData_H_INCLUDED

