#ifndef ME_I_CONTEXT_H
#define ME_I_CONTEXT_H

namespace TA_Base_Bus
{
	class IContext
	{
	public:
		IContext(){}
		virtual ~IContext(){}
		
        virtual bool getBooleanValue(const std::string& name) = 0;
        virtual double getRealNumberValue(const std::string& name) = 0;
        virtual std::string getStringValue(const std::string& name) = 0;
        virtual bool isBooleanVariable(const std::string& name) = 0;
        virtual bool isRealNumberVariable(const std::string& name) = 0;
        virtual bool isStringVariable(const std::string& name) = 0;
	};
}

#endif