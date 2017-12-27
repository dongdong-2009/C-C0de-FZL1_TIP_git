#ifndef M_Eval_ExpressionEvaluator_H
#define M_Eval_ExpressionEvaluator_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/ExpressionEvaluator.h $
  * @author:  Ross Tucker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:51:44 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  */
#include <vector>
#include <string>
#include <map>
#include "bus/mathematical_evaluation/src/IBooleanContext.h"
#include "bus/mathematical_evaluation/src/IRealNumberContext.h"
#include "bus/mathematical_evaluation/src/IStringContext.h"
#include "bus/mathematical_evaluation/src/MEOperand.h"
#include "bus/mathematical_evaluation/src/IOperator.h"
#include "bus/mathematical_evaluation/src/IContext.h"
#include "bus/mathematical_evaluation/src/Tokeniser.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{

    class ExpressionEvaluator
    {
    friend class ReadLockHelper;

    public:

        /**
          * Constructor
          */
        ExpressionEvaluator();

        /**
          * Constructor
          *
          * This constructor instantiates a Tokeniser object and parses
          * the supplied expression (by calling setNewExpression)
          *
          * @param The expression
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be parsed.
          */
        ExpressionEvaluator( const std::string& p_expression );

        /**
          * Destructor
          */
        virtual ~ExpressionEvaluator();

        /**
          * setNewExpression
          *
          * This method instantiates the m_tokeniser object (if necessary)
          * and parses/tokenises the expression.
          *
          * @param The expression
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be parsed.
          */
        void setNewExpression(const std::string& p_expression);


		/**
          * evaluateBooleanExpression
          *
          * This method evaluates a boolean expression.
          *
          * @return The result of the expression after evaluation
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
		bool evaluateBooleanExpression();


		/**
          * evaluateRealNumberExpression
          *
          * This method evaluates a real number expression.
          *
          * @return The result of the expression after evaluation
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
		double evaluateRealNumberExpression();
		
		/**
          * registerEOperators
          *
          * This method register operators which
          *
          * @return The result of the expression after evaluation
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
		void registerEOperators();

		/**
          * addContext
          *
          * This method is used to add contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any variables according it's type
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
		void addContext(IContext* pContext);

		/**
          * addBooleanContext
          *
          * This method is used to add boolean contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any boolean variables
          * included in supplied expressions.
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
        void addBooleanContext( const IBooleanContext* p_booleanContext );

        /**
          * addRealNumberContext
          *
          * This method is used to add real number contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any real number variables
          * included in supplied expressions.
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
        void addRealNumberContext( const IRealNumberContext* p_realNumContext );

        /**
          * addStringContext
          *
          * This method is used to add string contexts to the expression evaluator.
          * These contexts will be used to retrieve the values for any string variables
          * included in supplied expressions.
          *
          * NOTE: Contexts need to exist for at least as long as the ExpressionEvaluator object since no ownership is
          *       transferred to it.
          *
          * @param The context to add
          */
        void addStringContext( const IStringContext* p_stringContext );

    private:
        ExpressionEvaluator( const ExpressionEvaluator& theExpressionEvaluator);
        ExpressionEvaluator& operator=(const ExpressionEvaluator&);
		/**
          * evaluate
          *
          * This method evaluates a expression.
          *
          * @return The result of the expression after evaluation
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
		MEOperand evaluate(bool bVerifySemantic = false);

		/**
          * evaluateElementExpression
          *
          * This method evaluates an element expression and put the result back to parameter calcStack
          *
          * @param op			the token of operator which need to evaluate
		  * @param calcStack    the stack which contain operands that op need to calculate
          *
          * @exception MathematicalEvaluationException thrown if the expression
          *            could not be evaluated.
          */
		void evaluateElementExpression(Tokeniser::TokenPtr& op, std::stack<MEOperand>& calcStack, bool bVerifySemantic);

		/**
          * isOperator
          *
          * This method check whether tokenType is a operator 
          *
          * @param tokenType
          */
		bool isOperator(TA_Base_Bus::E_TokenType tokenType);

		/**
          * getOperator
          *
          * This method get Operator pointer which this evaluator support 
          *
          * @param opID : Operator's identify
		  *
		  * @the pointer of operator if support
          */
		boost::shared_ptr<IOperator> getOperator(const std::string& opID);

		/**
          * convertTokenToOperand
          *
          * This method convert token to operand 
          *
          * @param token : token which need to be converted
		  *
		  * @return : operand which token convert
          */
		MEOperand convertTokenToOperand(Tokeniser::TokenPtr& token, bool bVerifySemantic);
		
		/**
          * convertVariableTokenToOperand
          *
          * This method convert variable token to operand
		  *
		  * @param token : token which need to be converted
		  *
		  * @return : operand which token convert
          */
		MEOperand convertVariableTokenToOperand(Tokeniser::TokenPtr& token, bool bVerifySemantic);

		void checkSemantic();

		void aquireReadLock();
		void releaseReadLock();
	private:
		int						m_readCount;
		std::string				m_expression;
		TA_Base_Bus::Tokeniser	m_tokeniser;
		//Use IContext instead of IStringContext, 
		//IRealNumberContext, IBooleanContext
		IContext*			    m_context;
		bool					m_useIContext;
		//////////////////////////////////
		//Here we reserve these three members 
		//to be compatible with old user of this library
		const IStringContext*		m_stringContext;
		const IRealNumberContext*	m_realContext;
		const IBooleanContext*		m_boolContext;
		/////////////////////////////////
		TA_Base_Core::ReEntrantThreadLockable m_writeLock;
		TA_Base_Core::ReEntrantThreadLockable m_readLock;
		std::map<std::string,  boost::shared_ptr<IOperator> > m_operators;
	};
}
#endif // M_Eval_ExpressionEvaluator_H
