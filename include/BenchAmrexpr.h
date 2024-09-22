#ifndef BENCH_AMREXPR_H
#define BENCH_AMREXPR_H

#include <vector>
#include <string>

#include "Benchmark.h"

//-------------------------------------------------------------------------------------------------
class BenchAmrexpr : public Benchmark
{
public:

   explicit BenchAmrexpr(int nthreads = 1);

   double DoBenchmark(const std::string& sExpr, long iCount);

private:
   int m_nthreads = 1;
};

#endif
