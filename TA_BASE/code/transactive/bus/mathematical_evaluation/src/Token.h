#ifndef M_Eval_Token_H
#define M_Eval_Token_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mathematical_evaluation/src/Token.h $
  * @author:  Ross Tucker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 17:51:44 $
  * Last modified by:  $Author: haijun.li $
  * 
  * 
  */
#include <string>
#include <map>

namespace TA_Base_Bus
{

    enum E_TokenType
    {
        EETT_BooleanOperator     = 0,
        EETT_BooleanUnaryOperator = 1,
        EETT_RealNumOperator     = 2,
        EETT_RealValue           = 3,
        EETT_IntegerValue        = 4,
        EETT_BooleanValue        = 5,
        EETT_StringLiteral       = 6,
        EETT_Variable            = 7,
        EETT_OpenParenthesis     = 8,
        EETT_CloseParenthesis    = 9
    };

    class Token
    {
    public:

        /**
          * Constructor
          */
        Token()	{}

        /**
          * Destructor
          */
        virtual ~Token() {}

        void setValue( const std::string& value );

        void setType( const E_TokenType type );

        const E_TokenType getType();

		const std::string getOperatorValue();
		
		const std::string getRealNumOperator();

		const std::string getBooleanOperator();

        const double getRealValue();

        const int getIntegerValue();

        const bool getBooleanValue();

        const std::string getStringLiteral();

        const std::string getVariable();

		const std::string getValue();

        // Allow Token to be copyable.
        Token( const Token& theToken);

    private:

        Token& operator=(const Token&);

		E_TokenType m_type;
        std::string m_value;
    };
}
#endif // M_Eval_Token_H
