/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/ExpressionEvaluator.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:51:44 $
  * Last modified by:  $Author: haijun.li $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

    class ReadLockHelper
    {
    public:

        ReadLockHelper( TA_Base_Bus::ExpressionEvaluator& expr) : m_objectToLock(expr)
        {
            m_objectToLock.aquireReadLock();
        }

        ~ReadLockHelper()
        {
            m_objectToLock.releaseReadLock();
        }

    private:

        ReadLockHelper( const ReadLockHelper& theReadLockHelper);
        ReadLockHelper& operator=(const ReadLockHelper&);

        TA_Base_Bus::ExpressionEvaluator& m_objectToLock;
    };


    ExpressionEvaluator::ExpressionEvaluator()
    :	
	m_readCount(0), 
	m_tokeniser(),
	m_useIContext(false),
	m_context(0),
	m_stringContext(0),
	m_boolContext(0),
	m_realContext(0)
    {
		registerEOperators();
    }


    ExpressionEvaluator::ExpressionEvaluator( const std::string& p_expression )
    :
	m_readCount(0), 
	m_expression(p_expression),
	m_tokeniser(),
	m_useIContext(false),
	m_context(0),
	m_stringContext(0),
	m_boolContext(0),
	m_realContext(0)
    {
		registerEOperators();

        try
        {
            m_tokeniser.setExpression(p_expression);
			checkSemantic();
        }
        catch( TA_Base_Core::MathematicalEvaluationException )
        {
            throw;
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception raised.", "Supplied expression could not be parsed." );
            TA_THROW(TA_Base_Core::MathematicalEvaluationException("Unknown exception raised - supplied expression could not be parsed"));
        }
    }


    ExpressionEvaluator::~ExpressionEvaluator()
    {
		m_operators.clear();
    }


    void ExpressionEvaluator::setNewExpression(const std::string& p_expression)
    {
        TA_Base_Core::ThreadGuard guard( m_writeLock );

		if (m_expression != p_expression)
		{
			m_expression = p_expression;

			try
			{
				m_tokeniser.setExpression(p_expression);
				checkSemantic();
			}
			catch( TA_Base_Core::MathematicalEvaluationException )
			{
				throw;
			}
			catch( ... )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception raised.", "Supplied expression could not be parsed." );
				TA_THROW(TA_Base_Core::MathematicalEvaluationException("Unknown exception raised - supplied expression could not be parsed"));
			}
		}
    }

	bool ExpressionEvaluator::evaluateBooleanExpression()
	{
		bool returnValue = false;
		TA_Base_Bus::ReadLockHelper g(*this);
		try
		{
			MEOperand calcRet = evaluate();
			returnValue = calcRet.getValueAsBool();
		}
		catch(TA_Base_Core::MathematicalEvaluationException& e)
		{
			LOG1(SourceInfo, DebugUtil::DebugError, "Evaluate bool expression exception, reason: %s", e.what());
			throw;
		}
		catch(...)
		{
			LOG0(SourceInfo, DebugUtil::DebugError, "Evaluate bool expression unknown exception");
			throw;
		}
		LOG1(SourceInfo, DebugUtil::DebugDebug, "Evaluate bool expression result:%d", returnValue);
		return returnValue;
	}

	double ExpressionEvaluator::evaluateRealNumberExpression()
	{
		double returnValue = 0.0;
		TA_Base_Bus::ReadLockHelper g(*this);
		try
		{
			MEOperand calcRet = evaluate();
			returnValue = calcRet.getValueAsReal();
		}
		catch(TA_Base_Core::MathematicalEvaluationException& e)
		{
			LOG1(SourceInfo, DebugUtil::DebugError, "Evaluate real expression exception, reason: %s", e.what());
			throw;
		}
		catch(...)
		{
			LOG0(SourceInfo, DebugUtil::DebugError, "Evaluate real expression unknown exception");
			throw;
		}
		LOG1(SourceInfo, DebugUtil::DebugDebug, "Evaluate real expression result:%lf", returnValue);
		return returnValue;
	}

	void ExpressionEvaluator::registerEOperators()
	{
		boost::shared_ptr<IOperator> pOperator(new AddOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new SubOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new MultiplyOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new DivisionOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new EqualOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new NotEqualOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new GreaterThanOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new LessThanOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new GreaterThanOrEqualOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new LessThanOrEqualOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new PowerOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new FabsOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new SqrtOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new AndOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new OrOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new XorOperator());
		m_operators[pOperator->getIdentify()] = pOperator;

		pOperator.reset(new NotOperator());
		m_operators[pOperator->getIdentify()] = pOperator;
	}

	void ExpressionEvaluator::addContext(IContext* pContext)
	{
		TA_Base_Core::ThreadGuard guard( m_writeLock );
		m_useIContext = true;
		m_context = pContext;
	}

	void ExpressionEvaluator::addBooleanContext( const IBooleanContext* p_booleanContext )
	{
		TA_Base_Core::ThreadGuard guard( m_writeLock );
		m_boolContext = p_booleanContext;
	}

	void ExpressionEvaluator::addRealNumberContext( const IRealNumberContext* p_realNumContext )
	{
		TA_Base_Core::ThreadGuard guard( m_writeLock );
		m_realContext = p_realNumContext;
	}


	void ExpressionEvaluator::addStringContext( const IStringContext* p_stringContext )
	{
		TA_Base_Core::ThreadGuard guard( m_writeLock );
		m_stringContext = p_stringContext;
	}

	MEOperand ExpressionEvaluator::evaluate(bool bVerifySemantic)
	{
		std::stack<MEOperand> calcStack;
		std::vector<Tokeniser::TokenPtr > calcTokens = m_tokeniser.getTokens();
		for (unsigned int i = 0; i < calcTokens.size(); ++i)
		{
			Tokeniser::TokenPtr& token = calcTokens[i];
			if ( !isOperator(token->getType()) )
			{
				calcStack.push(convertTokenToOperand(token, bVerifySemantic));
			}
			else
			{
				evaluateElementExpression(token, calcStack, bVerifySemantic);
			}
		}

		if(calcStack.size() == 1)
		{
			return calcStack.top();
		}
		else
		{
			std::string msg("Invalid expression");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	bool ExpressionEvaluator::isOperator(TA_Base_Bus::E_TokenType tokenType)
	{
		bool isOperator = false;
		switch(tokenType)
		{
		case TA_Base_Bus::EETT_BooleanOperator:
		case TA_Base_Bus::EETT_RealNumOperator:
		case TA_Base_Bus::EETT_BooleanUnaryOperator:
			isOperator = true;
			break;
		default :
			isOperator = false;
			break;
		}
		return isOperator;
	}

	void ExpressionEvaluator::evaluateElementExpression(Tokeniser::TokenPtr& op, std::stack<MEOperand>& calcStack, bool bVerifySemantic)
	{
		const std::string& opId = op->getValue();
		boost::shared_ptr<IOperator> opPtr =  getOperator(opId);
		if (opPtr.get() != NULL)
		{
			if (bVerifySemantic)
			{
				opPtr->verifySemantic(calcStack);
			}
			else
			{
				opPtr->evaluate(calcStack);
			}
		}
		else
		{
			std::string msg("Not supported operator : ");
			msg += opId;
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}

	boost::shared_ptr<IOperator> ExpressionEvaluator::getOperator(const std::string& opID)
	{
		boost::shared_ptr<IOperator> op;
		std::map<std::string,  boost::shared_ptr<IOperator> >::iterator itOp = m_operators.find(opID);
		if (itOp != m_operators.end())
		{
			op = itOp->second;
		}
		return op;
	}

	MEOperand ExpressionEvaluator::convertTokenToOperand(Tokeniser::TokenPtr& token, bool bVerifySemantic)
	{
		MEOperand op;
		E_TokenType tokenType = token->getType();

		switch(tokenType)
		{
		case TA_Base_Bus::EETT_RealValue:
			op.setRealValue(token->getRealValue());
			break;
		case TA_Base_Bus::EETT_IntegerValue:
			op.setIntValue(token->getIntegerValue());
			break;
		case TA_Base_Bus::EETT_BooleanValue:
			op.setBoolValue(token->getBooleanValue());
			break;
		case TA_Base_Bus::EETT_StringLiteral:
			op.setStringValue(token->getStringLiteral());
			break;
		case TA_Base_Bus::EETT_Variable:
			op = convertVariableTokenToOperand(token, bVerifySemantic);
			break;
		default:
			std::string msg("Invalid token for operand");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		return op;
	}

	MEOperand ExpressionEvaluator::convertVariableTokenToOperand(Tokeniser::TokenPtr& token, bool bVerifySemantic)
	{
		if(token->getType() != EETT_Variable)
		{
			std::string msg("Invalid token pass to convertVariableTokenToOperand");
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}

		if (bVerifySemantic)
		{
			MEOperand operand;
			operand.setDataType(ME_ANY_TYPE);
			return operand;
		}

		if (m_useIContext)
		{
			if(0 == m_context)
			{
				std::string msg("context is null when get value of variable");
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			} 

			if (m_context->isBooleanVariable(token->getValue()))
			{
				MEOperand ret(m_context->getBooleanValue(token->getValue()));
				return ret;
			}
			else if (m_context->isRealNumberVariable(token->getValue()))
			{
				MEOperand ret(m_context->getRealNumberValue(token->getValue()));
				return ret;
			}
			else if (m_context->isStringVariable(token->getValue()))
			{
				std::string varName(m_context->getStringValue(token->getValue()));
				MEOperand ret(varName);
				return ret;
			}
			else
			{
				std::string msg("Invalid variable type, var:");
				msg += token->getValue();
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			}
		}
		else
		{
			if(0 == m_boolContext && 0 == m_realContext && 0 == m_stringContext)
			{
				std::string msg("context is null when get value of variable");
				throw TA_Base_Core::MathematicalEvaluationException(msg);
			} 

			if (m_boolContext != NULL)
			{
				try
				{
					bool varValue = m_boolContext->getValue(token->getValue());
					MEOperand ret(varValue);
					return ret;
				}
				catch(TA_Base_Core::MathematicalEvaluationException)
				{

				}
			}

			if (m_realContext != NULL)
			{
				try
				{
					double varValue = m_realContext->getValue(token->getValue());
					MEOperand ret(varValue);
					return ret;
				}
				catch(TA_Base_Core::MathematicalEvaluationException)
				{

				}
			}

			if (m_stringContext != NULL)
			{
				try
				{
					std::string varValue = m_stringContext->getValue(token->getValue());
					MEOperand ret(varValue);
					return ret;
				}
				catch(TA_Base_Core::MathematicalEvaluationException)
				{

				}
			}

			std::string msg("Invalid variable type, var:");
			msg += token->getValue();
			throw TA_Base_Core::MathematicalEvaluationException(msg);
		}
	}
    
	void ExpressionEvaluator::checkSemantic()
	{
		evaluate(true);
	}

    void ExpressionEvaluator::aquireReadLock()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "aquireReadLock" );

        TA_Base_Core::ThreadGuard guard(m_readLock);

        ++m_readCount;
        if( 1 == m_readCount )
        {
            m_writeLock.acquire();
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "aquireReadLock" );
    }


    void ExpressionEvaluator::releaseReadLock()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "releaseReadLock" );

        TA_Base_Core::ThreadGuard guard(m_readLock);

        --m_readCount;
        if( 0 == m_readCount )
        {
            m_writeLock.release();
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "releaseReadLock" );
    }
}
