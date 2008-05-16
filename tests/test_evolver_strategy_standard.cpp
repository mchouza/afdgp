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
// test_evolver_strategy_standard.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 13 de mayo de 2008
//=============================================================================

#include "cached_eval_module.h"
#include "common.h"
#include "config.h"
#include "eval_module.h"
#include "evolver_strategy.h"
#include "evolver_strategy_factory.h"
#include "module_library.h"
#include "ops_module.h"
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace
{
	using std::map;
	using std::string;
	using std::vector;

	const size_t NUM_INDIV = 10000;
	
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

	class HardcodedConfig : public Core::Config
	{
		map<string, string> data_;
	public:
		HardcodedConfig()
		{
			data_["MutRate"] = "0.10";
			data_["CrossRate"] = "0.50";
			data_["ArchModRate"] = "0.15";
			data_["FitnessSelLambda"] = "3.0";
		}
		virtual string readValue(const string& key) const
		{
			map<string, string>::const_iterator it(data_.find(key));
			if (it == data_.end())
				throw;
			else
				return it->second;
		}
		virtual string readValue(const string& key, 
			const string& defaultValue) const
		{
			if (hasKey(key))
				return readValue(key);
			else
				return defaultValue;
		}
		virtual bool hasKey(const string& key) const
		{
			return data_.find(key) != data_.end();
		}
		virtual vector<string> getKeys() const
		{
			vector<string> ret;
			map<string, string>::const_iterator it(data_.begin()), 
				itEnd(data_.end());
			for (; it != itEnd; ++it)
				ret.push_back(it->first);
			return ret;
		}
		virtual map<string, string> getKeyValuePairs() const
		{
			return data_;
		}
	};
}

#ifdef TEST_EVOLVER_STRATEGY_STANDARD

int main()
{
	using boost::shared_dynamic_cast;
	using boost::shared_ptr;
	using GP::CachedEvalModule;
	using GP::EvolverStrategy;
	using GP::EvolverStrategyFactory;
	using std::cout;
	using std::map;
	using std::min;
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

	cout << "Módulo de evaluación: " << pEM->getName() << "\n";
	cout << "Módulo de operaciones: " << pOM->getName() << "\n";

	cout << "Generando población...\n";
	startTime = clock();
	TPop basePop = generatePopulation(*pOM);
	TPop pop(basePop);
	endTime = clock();
	cout << "Se han generado los " << pop.size() << " individuos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	std::vector<double> scores;
	
	cout << "Evaluando la población...\n";
	startTime = clock();
	scores = evaluatePopulation(*pEM, pop);
	endTime = clock();
	cout << "Se han evaluado los " << pop.size() << " individuos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	cout << "El mejor puntaje es "
		<< *min_element(scores.begin(), scores.end()) << " y el peor es "
		<< *max_element(scores.begin(), scores.end()) << ".\n";

	HardcodedConfig c;
	shared_ptr<EvolverStrategy> pES = EvolverStrategyFactory::make("Standard",
		c);

	cout << "Realizando un paso evolutivo...\n";
	startTime = clock();
	pES->evolutionaryStep(pop, *pEM, *pOM);
	endTime = clock();
	cout << "Se ha terminado el paso evolutivo en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	cout << "El puntaje del primer elemento es " 
		<< pEM->evaluateGenome(pop.front()) << " y el del último elemento es "
		<< pEM->evaluateGenome(pop.back()) << ".\n";

	cout << "Realizando otros diez pasos evolutivos...\n";
			startTime = clock();
	for (int i = 0; i < 10; i++)
	{
		pES->evolutionaryStep(pop, *pEM, *pOM);
		cout << i << "\n";
	}
	endTime = clock();
	cout << "Se han realizado estos 10 pasos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";
	cout << "El puntaje del primer elemento es " 
		<< pEM->evaluateGenome(pop.front()) << " y el del último elemento es "
		<< pEM->evaluateGenome(pop.back()) << ".\n";

	pop = basePop;
	CachedEvalModule cEM(pEM, 1<<20);
	pES = EvolverStrategyFactory::make("Standard", c);
	cout << "Utilizando el módulo con caché...\n";
	cout << "Realizando un paso evolutivo...\n";
	startTime = clock();
	pES->evolutionaryStep(pop, cEM, *pOM);
	endTime = clock();
	cout << "Se ha terminado el paso evolutivo en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	cout << "El puntaje del primer elemento es " 
		<< cEM.evaluateGenome(pop.front()) << " y el del último elemento es "
		<< cEM.evaluateGenome(pop.back()) << ".\n";

	cout << "Realizando otros diez pasos evolutivos...\n";
			startTime = clock();
	for (int i = 0; i < 10; i++)
	{
		pES->evolutionaryStep(pop, cEM, *pOM);
		cout << i << "\n";
	}
	endTime = clock();
	cout << "Se han realizado estos 10 pasos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";
	cout << "El puntaje del primer elemento es " 
		<< cEM.evaluateGenome(pop.front()) << " y el del último elemento es "
		<< cEM.evaluateGenome(pop.back()) << ".\n";
	cout << "Realizando otros diez pasos evolutivos adicionales...\n";
			startTime = clock();
	for (int i = 0; i < 10; i++)
	{
		pES->evolutionaryStep(pop, cEM, *pOM);
		cout << i << "\n";
	}
	endTime = clock();
	cout << "Se han realizado estos 10 pasos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";
	cout << "El puntaje del primer elemento es " 
		<< cEM.evaluateGenome(pop.front()) << " y el del último elemento es "
		<< cEM.evaluateGenome(pop.back()) << ".\n";
}

#endif
