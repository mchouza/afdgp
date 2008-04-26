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
// test_cached_eval.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 25 de abril de 2008
//=============================================================================

#include "cached_eval_module.h"
#include "common.h"
#include "eval_module.h"
#include "module_library.h"
#include "ops_module.h"
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <vector>

namespace
{
	using std::map;
	using std::string;
	using std::vector;

	const size_t NUM_INDIV = 100000;
	
	TPop generatePopulation(const OpsModule& opsMod)
	{
		TPop ret(NUM_INDIV);
		for (size_t i = 0; i < ret.size(); i++)
			opsMod.randomInit(ret[i]);
		return ret;
	}

	vector<double> evaluatePopulation(const EvalModule& evalMod, 
		const TPop& pop)
	{
		using std::cout;
		vector<double> score(pop.size());
		for (size_t i = 0; i < score.size(); i++)
		{
			if (i % (score.size() / 10) == 0)
				cout << (i * 100) / score.size() << "%\n";
			score[i] = evalMod.evaluateGenome(pop[i]);
		}
		return score;
	}

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

#ifdef TEST_CACHED_EVAL

int main()
{
	using boost::shared_dynamic_cast;
	using boost::shared_ptr;
	using std::cout;
	using std::map;
	using std::string;

	time_t startTime, endTime;

#ifdef NDEBUG
	Core::ModuleLibrary lib("../release");
#else
	Core::ModuleLibrary lib("../debug");
#endif
	lib.dump(cout);

	shared_ptr<EvalModule> pEM = shared_dynamic_cast<EvalModule, Module>(
		lib.getModuleByName("EvalPassiveAnalogFilter"));
	shared_ptr<OpsModule> pOM = shared_dynamic_cast<OpsModule, Module>(
		lib.getModuleByName("GenOpsPassiveAnalogFilter"));
	shared_ptr<Module> pSM = lib.getModuleByName("Spice");
	pEM->giveReqMods(std::vector<shared_ptr<Module> >(1, pSM));

	pEM->giveConfigData(makeEMConfigMap());

	CachedEvalModule cEM(pEM, (1 << 20));

	cout << "Módulo de evaluación: " << pEM->getName() << "\n";
	cout << "Módulo de operaciones: " << pOM->getName() << "\n";

	cout << "Generando población...\n";
	startTime = clock();
	TPop pop = generatePopulation(*pOM);
	endTime = clock();
	cout << "Se han generado los " << pop.size() << " individuos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	std::vector<double> scores[4];
	
	for (int i = 0; i < 3; i++)
	{
		cout << i << ". Evaluando población...\n";
		startTime = clock();
#if 0
		evaluatePopulation(*pEM, pop);
#else
		scores[i] = evaluatePopulation(cEM, pop);
#endif
		endTime = clock();
		cout << "Se han evaluado los " << pop.size() << " individuos en "
			<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";
	}

	cout << "Evaluando con módulo sin caché...\n";
	startTime = clock();
	scores[3] = evaluatePopulation(*pEM, pop);
	endTime = clock();
	cout << "Se han evaluado los " << pop.size() << " individuos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	cout << "Comparando resultados...\n";
	if (scores[0] == scores[1] && scores[1] == scores[2] && scores[2] == scores[3])
		cout << "Los resultados coinciden.\n";
	else
		cout << "Los resultados difieren.\n";

	return 0;
}

#endif
