#if !defined(NameValuePair_486B4473_8472_4781_8430_650410E73806__INCLUDED_)
#define NameValuePair_486B4473_8472_4781_8430_650410E73806__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/message/src/NameValuePair.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is used for subscription information. Each Struct represents a filterable
  * name, and the value that it must be.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;

namespace TA_Base_Core
{

	struct NameValuePair
	{
		// What type of evaluation
		enum EvaluationType
		{
			EQUALS = 0,
			NOT_EQUALS
		};

		std::string name;
		std::string value;
        EvaluationType evalType;

        NameValuePair(const std::string& filterName, const std::string& filterValue)
            : name(filterName),
              value(filterValue),
              evalType(EQUALS)
        {
        }

        NameValuePair(const std::string& filterName, EvaluationType eval, const std::string& filterValue)
            : name(filterName),
              value(filterValue),
              evalType(eval)
        {
        }

		~NameValuePair() {}
	}; // NameValuePair

    typedef std::vector<NameValuePair*> FilterData;
    typedef std::vector<NameValuePair*> DescriptionParameters;
    typedef std::vector<NameValuePair*>::iterator DescriptionParametersIterator;
    typedef std::vector<NameValuePair*>::const_iterator ConstDescriptionParametersIterator;

    struct DescriptionParametersWrap
    {
        std::vector<NameValuePair> m_vp;
        std::vector<NameValuePair*> m_vpp;
        DescriptionParametersWrap() {}

        template<typename T>
        DescriptionParametersWrap(const T& vs)
        {
            assign(vs);
        }

        template<typename T>
        DescriptionParametersWrap& operator = (const T& vs)
        {
            assign(vs);
            return *this;
        }

        template<typename T>
        void assign(const T& vs)
        {
            m_vp.clear();
            m_vpp.clear();
            const std::vector< boost::tuple<std::string, std::string> >& v = vs;

            for (size_t i = 0; i < v.size(); ++i)
            {
                m_vp.push_back(NameValuePair(boost::get<0>(v[i]), boost::get<1>(v[i])));
                m_vpp.push_back(&m_vp.back());
            }
        }

        operator std::vector<NameValuePair*>& ()
        {
            return m_vpp;
        }

        operator const std::vector<NameValuePair*>& () const
        {
            return m_vpp;
        }
    };
}; // TA_Base_Core

#endif // !defined(NameValuePair_486B4473_8472_4781_8430_650410E73806__INCLUDED_)
