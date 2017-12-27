/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/Token.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:51:44 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  */
#include "bus/mathematical_evaluation/src/Token.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "bus/mathematical_evaluation/src/MEcommonDef.h"

namespace TA_Base_Bus
{

    //
    // setValue
    //
    void Token::setValue( const std::string& value )
    {
        m_value = value;
    }


    //
    // setType
    //
    void Token::setType( const E_TokenType type )
    {
        m_type = type;
    }


    //
    // getType
    //
    const E_TokenType Token::getType()
    {
        return m_type;
    }

	const std::string Token::getOperatorValue()
	{
		return m_value;
	}

	//
	// getBooleanOperator
	//
	const std::string Token::getBooleanOperator()
	{
		return m_value;
	}


	//
	// getRealNumOperator
	//
	const std::string Token::getRealNumOperator()
	{
		return m_value;
	}

    //
    // getRealValue
    //
    const double Token::getRealValue()
    {
        return atof(m_value.c_str());
    }


    //
    // getIntegerValue
    //
    const int Token::getIntegerValue()
    {
        return atoi(m_value.c_str());
    }


    //
    // getBooleanValue
    //
    const bool Token::getBooleanValue()
    {
        if( 0 == m_value.compare(TRUE_CONSTANT) )
        {
            return true;
        }
        return false;
    }


    //
    // getStringLiteral
    //
    const std::string Token::getStringLiteral()
    {
        return m_value;
    }


    //
    // getVariable
    //
    const std::string Token::getVariable()
    {
        return m_value;
    }

	const std::string Token::getValue()
	{
		return m_value;
	}
}
