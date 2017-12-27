/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/RandomNumberGenerator.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if !defined( RANDOMNUMBERGENERATOR_H )
#define RANDOMNUMBERGENERATOR_H

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <ctime>
#include <cstdlib> 

namespace TA_Base_Core
{
    // Generates a uniformly distributed random number in the range [0, RAND_MAX].
    // TODO LPP: RAND_MAX is 0x7fff on Windows, which unfortunately wastes one bit.
    inline int generate()
    {
        static bool s_seededRNG = false;
        if (!s_seededRNG)
        {
            srand( (unsigned)time(NULL) );
            s_seededRNG = true;
        }

        return rand();
    }

    // Generate uniformly distributed random numbers in the range [minVal, maxVal).
    template <class T>
    T generateUniformRandom(T minVal, T maxVal)
    {
        TA_Assert(maxVal > minVal);

        // |----------+------|
        // 0          u     RAND_MAX
        // minVal     v     maxVal
        //
        //  v - minVal        u - 0
        // ------------- = ------------
        // maxVal-minVal   RAND_MAX - 0

        // Although the scaling from u -> v is trivial, it is difficult to avoid overflow (for 
        // integral types) and roundoff problems.  We will normalise the random number (ie. so that
        // it is in the range [0.0, 1.0] and scale using double precision floating point arithmetic,
        // then convert back to T.
        // TODO LPP: Is there a better solution?

        double v = 0.0;
        do
        {
            double u = (double) generate();
            v = u * ((double)maxVal - (double)minVal) / (double)RAND_MAX + (double)minVal;
        }
        while (v >= maxVal);

        // TODO LPP: This doesn't work too well if T is bool
        T vT = (T)v;
        TA_Assert(minVal <= v && v <= maxVal);
        return vT;
    }

    class DiscreteProbabilityDistribution
    {
    public:
        DiscreteProbabilityDistribution();

        // Add a bucket to the probability distribution.  The probability of the bucket being
        // chosen will be Pn / SUM(P0 + P1 + ... + Pn).  Therefore the probability will change
        // as buckets are being added.
        void bucket_push_back(int Pn);
    
        // Returns the number of buckets that have been added.
        unsigned int size();
    
        // Select a bucket at random.
        // At least one bucket must have been added.
        int generate();

    private:
        // Vector of cumulative probabilities.  ie. if n buckets have been added with relative
        // likelihoods (P0, P1, ..., Pn), the cumulative probabilities of the buckets will be 
        // (0, P0, P0+P1, ..., P0+P1+...+Pn)
        typedef std::vector<int> CumProbVec_t;
        CumProbVec_t m_cumProb;
        int m_totalP;
    };

};  // namespace TA_Base_Core

#endif   // RANDOMNUMBERGENERATOR_H
