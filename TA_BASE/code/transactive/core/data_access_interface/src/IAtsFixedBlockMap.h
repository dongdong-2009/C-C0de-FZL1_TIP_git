/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IAtsFixedBlockMap.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IAtsFixedBlockMap is an interface to an AtsFixedBlockMap object. It allows the AtsFixedBlockMap object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IAtsFixedBlockMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IAtsFixedBlockMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IAtsFixedBlockMap
    {

    public:
        virtual ~IAtsFixedBlockMap() { };


        /**
         * getZoneId
         *
         * Returns the zone Id that maps to the branchID and abscissa value.  The zone 
         * Id could be a whole number of a decimal number to indicate an overlap zone.
         *
         * @param branchId - The branch Id
         * @param fixBlockAbsicissa - The abscissa of Fixed Block from origin of Branch
         * @param headCarAbscissa - The abscissa of Head Car along Fixed Block
         *
         * @return The zone Id.  This may be in the form of an integer to indicate a 
         *         normal zone or a decimal number like 3.5 to indicate it is in the
         *         pre-conflict state in the stage overlap zone "3/4".
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual double getZoneId(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa) =0;

			/**
         * getTrack
         *
         * Returns the track that maps to the branchID and abscissa value.  
         *
         * @param branchId - The branch Id
         * @param fixBlockAbsicissa - The abscissa of Fixed Block from origin of Branch
         * @param headCarAbscissa - The abscissa of Head Car along Fixed Block
         *
         * @return The track. 
         *
         * @exception DataException A DataException will be thrown if there is no such
         *            branch in the zone type or if the abscissa is out of range (i.e.
         *            it does not fall into any of the zones along the branch).
         */
        virtual std::string getTrack(unsigned long branchId, unsigned long fixBlockAbscissa, unsigned long headCarAbscissa) =0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IAtsFixedBlockMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
