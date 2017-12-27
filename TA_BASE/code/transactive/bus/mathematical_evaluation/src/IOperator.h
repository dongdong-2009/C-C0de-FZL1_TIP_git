#ifndef I_OPERATOR_H
#define I_OPERATOR_H

#include "bus/mathematical_evaluation/src/MEcommonDef.h"
#include "bus/mathematical_evaluation/src/MEOperand.h"
#include <stack>
#include <string>
namespace TA_Base_Bus
{
	class IOperator
	{
	public:
		IOperator(const std::string& identify)
			:
		m_identify(identify)
		{}

		virtual ~IOperator(){}

		//evaluate and push result back to operands
		virtual void evaluate(std::stack<MEOperand>& operands) = 0;
		
		//verify whether operands satisfy this operator's semantic
		virtual void verifySemantic(std::stack<MEOperand>& operands) = 0;

		std::string getIdentify()
		{
			return m_identify;
		}

	private:	
		std::string m_identify;
	};

	class AddOperator : public IOperator
	{
	public:
		AddOperator()
			:IOperator(TA_Base_Bus::ADDITION)
		{
		}

		~AddOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class SubOperator : public IOperator
	{
	public:
		SubOperator()
			:IOperator(TA_Base_Bus::SUBTRACTION)
		{
		}

		~SubOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class MultiplyOperator : public IOperator
	{
	public:
		MultiplyOperator()
			:IOperator(TA_Base_Bus::MULTIPLY)
		{
		}

		~MultiplyOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class DivisionOperator : public IOperator
	{
	public:
		DivisionOperator()
			:IOperator(TA_Base_Bus::DIVIDE)
		{
		}

		~DivisionOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class EqualOperator : public IOperator
	{
	public:
		EqualOperator()
			:IOperator(TA_Base_Bus::EQUAL)
		{
		}

		~EqualOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class NotEqualOperator : public IOperator
	{
	public:
		NotEqualOperator()
			:IOperator(TA_Base_Bus::NOTEQUAL)
		{
		}

		~NotEqualOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class GreaterThanOperator : public IOperator
	{
	public:
		GreaterThanOperator()
			:IOperator(TA_Base_Bus::GREATERTHAN)
		{
		}

		~GreaterThanOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class LessThanOperator : public IOperator
	{
	public:
		LessThanOperator()
			:IOperator(TA_Base_Bus::LESSTHAN)
		{
		}

		~LessThanOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class GreaterThanOrEqualOperator : public IOperator
	{
	public:
		GreaterThanOrEqualOperator()
			:IOperator(TA_Base_Bus::GREATERTHANOREQUAL)
		{
		}

		~GreaterThanOrEqualOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class LessThanOrEqualOperator : public IOperator
	{
	public:
		LessThanOrEqualOperator()
			:IOperator(TA_Base_Bus::LESSTHANOREQUAL)
		{
		}

		~LessThanOrEqualOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class PowerOperator : public IOperator
	{
	public:
		PowerOperator()
			:IOperator(TA_Base_Bus::POWER)
		{
		}

		~PowerOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class FabsOperator : public IOperator
	{
	public:
		FabsOperator()
			:IOperator(TA_Base_Bus::FABS)
		{
		}

		~FabsOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class SqrtOperator : public IOperator
	{
	public:
		SqrtOperator()
			:IOperator(TA_Base_Bus::SQRT)
		{
		}

		~SqrtOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class AndOperator : public IOperator
	{
	public:
		AndOperator()
			:IOperator(TA_Base_Bus::AND)
		{
		}

		~AndOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class OrOperator : public IOperator
	{
	public:
		OrOperator()
			:IOperator(TA_Base_Bus::OR)
		{
		}

		~OrOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class XorOperator : public IOperator
	{
	public:
		XorOperator()
			:IOperator(TA_Base_Bus::XOR)
		{
		}

		~XorOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};

	class NotOperator : public IOperator
	{
	public:
		NotOperator()
			:IOperator(TA_Base_Bus::NOT)
		{
		}

		~NotOperator()
		{
		}

		virtual void evaluate(std::stack<MEOperand>& operands);
		virtual void verifySemantic(std::stack<MEOperand>& operands);
	};
}

#endif
