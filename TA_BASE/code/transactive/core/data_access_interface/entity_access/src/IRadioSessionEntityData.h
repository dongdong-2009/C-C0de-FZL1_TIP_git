/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IRadioSessionEntityData.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IRadioSession is an interface to a Radio Session entity object. It allows the RadioSession object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRadioSessionEntity_INCLUDED_)
#define IRadioSessionEntity_INCLUDED_

#include <string>
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

    class IRadioSessionEntityData : public virtual IEntityData
    {

    public:
        virtual ~IRadioSessionEntityData() { };

        /**
         * getAssociatedConsole
         *
         * Returns the key of the associated console that will be using this radio session
		 * Some sessions, which do not use audio, do not have a console key in which case 
		 * 0 is returned.
         *
         * @return    The associated console key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAssociatedConsoleKey() = 0;


        /**
         * getAudioLinkId
         *
         * Returns the radio audio link to be associated with this radio session. 0 indicates
		 * that no audio link is required.
         *
         * @return    The audio link
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAudioLinkId() = 0;


        /**
         * getAudioLinkServerId
         *
         * Returns the index of the server (CRS/SCN-TX) with which to associate
         * the radio audio link to be associated with this radio session.
		 * (Zero is a valid value).
         *
         * NOTE: also applies to the Occa/TrainComms session, which must be
         *       allocated a session on the radio server
         *
         * @return    The server index for the corresponding audio link
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAudioLinkServerId() = 0;

        /**
         * useForTrainCommunication
         *
         * Indicates if this session should be used for train communications.
         *
         * @return    True - can be used for train communications
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual bool useForTrainCommunication() = 0;

        /**
         * getRadioUserName
         *
         * Returns the user name to be used for login into the radio system.
         *
         * @return    user name
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getRadioUserName() = 0;


        virtual std::string getConsoleComPort() = 0;

    };

} //close namespace TA_Base_Core

#endif // !defined(IRadioSessionEntity_INCLUDED_)
