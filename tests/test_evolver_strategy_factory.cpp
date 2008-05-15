//
// Copyright (c) 2008, Mariano M. Chouza
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, 
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * The names of the contributors may not be used to endorse or promote
//      products derived from this software without specific prior written
//      permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

//=============================================================================
// test_evolver_strategy_factory.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 30 de abril de 2008
//=============================================================================

#include "common.h"
#include "eval_module.h"
#include "evolver_strategy.h"
#include "evolver_strategy_factory.h"
#include "module_library.h"
#include "ops_module.h"
#include "registrator.h"
#include "stats_collector.h"
#include <iostream>

using namespace GP;

class NullStatsCollector : public StatsCollector
{
	virtual void printStatsByGenHeader(std::ostream& os) const
	{
	}
	virtual void printStatsByGenRow(std::ostream& os) const
	{
	}
	virtual void printGlobalStats(std::ostream& os) const
	{
	}
};

class TestES : public EvolverStrategy
{
	TestES()
	{
	}

	NullStatsCollector nsc_;

public:
	virtual void evolutionaryStep(TPop& pop, EvalModule& evalMod,
				OpsModule& opsMod)
	{
		std::cout << "evolutionaryStep() de TestES\n";
	}

	static boost::shared_ptr<EvolverStrategy> create(const Core::Config&)
	{
		return boost::shared_ptr<EvolverStrategy>(new TestES());
	}

	virtual const StatsCollector& getStatsCollector() const
	{
		return nsc_;
	}
};

class TestES2 : public EvolverStrategy
{
	TestES2()
	{
	}

	NullStatsCollector nsc_;

public:
	virtual void evolutionaryStep(TPop& pop, EvalModule& evalMod,
				OpsModule& opsMod)
	{
		std::cout << "evolutionaryStep() de TestES2\n";
	}

	static boost::shared_ptr<EvolverStrategy> create(const Core::Config&)
	{
		return boost::shared_ptr<EvolverStrategy>(new TestES2());
	}

	virtual const StatsCollector& getStatsCollector() const
	{
		return nsc_;
	}
};

namespace
{
	using std::map;
	using std::string;
	
	Util::Registrator<EvolverStrategyFactory, TestES> r("TEST_ES");
	Util::Registrator<EvolverStrategyFactory, TestES2> r2("TEST_ES_2");

	map<string, string> makeEMConfigMap()
	{
		map<string, string> ret;

		ret["InitialEmbryo"] =
			"TITLE\nV1 0 1 1\nR1 1 2 1K\nZ1 2 3 X\nR2 3 0 1K\nP1 3 0 X\n";
		ret["Tolerance"] = "50";
		ret["ProbedNode"] = "3";
		ret["DesiredTransfer"] = "0 0.5 10000 0.5 10010 0 1000000 0";
		ret["PenaltyZero"] = "1000";
		ret["PenaltyMultiplier"] = "0.005";
		ret["StartFreq"] = "1";
		ret["EndFreq"] = "100000";
		ret["PointsPerDec"] = "10";

		return ret;
	}
}

#ifdef TEST_ESF

int main()
{
	using boost::shared_dynamic_cast;
	using boost::shared_ptr;
	using GP::EvolverStrategy;
	using GP::EvolverStrategyFactory;

	shared_ptr<EvolverStrategy> p, q;

	p = EvolverStrategyFactory::make("TEST_ES");
	q = EvolverStrategyFactory::make("TEST_ES_2");

	TPop pop;
#ifdef NDEBUG
	Core::ModuleLibrary lib("../release");
#else
	Core::ModuleLibrary lib("../debug");
#endif
	shared_ptr<EvalModule> pEM = shared_dynamic_cast<EvalModule, Module>(
		lib.getModuleByName("EvalPassiveAnalogFilter"));
	shared_ptr<OpsModule> pOM = shared_dynamic_cast<OpsModule, Module>(
		lib.getModuleByName("GenOpsPassiveAnalogFilter"));
	shared_ptr<Module> pSM = lib.getModuleByName("Spice");
	pEM->giveReqMods(std::vector<shared_ptr<Module> >(1, pSM));
	pEM->giveConfigData(makeEMConfigMap());

	p->evolutionaryStep(pop, *pEM, *pOM);
	q->evolutionaryStep(pop, *pEM, *pOM);
	p.swap(q);
	p->evolutionaryStep(pop, *pEM, *pOM);
	q->evolutionaryStep(pop, *pEM, *pOM);
}

#endif
