/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IAtsPSDData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IAtsPSDData is an interface to an AtsPSDData object.
  * It provides access to methods which will retrieve details of a PSD door including
  * station id, platform id and the mapped ISCS data point and its types.
  *
  */

#if !defined(AFX_IATSPSDDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
#define AFX_IATSPSDDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_

#include <string>

namespace TA_Base_Core
{
	class IAtsPSDData  
	{	

	public:
		virtual ~IAtsPSDData() {};

		/**
		 *	getPSDDataPointEntityKey()
		 *
		 *  Returns the ISCS data point entity key which corresponds to given Ats PSD door number
		 *
		 *  @return		entity key of the ISCS data point
		 *
		 */
		virtual unsigned long getPSDDataPointEntityKey() = 0;
		
		/**
		 *	getPSDDataPointType()
		 *
		 *  Returns the ISCS data point's type.
		 *
		 *  @return		Type as string. One of "Input" / "Output"
		 *
		 */
		virtual std::string getPSDDataPointType() = 0;

		/**
		 *	getPSDDataPointNameOfType()
		 *
		 *  Returns the ISCS data point type's name.
		 *
		 *  @return		Name as string. One of "Door Status" / "Door Inhibit" / "Pre-Close Warning"
		 *
		 */
		virtual std::string getPSDDataPointNameOfType() = 0;
		
		/**
		 *	getStationId()
		 *
		 *  Returns the Id of the location (station) where this PSD is found
		 *
		 *  @return		station Id
		 *
		 */
		virtual unsigned long getStationId() = 0;
		
		/**
		 *	getPlatformId()
		 *
		 *  Returns the Id of the platform where this PSD is found
		 *
		 *  @return		platform Id
		 *
		 */
		virtual unsigned long getPlatformId() = 0;
		
		/**
		 *	getPlatformName()
		 *
		 *  Returns the name of the platform where this PSD is found
		 *
		 *  @return		Name of the platform. One of "IB" / "OB"
		 *
		 */
		virtual std::string getPlatformName() = 0;
		
		/**
		 *	getPSDNumber()
		 *
		 *  Returns the number of the PSD door
		 *
		 *  @return		PSD Number
		 *
		 */
		virtual int getPSDNumber() = 0;

		/**
		 *	getMSSZone()
		 *
		 *  Returns the MSS Zone within which this platform can be found
		 *
		 *  @return		Name of the MSS Zone
		 *
		 */
		virtual std::string getMSSZone() = 0;	
		
		/**
		 *	getKey()
		 *
		 *  Returns the primary key of the record holding this PSD Data
		 *
		 *  @return		entity key of the record
		 *
		 */
		virtual unsigned long getKey() = 0;
	};

} // TA_Base_Core

#endif // !defined(AFX_IATSPSDDATA_H__0086FE03_6182_4BC2_A72B_803743F9C511__INCLUDED_)
