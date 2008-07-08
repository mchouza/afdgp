#include "common.h"
#include "cached_eval_module.h"
#include "eval_module.h"
#include "mm2_wrapper.h"
#include "module_library.h"
#include "ops_module.h"
#include "testing_cached_eval_module.h"

#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>

uint32_t SuperFastHash (const unsigned char * data, int len, uint32_t seed);
uint64_t fnv_64a_buf(const void *buf, size_t len, uint64_t hval);

namespace
{
	using std::map;
	using std::string;
	using std::vector;

	const size_t NUM_INDIV = 100000;
	const uint64_t FNV_SEED = 0xcbf29ce484222325;
	const uint32_t HSIEH_SEED_1 = 0;
	const uint32_t HSIEH_SEED_2 = 0x37b05be6;
	
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

	template <typename T>
	uint64_t hsiehWrapper(const vector<T>& buffer)
	{
		if (buffer.empty())
			return FNV_SEED;
		uint64_t ret = SuperFastHash(
			reinterpret_cast<const unsigned char*>(&buffer[0]),
			static_cast<int>(buffer.size() * sizeof(T)),
			HSIEH_SEED_1);
		ret <<= 32;
		ret |= SuperFastHash(
			reinterpret_cast<const unsigned char*>(&buffer[0]),
			static_cast<int>(buffer.size() * sizeof(T)),
			HSIEH_SEED_2);
		return ret;
	}

	template <typename T>
	uint64_t fnvWrapper(const vector<T>& buffer)
	{
		if (buffer.empty())
			return FNV_SEED;
		return fnv_64a_buf(&buffer[0],
			static_cast<int>(buffer.size() * sizeof(T)),
			FNV_SEED);
	}
}

#ifdef TEST_HASH_REAL_POP

void testCachedEvalModule(EvalModule& cEM, EvalModule& ucEM, const TPop& pop)
{
	using std::cout;
	using std::vector;
	
	vector<double> scores[4];
	
	for (int i = 0; i < 3; i++)
	{
		cout << i << ". Evaluando población...\n";
		clock_t startTime = clock();
		scores[i] = evaluatePopulation(cEM, pop);
		clock_t endTime = clock();
		cout << "Se han evaluado los " << pop.size() << " individuos en "
			<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";
	}

	cout << "Evaluando con módulo sin caché...\n";
	clock_t startTime = clock();
	scores[3] = evaluatePopulation(ucEM, pop);
	clock_t endTime = clock();
	cout << "Se han evaluado los " << pop.size() << " individuos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	cout << "Comparando puntajes...\n";
	if (scores[0] == scores[1] && scores[1] == scores[2] && 
		scores[2] == scores[3])
		cout << "Los resultados coinciden.\n";
	else
		cout << "Los resultados no coinciden.\n";
}

int main()
{
	using boost::shared_dynamic_cast;
	using boost::shared_ptr;
	using GP::CachedEvalModule;
	using std::cout;
	using std::map;
	using std::string;

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

	cout << "Generando población...\n";
	clock_t startTime = clock();
	TPop pop = generatePopulation(*pOM);
	clock_t endTime = clock();
	cout << "Se han generado los " << pop.size() << " individuos en "
		<< double(endTime - startTime) / CLOCKS_PER_SEC << " segundos.\n";

	{
		std::cout << "Testing FNV-1a...\n";
		TestingCachedEvalModule cEM(pEM, (1 << 20), fnvWrapper);
		testCachedEvalModule(cEM, *pEM, pop);
		std::cout << "\n\n";
	}

	{
		std::cout << "Testing SuperFastHash...\n";
		TestingCachedEvalModule cEM(pEM, (1 << 20), hsiehWrapper);
		testCachedEvalModule(cEM, *pEM, pop);
		std::cout << "\n\n";
	}

	{
		std::cout << "Testing MurmurHash 2.0...\n";
		CachedEvalModule cEM(pEM, (1 << 20));
		testCachedEvalModule(cEM, *pEM, pop);
		std::cout << "\n\n";
	}

	return 0;
}

#endif
