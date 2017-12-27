/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/CommonContext.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/01/21 14:56:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	Base class for BooleanContext and RealNumberContext...
  *
  */

#ifndef COMMON_CONTEXT_H
#define COMMON_CONTEXT_H

#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/mathematical_evaluation/src/IContext.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_Base_Bus
{
	class IExpressionNodeNotification;
    class EventDetailsFactory;
	class InputAssociation;

	class CommonContext : public IEntityUpdateEventProcessor,
						  public IContext
	{
	public:

		/**
		* Constructor, Destructor
		* 
		*/
		CommonContext();
		CommonContext ( IExpressionNodeNotification * userOfExpression );
		virtual ~CommonContext();


		//
		// operations specific to IEntityUpdateEventProcessor
		//


		virtual void processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType );


		//
		// operations specific to CommonContext
		//


		/**
		*
		* enableCallback
		*
		* indicate if the online changes of the input entities associated with the
		* mathematical expression will be notifiying the user/owner of the expression
		*
		* @param    enabled		Indication if callback mechanism is enabled
		*
		*/
		virtual void enableCallback ( bool enabled );

		
		/**
		*
		* getCombinedStatus
		*
		* calculate and return the combined status from all input entities associated
		* with the mathematical expression.
		*
		*/
		virtual EQualityStatus getCombinedStatus() const;


		/**
		*
		* isGoodCombinedStatus
		*
		* return true if the combined status from all input entities associated
		* with the mathematical expression are either QUALITY_GOOD_NO_SPECIFIC_REASON
		* or QUALITY_GOOD_LOCAL_OVERRIDE, false otherwise
		*
		*/
		virtual bool isGoodCombinedStatus() const;


		/**
		*
		* getExpressionResultString
		*
		* Returns a string containing the result of the expression evaluation in
		* the following format:
		*
		* [PointDescription1] is [Value1], [PointDescription2] is [Value2], ...
		*
		*/
		virtual std::string getExpressionResultString();

		virtual bool getBooleanValue(const std::string& name);
		virtual double getRealNumberValue(const std::string& name);
		virtual std::string getStringValue(const std::string& name);
		virtual bool isBooleanVariable(const std::string& name);
		virtual bool isRealNumberVariable(const std::string& name);
		virtual bool isStringVariable(const std::string& name);

	private:
		InputAssociation * findInputDependency ( const std::string & inputName );
		InputAssociation * createInputDependency ( const std::string & inputName );

        CommonContext ( const CommonContext & obj );
		CommonContext & operator= ( const CommonContext & rhs );

	private:
		std::vector < InputAssociation * >  m_listOfInputDependencies;
		IExpressionNodeNotification *       m_userOfExpression;
		bool                                m_toCallback;
        std::string							m_invalidDependencyErrorString;
        TA_Base_Core::ReEntrantThreadLockable m_lock;
        EventDetailsFactory *				m_eventDetailsFactory;
    };

} // close namespace TA_Base_Bus

#endif

