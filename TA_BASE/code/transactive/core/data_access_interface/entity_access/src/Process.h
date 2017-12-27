/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Process.h $
  * @author Rob Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Process is a concrete implementation of IProcess, which is in turn an implementation of
  * IEntityData. It proves read-only access to Process entity data, and a specific framework
  * for Process entity parameters.
  *
  * 
  */

#if !defined(AFX_PROCESS_H__466F7C62_AD36_4CB6_B58A_44C5906CF2FF__INCLUDED_)
#define AFX_PROCESS_H__466F7C62_AD36_4CB6_B58A_44C5906CF2FF__INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IProcess.h"

namespace TA_Base_Core
{
    class Process : public IProcess, public EntityData
    {

    public:

		// A constant to indicate that the process should not be bound to a processor.
		static const int NOT_BOUND;

        /**
         * Process (constructor)
         *
         * Constructs a new Process object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        Process();

        /**
         * Process (constructor)
         *
         * Constructs a new Process object with the specified key.
         *
         * @param key The key to this Process in the database.
         */
        Process(unsigned long key);
        virtual ~Process();

    public:

		// Methods to fetch each entity parameter
        virtual std::string getUserName();
        virtual bool isManagedProcess();
        virtual bool isServiceProcess();
        virtual unsigned int getStartLag();
        virtual unsigned int getTerminateTimeout();
        virtual unsigned int getKillTimeout();
        virtual unsigned int getStartupTimeout();
        virtual unsigned int getChangeControlTimeout();
        virtual unsigned int getChangeMonitorTimeout();
        virtual bool shouldAutoFailback();
		virtual int getPriority();
		virtual int getBoundProcessor();

		virtual std::string getPassWord();

       /**
         * clone
         *
         * Returns a pointer to a new Process object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the Process object.
         */
        static IEntityData* clone(unsigned long key);

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Process").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

    private:
        Process( const Process& theProcess);            
		Process& operator=(const Process &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string USERNAME;
		static const std::string PASSWORD;
        static const std::string MANAGEDPROCESS;
        static const std::string SERVICEPROCESS;
        static const std::string STARTLAG;
        static const std::string TERMINATETIMEOUT;
        static const std::string KILLTIMEOUT;
        static const std::string AUTOFAILBACK;
        static const std::string STARTUPTIMEOUT;
        static const std::string CONTROLTIMEOUT;
        static const std::string MONITORTIMEOUT;
		static const std::string PRIORITY;
		static const std::string PROCESSOR;
               
    };
    
    typedef boost::shared_ptr<Process> ProcessPtr;
} //close namespace TA_Base_Core

#endif // !defined(AFX_PROCESS_H__466F7C62_AD36_4CB6_B58A_44C5906CF2FF__INCLUDED_)
