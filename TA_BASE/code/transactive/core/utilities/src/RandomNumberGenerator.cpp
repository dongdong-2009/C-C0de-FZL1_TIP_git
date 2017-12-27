/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/RandomNumberGenerator.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "RandomNumberGenerator.h"
#include <algorithm>

namespace TA_Base_Core
{

    DiscreteProbabilityDistribution::DiscreteProbabilityDistribution() :
        m_totalP(0)
    {
    }

    void DiscreteProbabilityDistribution::bucket_push_back(int Pn)
    {
        m_cumProb.push_back(Pn + m_totalP);
        m_totalP += Pn;
    }

    unsigned int DiscreteProbabilityDistribution::size()
    {
        return m_cumProb.size();
    }

    int DiscreteProbabilityDistribution::generate()
    {
        TA_Assert(m_cumProb.size() > 0);

        int u = generateUniformRandom<int>(0, m_totalP);
        CumProbVec_t::iterator iter = std::upper_bound(m_cumProb.begin(), m_cumProb.end(), u);
        unsigned int index = iter - m_cumProb.begin();
        TA_Assert(index < m_cumProb.size());

        return index;
    }

}   // namespace TA_Base_Core


