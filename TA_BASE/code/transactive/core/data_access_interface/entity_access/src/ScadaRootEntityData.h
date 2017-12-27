#ifndef SCADAROOTENTITYDATA_H_INCLUDED
#define SCADAROOTENTITYDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/ScadaRootEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// ScadaRootEntityData.h: interface for the ScadaRootEntityData class.
//
//////////////////////////////////////////////////////////////////////

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

	class ScadaRootEntityData : public EntityData 
	{
	public:
		ScadaRootEntityData();
		ScadaRootEntityData( unsigned long key );
		virtual ~ScadaRootEntityData();

		virtual void invalidate();
		virtual std::string getType();
		static std::string getStaticType();
		static IEntityData* clone(unsigned long key);

		virtual bool getAckCloseAlarmOnInhibit();
		virtual bool getAckAnalogAlarmOnTransition(); //TD13619

	private:       
		    ScadaRootEntityData( const ScadaRootEntityData&);
		    ScadaRootEntityData& operator=(const ScadaRootEntityData&);
            // 
            // The entity type is fixed for all instances of this class.
            //
            static const std::string ENTITY_TYPE;
            static const std::string ACK_CLOSE_ALARM_ON_INHIBIT;
		static const std::string ACK_ANALOG_ALARM_ON_TRANSITION; //TD13619
	};
    
    typedef boost::shared_ptr<ScadaRootEntityData> ScadaRootEntityDataPtr;
}

#endif // SCADAROOTENTITYDATA_H_INCLUDED

