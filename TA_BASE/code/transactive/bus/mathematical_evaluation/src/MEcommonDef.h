#ifndef ME_COMMON_DEF_H
#define ME_COMMON_DEF_H

#include <string>

namespace TA_Base_Bus
{
	// Operator's identify
	const std::string OPENPARENTHESIS    = "(";
	const std::string CLOSEPARENTHESIS   = ")";
	const std::string LESSTHAN           = "<";
	const std::string GREATERTHAN        = ">";
	const std::string GREATERTHANOREQUAL = ">=";
	const std::string LESSTHANOREQUAL    = "<=";
	const std::string EQUAL              = "=";
	const std::string NOTEQUAL           = "!=";
	const std::string AND                = "AND";
	const std::string OR                 = "OR";
	const std::string XOR                = "XOR";
	const std::string NOT                = "NOT";
	const std::string MULTIPLY           = "*";
	const std::string DIVIDE             = "/";
	const std::string ADDITION           = "+";
	const std::string SUBTRACTION        = "-";
	const std::string POWER              = "^";
	const std::string SQRT               = "SQRT";
	const std::string FABS				 = "FABS";


	//constant used to parse token
	const std::string INTEGER_NUM       = "0123456789";
	const std::string REAL_NUM          = "0123456789.";
	const std::string OPERATOR          = "*/+-^<>=";
	const std::string TRUE_CONSTANT     = "TRUE";
	const std::string FALSE_CONSTANT    = "FALSE";
}


#endif