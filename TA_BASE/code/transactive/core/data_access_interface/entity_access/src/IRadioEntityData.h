/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IRadioEntityData.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IRadioEntityData is an interface to a Radio entity object. It allows the Radio object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IRadioEntityData_INCLUDED)
#define IRadioEntityData_INCLUDED

#include <string>
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

    class IRadioEntityData : public virtual IEntityData
    {

    public:
        virtual ~IRadioEntityData() { };


        /**
         * getPrimaryTCPServers
         *
         * Returns the IP address(es) (or machine name(s)) of the primary radio TCPServer(s).
		 * They are returned in the format <name>,<name> when more than 1 is specified.
		 *                            e.g. 123.234.567.432,123.234.567.433
		 *                                 localhost,NTServer
         *
         * @return    IP address of the primary radio TCPServer(s)
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getPrimaryTCPServers() = 0;

        /**
         * getSecondaryTCPServers
         *
         * Returns the IP address(es) (or machine name(s)) of the secondary radio TCPServer(s).
		 * They are returned in the format <name>,<name> when more than 1 is specified.
		 *                            e.g. 123.234.567.432,123.234.567.433
		 *                                 localhost,NTServer
         *
         * @return    IP address of the secondary radio TCPServer(s)
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getSecondaryTCPServers() = 0;


		
        /**
         * getXONXOFFTimeout
         *
         * Returns the time interval to be used for timing out an XON signal from TIMS.
         *
         * @return    Time interval (in seconds) as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getXONXOFFTimeout() = 0;

        /**
         * getDefaultVolume
         *
         * Returns the Default volume to be used for every call.
         *
         * @return    volume (0-15) as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getDefaultVolume() = 0;

		virtual std::string getAssetName() = 0;

	virtual std::string getUDPServer () = 0;
	virtual std::string getUDPServicePort () = 0;
	virtual long getTis5Timer () = 0;
	virtual long getTis7Timer () = 0;

		/**
		 * getSDSLinkDpName
		 *
		 * Returns the SDS Link Datapoint Name
		 *
		 *@return Datapoint Name
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
		 *            problem establishing a connection with the database, or if an
		 *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be 
		 *            converted to the required format (e.g. ACCESSLEVEL should be an 
		 *            unsigned long), or if the wrong amount of data is retrieved.
		 */
		virtual std::string getSDSLinkDpName() = 0;

    };

} //close namespace TA_Base_Core

#endif // !defined(IRadioEntityData_INCLUDED)
