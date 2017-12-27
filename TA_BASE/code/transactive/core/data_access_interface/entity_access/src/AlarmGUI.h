/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmGUI.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifndef ALARM_GUI_ENTITY_DATA
#define ALARM_GUI_ENTITY_DATA
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
        class AlarmGUI : public EntityData
    {
public:
/**
 * implement methods as required by the interfaces from
 * which this class inherits
 */
		AlarmGUI();
        AlarmGUI( unsigned long key );
        virtual ~AlarmGUI();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);

		//virtual std::string getSoundFilePath();
		virtual bool getWindowCloseOption();
		virtual std::string getExportAlarmDirectory();
		//TD17888 marc_bugfix
		virtual std::string getExportDirectory();

protected:

private:
        // 
        // The entity type is fixed for all instances of this class.
        // 

        static const std::string ENTITY_TYPE;
		static const std::string m_alarmCloseWindowParameter;
		static const std::string m_exportAlarmDirPathParameter;
		//TD17888 marc_bugfix
		static const std::string m_exportDirectoryParameter;
    };

    typedef boost::shared_ptr<AlarmGUI> AlarmGUIPtr;
} //close namespace TA_Base_Core 
#endif // ALARM_GUI_ENTITY_DATA
