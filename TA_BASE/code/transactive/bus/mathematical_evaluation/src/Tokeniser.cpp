/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/Tokeniser.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  * 
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "bus/mathematical_evaluation/src/Tokeniser.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/utilities/src/DebugUtil.h"

#include <cctype>

namespace
{
    const std::string INTEGER_NUM       = "0123456789";

    //
    // isValidRealNumber
    //
    bool isValidRealNumber(const std::string& p_expression)
    {
        if( p_expression.find_first_of(INTEGER_NUM) != 0 )
        {
            return false;
        }

        std::string::size_type decimal = p_expression.find_first_not_of(INTEGER_NUM);

        if( decimal == std::string::npos )
        {
            return false;
        }

        if( p_expression[decimal] != '.' )
        {
            return false;
        }

        if( p_expression.find_first_of(INTEGER_NUM, decimal) != (decimal + 1) )
        {
            return false;
        }

        return true;
    }

    //
    // stringsAreEqual
    //
    bool stringsAreEqual(const std::string& string1, const std::string& string2)
    {
        if( string1.length() != string2.length() )
        {
            return false;
        }

        std::string::const_iterator it1=string1.begin();
        std::string::const_iterator it2=string2.begin();

        //stop when either string's end has been reached
        while ( (it1!=string1.end()) && (it2!=string2.end()) ) 
        { 
            if(::toupper(*it1) != ::toupper(*it2)) //letters differ?
            {
                return false;
            }

            ++it1;
            ++it2;
        }

        return true;
    }
};

namespace TA_Base_Bus
{
	OperatorPrecedence Tokeniser::m_operatorPrecedence;
    //  
    // Constructor
    //
    Tokeniser::Tokeniser()
    {
    }


    //
    // Constructor
    //
    Tokeniser::Tokeniser(const std::string& p_expression)
    {
        parse(p_expression);
    }


    //
    // Destructor
    //
    Tokeniser::~Tokeniser()
    {
        clearTokens();
    }


    //
    // clearTokens
    //
    void Tokeniser::clearTokens()
    {
		m_lastToken.reset();
        m_tokens.clear();
		while( !m_tokenStack.empty() )
			m_tokenStack.pop();
    }


    //
    // getTokens
    //
    std::vector< boost::shared_ptr< TA_Base_Bus::Token > > Tokeniser::getTokens() const
    {
        return m_tokens;
    }


    //
    // setExpression
    //
    void Tokeniser::setExpression(const std::string& p_expression)
    {
        parse(p_expression);
    }


    //
    // extractNextOperator
    //
    unsigned int Tokeniser::extractNextOperator(const std::string& p_expression)
    {
		unsigned int newIndex = 0;
        if( 0 == p_expression.find_first_of(OPERATOR, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
			bool isOperator = false;
            std::string val = p_expression.substr(0, 1);
            
			newIndex = 1;
			token->setValue(val);

			if ( 0 == val.compare("-") ) 
			{		   
				//this is may be a negative number
				if ( m_lastToken.get() == NULL || 
					 m_lastToken->getType() == TA_Base_Bus::EETT_OpenParenthesis ||
					 m_lastToken->getType() == TA_Base_Bus::EETT_RealNumOperator ||
					 m_lastToken->getType() == TA_Base_Bus::EETT_BooleanOperator || 
					 m_lastToken->getType() == TA_Base_Bus::EETT_BooleanUnaryOperator)
				{
					if( isValidRealNumber(p_expression.substr(1, (p_expression.find_first_not_of(REAL_NUM, 1)))) )
					{
						std::string num = p_expression.substr(0, (p_expression.find_first_not_of(REAL_NUM, 1)));
						token->setValue(num);
						token->setType(TA_Base_Bus::EETT_RealValue);
						m_tokens.push_back(token);
						newIndex = num.length();
					}
					else if( 1 == p_expression.find_first_of(INTEGER_NUM, 1) )
					{
						std::string num = p_expression.substr(0, (p_expression.find_first_not_of(INTEGER_NUM, 1)));
						token->setValue(num);
						token->setType(TA_Base_Bus::EETT_IntegerValue);
						m_tokens.push_back(token);
						newIndex = num.length();
					}
					else
					{
                    	TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression. Illegal positioning of '-' character"));
					}	
				}
				else
				{
					isOperator = true;
					token->setType(TA_Base_Bus::EETT_RealNumOperator);
				}
            }
			else if( 0 == val.compare("=") )
			{
				isOperator = true;
				token->setType(TA_Base_Bus::EETT_BooleanOperator);  
			}
			else
			{
				isOperator = true;
				token->setType(TA_Base_Bus::EETT_RealNumOperator);
			}

			if (isOperator)
			{
				saveOperator(token);
			}

			m_lastToken = token;
        }

        return newIndex;
    }


    //
    // extractNextConstant
    //
    unsigned int Tokeniser::extractNextConstant(const std::string& p_expression)
    {
		bool isOperatorConstant = true;
		unsigned int retIndex	= 0;
		TokenPtr token(new TA_Base_Bus::Token);

		if( (p_expression.length() >= FALSE_CONSTANT.length()) &&
			stringsAreEqual(p_expression.substr(0, FALSE_CONSTANT.length()), FALSE_CONSTANT) )
		{
			token->setValue(std::string(FALSE_CONSTANT));
			token->setType(TA_Base_Bus::EETT_BooleanValue);
			m_tokens.push_back(token);
			m_lastToken = token;
			isOperatorConstant = false;
			retIndex = FALSE_CONSTANT.length();
		}
		else if( (p_expression.length() >= TRUE_CONSTANT.length()) &&
			stringsAreEqual(p_expression.substr(0, TRUE_CONSTANT.length()), TRUE_CONSTANT) )
		{
			token->setValue(std::string(TRUE_CONSTANT));
			token->setType(TA_Base_Bus::EETT_BooleanValue);
			m_tokens.push_back(token);
			m_lastToken = token;
			isOperatorConstant = false;
			retIndex = TRUE_CONSTANT.length();
		}
        else if( (p_expression.length() >= NOTEQUAL.length()) &&
            stringsAreEqual(p_expression.substr(0, NOTEQUAL.length()), NOTEQUAL) )
        {
            token->setValue(std::string(NOTEQUAL));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            retIndex = NOTEQUAL.length();
        }
        else if( (p_expression.length() >= GREATERTHANOREQUAL.length()) &&
            stringsAreEqual(p_expression.substr(0, GREATERTHANOREQUAL.length()), GREATERTHANOREQUAL) )
        {
            token->setValue(std::string(GREATERTHANOREQUAL));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            retIndex = GREATERTHANOREQUAL.length();
        }
        else if( (p_expression.length() >= LESSTHANOREQUAL.length()) &&
            stringsAreEqual(p_expression.substr(0, LESSTHANOREQUAL.length()), LESSTHANOREQUAL) )
        {
            token->setValue(std::string(LESSTHANOREQUAL));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            retIndex = LESSTHANOREQUAL.length();
        }
        else if( (p_expression.length() >= SQRT.length()) &&
            stringsAreEqual(p_expression.substr(0, SQRT.length()), SQRT) )
        {
            token->setValue(std::string(SQRT));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            retIndex = SQRT.length();
        }
		else if( (p_expression.length() >= FABS.length()) &&
            stringsAreEqual(p_expression.substr(0, FABS.length()), FABS) )
        {
            token->setValue(std::string(FABS));
            token->setType(TA_Base_Bus::EETT_RealNumOperator);
            retIndex = FABS.length();
        }
        else if( (p_expression.length() >= AND.length()) &&
            stringsAreEqual(p_expression.substr(0, AND.length()), AND) )
        {
            token->setValue(std::string(AND));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            retIndex = AND.length();
        }
        else if( (p_expression.length() >= OR.length()) &&
            stringsAreEqual(p_expression.substr(0, OR.length()), OR) )
        {
            token->setValue(std::string(OR));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            retIndex = OR.length();
        }
        else if( (p_expression.length() >= XOR.length()) &&
            stringsAreEqual(p_expression.substr(0, XOR.length()), XOR) )
        {
            token->setValue(std::string(XOR));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);          
            retIndex = XOR.length();
        }
        else if( (p_expression.length() >= NOT.length()) &&
            stringsAreEqual(p_expression.substr(0, NOT.length()), NOT) )
        {
            token->setValue(std::string(NOT));
            token->setType(TA_Base_Bus::EETT_BooleanOperator);
            retIndex = NOT.length();
        }
		else
		{
			isOperatorConstant = false;
		}
		
		if ( isOperatorConstant )
		{
			saveOperator(token);
			m_lastToken = token;
		}

        return retIndex;
    }


    //
    // extractNextNumber
    //
    unsigned int Tokeniser::extractNextNumber(const std::string& p_expression)
    {
		unsigned int newIndex = 0;

        if( isValidRealNumber(p_expression.substr(0, (p_expression.find_first_not_of(REAL_NUM)))) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            std::string num = p_expression.substr(0, (p_expression.find_first_not_of(REAL_NUM)));
            token->setValue(num);
            token->setType(TA_Base_Bus::EETT_RealValue);
            m_tokens.push_back(token);
			m_lastToken = token;
            newIndex = num.length();
        }
        else if( 0 == p_expression.find_first_of(INTEGER_NUM, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            std::string num = p_expression.substr(0, (p_expression.find_first_not_of(INTEGER_NUM)));
            token->setValue(num);
            token->setType(TA_Base_Bus::EETT_IntegerValue);
            m_tokens.push_back(token);
			m_lastToken = token;
            newIndex = num.length();
        }

        return newIndex;
    }


    //
    // extractNextVariable
    //
    unsigned int Tokeniser::extractNextVariable(const std::string& p_expression)
    {
		unsigned int newIndex = 0;
        if( 0 == p_expression.find_first_of('{', 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            unsigned long len = p_expression.find_first_of('}', 1);

            if( len == std::string::npos )
            {
                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression, missing \' character"));
            }

            std::string val = p_expression.substr(1, len - 1);
            token->setValue(val);
            token->setType(TA_Base_Bus::EETT_Variable);
            m_tokens.push_back(token);
			m_lastToken = token;
            newIndex = val.length() + 2;
        }

        return newIndex;
    }


    //
    // extractNextStringLiteral
    //
    unsigned int Tokeniser::extractNextStringLiteral(const std::string& p_expression)
    {
		unsigned int newIndex = 0;
        if( 0 == p_expression.find_first_of('\'', 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            unsigned long len = p_expression.find_first_of('\'', 1);

            if( len == std::string::npos )
            {
                TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression, missing \' character"));
            }

            std::string val = p_expression.substr(1, len - 1);
            token->setValue(val);
            token->setType(TA_Base_Bus::EETT_StringLiteral);
            m_tokens.push_back(token);
			m_lastToken = token;
            newIndex = val.length() + 2;
        }

        return newIndex;
    }


    //
    // extractNextParenthesis
    //
    unsigned int Tokeniser::extractNextParenthesis(const std::string& p_expression)
    {
		unsigned int newIndex = 0;
        if( 0 == p_expression.find_first_of(OPENPARENTHESIS, 0) )
        {
            TokenPtr token(new TA_Base_Bus::Token);
            token->setValue(std::string(OPENPARENTHESIS));
            token->setType(TA_Base_Bus::EETT_OpenParenthesis);
			m_tokenStack.push(token);
			m_lastToken = token;
			newIndex = 1;
        }
		else if( 0 == p_expression.find_first_of(CLOSEPARENTHESIS, 0) )
        {
			bool bMatchParenthesis = false;
			while (!m_tokenStack.empty())
			{
				TokenPtr token = m_tokenStack.top();
				m_tokenStack.pop();
				if (token->getType() != TA_Base_Bus::EETT_OpenParenthesis)
				{
					m_tokens.push_back(token);
				}
				else
				{
					bMatchParenthesis = true;
					break;
				}
			}

			if ( !bMatchParenthesis )
			{
				TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression, Parenthesis don't match"));
			}
			TokenPtr token(new TA_Base_Bus::Token);
			token->setValue(std::string(CLOSEPARENTHESIS));
			token->setType(TA_Base_Bus::EETT_CloseParenthesis);
			m_lastToken = token;

            newIndex = 1;
        }

        return newIndex;
    }


    //
    // parse
    //
    void Tokeniser::parse( const std::string& p_expression)
    {
        clearTokens();

        unsigned long i = 0;

        while( i < p_expression.length() )
        {
            if( p_expression.find(" ", i) == i )
            {
                ++i;
            }
            else
            {
                unsigned long j = i;

                i += extractNextVariable(p_expression.substr(i));
                i += extractNextStringLiteral(p_expression.substr(i));
                i += extractNextParenthesis(p_expression.substr(i));
                i += extractNextConstant(p_expression.substr(i));
                i += extractNextOperator(p_expression.substr(i));
                i += extractNextNumber(p_expression.substr(i));
                if( i == j )
                {
                    TA_THROW(TA_Base_Core::MathematicalEvaluationException("Invalid expression."));
                }
            }
        }
		while( !m_tokenStack.empty() )
		{
			m_tokens.push_back(m_tokenStack.top());
			m_tokenStack.pop();
		}
    }

	bool Tokeniser::isValidRealNumber(const std::string& p_expression)
	{
		if( p_expression.find_first_of(TA_Base_Bus::INTEGER_NUM) != 0 )
		{
			return false;
		}

		unsigned long decimal = p_expression.find_first_not_of(TA_Base_Bus::INTEGER_NUM);

		if( decimal == std::string::npos )
		{
			return false;
		}

		if( p_expression[decimal] != '.' )
		{
			return false;
		}

		if( p_expression.find_first_of(TA_Base_Bus::INTEGER_NUM, decimal) != (decimal + 1) )
		{
			return false;
		}

		return true;
	}

	
	bool Tokeniser::stringsAreEqual(const std::string& string1, const std::string& string2)
	{
		if( string1.length() != string2.length() )
		{
			return false;
		}

		std::string::const_iterator it1=string1.begin();
		std::string::const_iterator it2=string2.begin();

		//stop when either string's end has been reached
		while ( (it1!=string1.end()) && (it2!=string2.end()) ) 
		{ 
			if(::toupper(*it1) != ::toupper(*it2)) //letters differ?
			{
				return false;
			}

			++it1;
			++it2;
		}

		return true;
	}
	
	void Tokeniser::saveOperator(TokenPtr& opToken)
	{
		if ( m_tokenStack.empty() )
		{
			m_tokenStack.push(opToken);
		}
		else
		{
			TokenPtr op = m_tokenStack.top();
			if (op->getType() != TA_Base_Bus::EETT_OpenParenthesis)
			{
				std::string op1Value = opToken->getOperatorValue();
				std::string op2Value = op->getOperatorValue();
				bool isLower = m_operatorPrecedence.isOperatorOfLowerPrecedence(op1Value, op2Value);
				if ( isLower )
				{
					m_tokenStack.pop();
					m_tokens.push_back(op);
					m_tokenStack.push(opToken);
				}
				else
				{
					m_tokenStack.push(opToken);
				}
			}
			else
			{
				m_tokenStack.push(opToken);
			}
		}
	}
}
