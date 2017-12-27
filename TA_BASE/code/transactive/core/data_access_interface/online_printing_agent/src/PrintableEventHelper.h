/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/PrintableEventHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * MmSchedulingHelper provides read/write support the the MM_SCHEDULING table.
  */


#ifndef PRINTABLE_EVENT_HELPER_H
#define PRINTABLE_EVENT_HELPER_H


#include <string>

namespace TA_Base_Core
{

    class PrintableEventHelper
    {

    public:


		PrintableEventHelper( unsigned long pkey,std::string printer,
                              std::string alarmflag, std::string createtime,
                              std::string severity, std::string value,
                              std::string asset,std::string description ,
                              std::string mms,std::string dss,
                              std::string avalanche,std::string operatorname,
                              std::string alarmcomment,std::string alm,
                              unsigned long locationkey);



 		PrintableEventHelper( const PrintableEventHelper& rhs );
		

  
        virtual ~PrintableEventHelper();



        /**
         * invalidate
         *
         * Make the data contained by this alarm plan association as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();



        virtual unsigned long getPkey();


 
        virtual std::string getPrinter();


    
        virtual std::string getAlarmflag();


        virtual std::string getCreatetime();


 
        virtual std::string getSeverity();


 
        virtual std::string getAsset();

    
 
        virtual std::string getDescription() ;


  
        virtual std::string getValue();


 
        virtual std::string getMMS();

  
        virtual std::string getDSS();

 
        virtual std::string getAvalanche();

  
        virtual std::string getOperatorname();


	    virtual std::string getAlarm_Comment();


	    virtual std::string getAlm();


	    virtual unsigned long getLocation_Key();


        virtual bool hasChanged();

  
        virtual void applyChanges();


        virtual void setPkey( unsigned long pkey);


  
        virtual void setPrinter( std::string printer);


 
        virtual void setAlarmflag( std::string alarmflag);


   
        virtual void setCreatetime( std::string createtime );


  
        virtual void setSeverity( std::string severity);


  
        virtual void setAsset( std::string asset);

    
   
        virtual void setDescription( std::string description);


   
        virtual void setValue( std::string value );


   
        virtual void setMMS( std::string mms);

    
        virtual void setDSS( std::string dss);

   
        virtual void setAvalanche( std::string avalanche);


	    virtual void setOperatorname( std::string operatorname);


    	virtual void setAlarm_Comment( std::string alarmcomment);


	    virtual void setAlm( std::string alm);

		
	    virtual void setLocation_Key( unsigned long locationkey);



    private:
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();


        /**
         * modifyExisting
         *
         * This will update an existing alarm plan association in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the alarm plan association object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExisting();

		//not implimented
        PrintableEventHelper& operator=(const PrintableEventHelper &);

        unsigned long  m_pkey;
        std::string m_printer;
        std::string m_alarmflag;
        std::string m_createtime;
        std::string m_severity;
        std::string m_asset;
        std::string m_description;
        std::string m_value;
        std::string m_mms;
        std::string m_dss;
        std::string m_avalanche;
	    std::string m_operatorname;
        std::string m_alarmcomment;
	    std::string m_alm;
	    unsigned long m_locationkey;


		//original data, prior to calling set methods
        unsigned long  m_pkeyOrig;
        std::string m_printerOrig;
        std::string m_alarmflagOrig;
        std::string m_createtimeOrig;
        std::string m_severityOrig;
        std::string m_assetOrig;
        std::string m_descriptionOrig;
        std::string m_valueOrig;
        std::string m_mmsOrig;
        std::string m_dssOrig;
        std::string m_avalancheOrig;
	    std::string m_operatornameOrig;
        std::string m_alarmcommentOrig;
	    std::string m_almOrig;
	    unsigned long m_locationkeyOrig;

        bool m_isValidData;
    };
} // closes TA_IRS_Core

#endif // PRINTABLE_EVENT_HELPER_H
