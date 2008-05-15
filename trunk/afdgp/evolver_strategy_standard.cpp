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
// evolver_strategy_standard.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de mayo de 2008
//=============================================================================

#include "evolver_strategy_standard.h"
#include "config.h"
#include "evolver_strategy_factory.h"
#include "registrator.h"
#include "stats_collector.h"
#include <boost/lexical_cast.hpp>
#include <cassert>

using namespace GP;

namespace
{
	using boost::exponential_distribution;
	using boost::lexical_cast;
	using boost::mt19937;
	using boost::uniform_real;
	using boost::variate_generator;
	using std::map;
	using std::pair;
	using std::string;
	using std::vector;
	using Util::ExpRandomGenerator;
	using Util::UniformRandomGenerator;
	
	// Registra la clase
	Util::Registrator<EvolverStrategyFactory, EvolverStrategyStandard>
		r("Standard");
	
	/// Evalúa la población y devuelve sus puntajes ordenados
	vector<pair<double, size_t> > evalPop(const TPop& pop, 
		const EvalModule& evalMod)
	{
		// Armo el vector de puntajes e índices
		vector<pair<double, size_t> > scores(pop.size());
		for (size_t i = 0; i < pop.size(); i++)
		{
			scores[i].first = evalMod.evaluateGenome(pop[i]);
			scores[i].second = i;
		}

		// Ordena los puntajes
		sort(scores.begin(), scores.end());

		// Los devuelve
		return scores;
	}

	/// Ordena la población de acuerdo a los puntajes
	void sortPopByScores(TPop& pop, 
		const vector<pair<double, size_t> >& sortedScores)
	{
		TPop sortedPop(pop.size());
		for (size_t i = 0; i < sortedPop.size(); i++)
			sortedPop[i].swap(pop[sortedScores[i].second]);
		pop.swap(sortedPop);
	}
	
	/// Lee del map o lanza una excepción
	template <typename T>
	T readMapOrThrow(const map<string, string>& m, const std::string& key)
	{
		map<string, string>::const_iterator it = m.find(key);
		if (it == m.end())
			throw; // FIXME: Lanzar algo más específico...
		return lexical_cast<T>(it->second);
	}

	/// Parámetros para obtener una nueva población
	struct PopSelParams
	{
		double crossRate;
		double mutRate;
		double archModRate;
		double fitnessSelLambda;
		
		PopSelParams(const map<string, string>& c) :
		crossRate(readMapOrThrow<double>(c, "CrossRate")),
		mutRate(readMapOrThrow<double>(c, "MutRate")),
		archModRate(readMapOrThrow<double>(c, "ArchModRate")),
		fitnessSelLambda(readMapOrThrow<double>(c, "FitnessSelLambda"))
		{
			assert(crossRate + mutRate + archModRate < 1.0);
		}
	};

	/// Obtiene un nuevo individuo
	TGenome getNewIndividual(const TPop& oldPop, const PopSelParams& params,
		OpsModule& opsMod, ExpRandomGenerator& eRG, 
		UniformRandomGenerator& uRG)
	{
		// Obtengo un valor aleatorio entre 0 y 1
		double r = uRG.getRandomFloat(0.0, 1.0);

		// Selecciono un individuo al azar con distribución exponencial
		double realLambda = params.fitnessSelLambda / oldPop.size();
		TGenome ret = oldPop[eRG.getRandomInteger(oldPop.size(), realLambda)];

		// Realizo la operación que corresponda con al probabilidad que 
		// corresponda
		if (r < params.archModRate)
		{
			opsMod.altOp(ret);
		}
		else if (r < params.archModRate + params.crossRate)
		{
			TGenome dummy = 
				oldPop[eRG.getRandomInteger(oldPop.size(), realLambda)];
			opsMod.cross(ret, dummy);
		}
		else if (r < params.archModRate + params.crossRate + params.mutRate)
		{
			opsMod.mutate(ret);
		}

		return ret;
	}

	/// Recolector de estadísticas para la estrategia estándar
	class StandardStrategyStatsCollector : public StatsCollector
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
}

EvolverStrategyStandard::EvolverStrategyStandard(const Core::Config& c) :
configParams_(c.getKeyValuePairs()),
pSC_(new StandardStrategyStatsCollector())
{
}

boost::shared_ptr<EvolverStrategy> 
EvolverStrategyStandard::create(const Core::Config& c)
{
	return boost::shared_ptr<EvolverStrategy>(
		new EvolverStrategyStandard(c));
}

void EvolverStrategyStandard::evolutionaryStep(TPop& pop, EvalModule& evalMod,
											   OpsModule& opsMod)
{
	// Ordeno la población en forma de puntaje creciente
	vector<pair<double, size_t> > sortedScores = evalPop(pop, evalMod);
	sortPopByScores(pop, sortedScores);

	// Armo la nueva población que siempre incorpora al mejor individuo de la
	// anterior
	TPop newPop(pop.size());
	newPop[0] = pop[0];

	// Obtengo los parámetros
	PopSelParams params(configParams_);

	// Relleno el resto
	for (size_t i = 1; i < newPop.size(); i++)
		newPop[i] = getNewIndividual(pop, params, opsMod, expRandomGen_,
			uniformRandomGen_);

	// Me quedo con la nueva población
	pop.swap(newPop);
}

const StatsCollector& EvolverStrategyStandard::getStatsCollector() const
{
	return *pSC_;
}