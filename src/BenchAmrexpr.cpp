#include "BenchAmrexpr.h"
#include "amrexpr/amrexpr.hpp"

#ifdef _OPENMP
#include <omp.h>
#endif

//-------------------------------------------------------------------------------------------------
BenchAmrexpr::BenchAmrexpr(int nthreads)
: Benchmark(), m_nthreads(nthreads)
{
    m_sName = "amrexpr";
    if (m_nthreads > 1) {
        m_sName.append(" (").append(std::to_string(m_nthreads)).append(" threads)");
    }
}

//-------------------------------------------------------------------------------------------------
double BenchAmrexpr::DoBenchmark(const std::string& sExpr, long iCount)
{
    double a = 1.1;
    double b = 2.2;
    double c = 3.3;
    double x = 2.123456;
    double y = 3.123456;
    double z = 4.123456;
    double w = 5.123456;

    // Perform basic tests for the variables used
    // in the expressions
    {
        bool test_result = true;

        auto tests_list = test_expressions();

        for (auto test : tests_list)
        {
            amrexpr::Parser parser(test.first);
            parser.registerVariables({"a","b","c","x","y","z","w"});
            auto exe = parser.compile<7>();

            if (!is_equal(test.second, exe(a,b,c,x,y,z,w)))
            {
                test_result = false;
                break;
            }
        }

        if (!test_result)
        {
            StopTimer(std::numeric_limits<double>::quiet_NaN(),
                      std::numeric_limits<double>::quiet_NaN(),
                      1);
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    amrexpr::Parser parser(sExpr);
    parser.registerVariables({"a","b","c","x","y","z","w"});
    parser.setConstant("pi", M_PI);
    parser.setConstant("e", M_E);
    auto exe = parser.compile<7>();

    //Prime the I and D caches for the expression
    {
        double d0 = 0.0;
        double d1 = 0.0;

        for (std::size_t i = 0; i < priming_rounds; ++i)
        {
            if (i & 1)
                d0 += exe(a,b,c,x,y,z,w);
            else
                d1 += exe(a,b,c,x,y,z,w);
        }

        if (
            (d0 == std::numeric_limits<double>::infinity()) &&
            (d1 == std::numeric_limits<double>::infinity())
            )
        {
            printf("\n");
        }
    }

    // Perform benchmark then return results
    double fRes = 0;
    double fSum = 0;

    fRes = exe(a,b,c,x,y,z,w);

    StartTimer();

#ifdef _OPENMP
#pragma omp parallel for firstprivate(a,b,x,y) schedule(static,128) \
    num_threads(m_nthreads) reduction(+:fSum) if (m_nthreads > 1)
#endif
    for (int j = 0; j < iCount; ++j)
    {
        fSum += exe(a,b,c,x,y,z,w);
        std::swap(a,b);
        std::swap(x,y);
    }

    StopTimer(fRes, fSum, iCount);

    return m_fTime1;
}
