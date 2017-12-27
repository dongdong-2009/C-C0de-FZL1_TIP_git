/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/scheduling_agent/src/ScheduleDataAccessFactory.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class creates objects that are used to read and change rows in the
  * Schedule database table.
  *
  */


#ifndef SCHEDULE_DATA_ACCESS_FACTORY_H
#define SCHEDULE_DATA_ACCESS_FACTORY_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/data_access_interface/scheduling_agent/src/IConfigScheduleData.h"

#include <vector>


namespace TA_Base_Core
{

class ScheduleDataAccessFactory
{

public:


    typedef std::vector< IConfigScheduleData* > IConfigScheduleDataVector;


    /**
     *  Gets the single instance of this class.
     */
    static ScheduleDataAccessFactory& getInstance();


    /**
     *  Gets a row from the scheduling table.
     *
     *  @param id  the unique ID of the row.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if the ID doesn't exist in the
     *  database.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    IConfigScheduleData* getSchedule(const std::string& id);


    /**
     *  Gets all rows from the scheduling table.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem retrieving
     *  values from the database.
     *
     *  @exception DataException
     *  A DataException will be thrown if the database contains invalid data.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    static IConfigScheduleDataVector getAllSchedules();

    
    /**
     *  Creates a new row in the scheduling table and returns an object
     *  representing that row.
     *
     *  The caller must delete the object returned.
     *
     *  @param schedule  the schedule whose fields will be copied into the row
     *                   created in the database.
     *  @param jobID     the ID of the job this schedule is for.
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem creating
     *  this Schedule in the database.
     *
     *  @exception BadScheduleException
     *             if any of the schedule's fields are out of range.
     */
    IConfigScheduleData* createSchedule( const std::string& jobID);

    /**
     *  Creates an empty scheduling object.
     */
    IConfigScheduleData* createSchedule( );

    /**
     *  Destructor.
     */
    virtual ~ScheduleDataAccessFactory();


private:

    ScheduleDataAccessFactory();
    ScheduleDataAccessFactory(const ScheduleDataAccessFactory&);
    const ScheduleDataAccessFactory& operator=(const ScheduleDataAccessFactory&);



    /** The single instance of this class */
    static ScheduleDataAccessFactory* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};


};// TA_Base_Core


#endif // SCHEDULE_DATA_ACCESS_FACTORY_H
