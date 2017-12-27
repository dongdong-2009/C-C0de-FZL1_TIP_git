/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IAtsPowerData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IAtsPowerData is an interface to an AtsPowerData object. 
  * Its purpose is to give access to information about a given Ats Electrical Subsection.
  *
  */

#if !defined(AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include <string>

namespace TA_Base_Core
{
	class IAtsPowerData  
	{	
		
	public:
		virtual ~IAtsPowerData() {};
										
		/**
		 *	getElectricalSubsectionId()
		 *
		 *  Returns the id of the Electrical SubSection within the main line
		 *
		 *  @return		subsection id
		 *
		 */
		virtual unsigned long getElectricalSubSectionId() = 0;		

		/**
		 *	getTractionPowerDataPointEntityKey()
		 *
		 *  Returns the entity key of the ISCS datapoint which corresponds to the electrical 
		 *  subsection within the main line
		 *
		 *  @return		entity key
		 *
		 */
		virtual unsigned long getTractionPowerDataPointEntityKey() = 0;

		/**
		 *	getLocationId()
		 *
		 *  Returns the location key of the location at which this Electrical Subsection exists
		 *
		 *  @return		key of the location
		 *
		 */
		virtual unsigned long getLocationId() = 0;
		
		/**
		 *	getMSSZone()
		 *
		 *  Returns the MSS Zone within which this Electrical SubSection can be found.
		 *
		 *  @return		name of the MSS Zone
		 *
		 */
		virtual std::string getMSSZone() = 0;

		/**
		 *	getKey()
		 *
		 *  Returns the primary key of the record holding this Ats Power Data
		 *
		 *  @return		entity key of the record
		 *
		 */
		virtual unsigned long getKey() = 0;
	};

} // TA_Base_Core

#endif // !defined(AFX_IATSPOWERDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
