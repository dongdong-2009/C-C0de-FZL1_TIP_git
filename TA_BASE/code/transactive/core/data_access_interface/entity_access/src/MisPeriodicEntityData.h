/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisPeriodicEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MIS_PERIODIC_ENTITY_DATA_H_INCLUDED
#define MIS_PERIODIC_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MisPeriodicEntityData : public EntityData
    {

    public:
        MisPeriodicEntityData() {}
        MisPeriodicEntityData(unsigned long key);

        virtual ~MisPeriodicEntityData();

    public:

        /**
         * getScanPeriodMinutes
         *
         * @return the value of the SCAN_PERIOD_MIN param. This parameter indicates
		 * how often we should check to see if any periodic mantenance messages
		 * are due to be sent
         */
		unsigned int getScanPeriodMinutes();

        /**
         * getBackLogCheckPeriodHours
         *
         * @return the value of the BACKLOG_CHECK_PERIOD_HOURS param. 
		 * This parameter indicates how far back in time we should check to 
		 * see if need to send any maintenance messages we have missed 
		 * while the MMS Agent was down
         */
		unsigned int getBackLogCheckPeriodHours();

        // Raymond Pau++ TD16613
        std::string getLessServerFtpAddress( void );
        std::string getLessServerFtpUsername( void );
        std::string getLessServerFtpPassword( void );
        std::string getLessServerFtpReportFilename( void );
        std::string getLessServerFtpSyncTime( int& hour, int& minute );
        // ++Raymond Pau TD16613

		//xxs add
        std::string getScadaFileReportScheduleTime( int& hour, int& minute );
		//end xxs add

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new MisPeriodicEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the MisPeriodicEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        

    protected:

        /**
         * reload
         *
         * (Re)loads the parameter data from the database.
         *
         * @exception DatabaseException If there is a database error.
         * @exception DataException If a parameter value cannot be converted to the required
         *            type
         */
        void reload();

		//parse a string to hour and minute
		std::string parseTimeString( std::string & timeString, int & hour, int & minute );	
	private:

        MisPeriodicEntityData( const MisPeriodicEntityData&);            
		MisPeriodicEntityData& operator=(const MisPeriodicEntityData&);
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string SCAN_PERIOD_MIN;
		static const std::string BACKLOG_CHECK_PERIOD_HOURS;
        // Raymond Pau++ TD16613
        static const std::string LESS_SERVER_FTP_ADDRESS;
        static const std::string LESS_SERVER_FTP_USERNAME;
        static const std::string LESS_SERVER_FTP_PASSSWORD;
        static const std::string LESS_SERVER_FTP_REPORT_FILENAME;
        static const std::string LESS_SERVER_FTP_SYNCTIME;
        // ++Raymond Pau TD16613

		//xxs add
		static const std::string SCADA_FILE_RPORT_SCHEDULE_TIME;
		//end xxs add
    };
    
    typedef boost::shared_ptr<MisPeriodicEntityData> MisPeriodicEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MIS_PERIODIC_ENTITY_DATA_H_INCLUDED
