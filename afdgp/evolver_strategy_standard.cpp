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
#include "escape.h"
#include "evolver_strategy_factory.h"
#include "registrator.h"
#include "stats_collector.h"
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <ctime>
#include <sstream>

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

	/// Calcula el promedio de una serie de valores
	template <typename RetType, typename InIt>
	RetType average(InIt it, InIt end)
	{
		RetType ret = RetType(0);
		size_t num = 0;
		for (; it != end; ++it)
		{
			ret += RetType(*it);
			++num;
		}
		return ret / RetType(num);
	}

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
			throw 0; // FIXME: Lanzar algo más específico...
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
}

/// Recolector de estadísticas para la estrategia estándar
class EvolverStrategyStandard::StatsCollector : public GP::StatsCollector
{
	/// Referencia a la fuente de datos
	const EvolverStrategyStandard& dataSrc_;

	/// Encabezados en orden
	std::vector<std::string> genDataHeaders_;

	/// Datos recolectados por generación
	std::map<std::string, std::string> genData_;

	/// Datos recolectados globalmente
	std::map<std::string, std::string> globalData_;

	/// Ejecución de la que tengo datos recolectados
	int dataRun_;

	/// Generación de la que tengo datos recolectados
	int dataGen_;

	/// Revisa los datos recolectados para ver si hay que tirarlos
	void checkGenData()
	{
		using boost::lexical_cast;
		using std::map;
		using std::string;
		
		if (dataSrc_.gen_ != dataGen_ || dataSrc_.run_ != dataRun_)
		{
			map<string, string>::iterator it(genData_.begin()), 
				itEnd(genData_.end());
			for (; it != itEnd; ++it)
				it->second = "X";
		}
		
		dataGen_ = dataSrc_.gen_;
		dataRun_ = dataSrc_.run_;

		genData_["Run"] = lexical_cast<std::string>(dataRun_);
		genData_["Gen"] = lexical_cast<std::string>(dataGen_);
	}

	/// Agrega un encabezado a los datos por generación
	void addHeader(const std::string& header)
	{
		genDataHeaders_.push_back(header);
		genData_[header] = "X";
	}

public:
	/// Constructor
	StatsCollector(const EvolverStrategyStandard& dataSrc) :
	dataGen_(-1),
	dataRun_(-1),
	dataSrc_(dataSrc)
	{
		addHeader("Run");
		addHeader("Gen");
		addHeader("MaxLen");
		addHeader("MinLen");
		addHeader("AvgLen");
		addHeader("MaxScore");
		addHeader("MinScore");
		addHeader("AvgScore");
		addHeader("TimeToNext");
	}

	virtual void printGeneralParameters(std::ostream& os) const
	{
		using std::map;
		using std::string;

		// Imprimo todos los pares clave-valor de la configuración
		const map<string, string>& c = dataSrc_.configParams_;
		map<string, string>::const_iterator it(c.begin()), itEnd(c.end());
		for (; it != itEnd; ++it)
			os << it->first << ": " << it->second << "\n";
	}

	virtual void printStatsByGenHeader(std::ostream& os) const
	{
		using std::string;
		using std::vector;

		vector<string>::const_iterator it(genDataHeaders_.begin()), 
			itEnd(genDataHeaders_.end());
		while (it != itEnd)
		{
			os << *it;
			++it;
			if (it != itEnd)
				os << '\t';
		}
		os << '\n';
	}

	virtual void printStatsByGenRow(std::ostream& os) const
	{
		using std::map;
		using std::string;
		using std::vector;

		vector<string>::const_iterator it(genDataHeaders_.begin()),
			itEnd(genDataHeaders_.end());
		map<string, string>::const_iterator itM;

		while (it != itEnd)
		{
			itM = genData_.find(*it);
			if (itM == genData_.end())
				throw 0; // FIXME: Lanzar algo más específico
			os << itM->second;
			++it;
			if (it != itEnd)
				os << '\t';
		}
		os << '\n';
	}

	virtual void printGlobalStats(std::ostream& os) const
	{
		map<string, string>::const_iterator it(globalData_.begin()),
			itEnd(globalData_.end());
		for (; it != itEnd; ++it)
			os << it->first << ": " << it->second << "\n";
	}

	template <typename T>
	void sendGenData(const std::string& key, T value)
	{
		using std::map;
		using std::string;
		
		// Borro lo que tengo si no está al día
		checkGenData();

		// Si no está la clave, salgo
		map<string, string>::iterator it = genData_.find(key);
		if (it == genData_.end())
			throw 0; // FIXME: Lanzar algo más específico

		// Si está, guardo los datos
		it->second = boost::lexical_cast<std::string>(value);
	}

	void sendScores(const std::vector<std::pair<double, size_t> >&
		sortedScores)
	{
		std::vector<double> onlyScores(sortedScores.size());
		for (size_t i = 0; i < sortedScores.size(); i++)
			onlyScores[i] = sortedScores[i].first;

		sendGenData("MinScore", onlyScores.front());
		sendGenData("MaxScore", onlyScores.back());
		sendGenData("AvgScore", average<double>(onlyScores.begin(),
			onlyScores.end()));
	}

	void sendLengths(const TPop& sortedPop)
	{
		size_t acumPopSize = 0;
		size_t minPopSize = sortedPop[0].size();
		size_t maxPopSize = sortedPop[0].size();
		
		for (size_t i = 0; i < sortedPop.size(); i++)
		{
			size_t currSize = sortedPop[i].size();
			minPopSize = std::min(minPopSize, currSize);
			maxPopSize = std::max(maxPopSize, currSize);
			acumPopSize += currSize;
		}

		sendGenData("MaxLen", maxPopSize);
		sendGenData("MinLen", minPopSize);
		sendGenData("AvgLen", double(acumPopSize) / sortedPop.size());
	}

	void sendBestIndividual(const std::string& indiv, double score)
	{
		using boost::lexical_cast;

		if (globalData_.find("BestScoreSoFar") == globalData_.end() ||
		score < lexical_cast<double>(globalData_["BestScoreSoFar"]))
		{
			globalData_["BestScoreSoFar"] = lexical_cast<std::string>(score);
			globalData_["BestIndividual"] = Util::escapeString(indiv);
		}
	}
};

EvolverStrategyStandard::EvolverStrategyStandard(const Core::Config& c) :
configParams_(c.getKeyValuePairs()),
run_(0),
gen_(0),
pSC_()
{
	pSC_.reset(new EvolverStrategyStandard::StatsCollector(*this));
}

EvolverStrategyStandard::~EvolverStrategyStandard()
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
	// Inicio el timer
	clock_t startT = clock();
	
	// Obtengo los puntajes
	vector<pair<double, size_t> > sortedScores = evalPop(pop, evalMod);

	// Realizo la ación "pre-sort"
	preSortAction(pop, sortedScores);

	// Ordeno la población en forma de puntaje creciente
	sortPopByScores(pop, sortedScores);

	// Realizo la acción "post-sort"
	postSortAction(pop);

	// Hago el trabajo estadístico con los puntajes
	pSC_->sendScores(sortedScores);

	// Hago el trabajo estadístico con los largos
	pSC_->sendLengths(pop);

	// Armo la nueva población que siempre incorpora al mejor individuo de la
	// anterior
	TPop newPop(pop.size());
	newPop[0] = pop[0];

	// Mando al mejor individuo para fines estadísticos
	std::ostringstream oss;
	evalMod.showIndiv(oss, pop[0]);
	pSC_->sendBestIndividual(oss.str(), sortedScores[0].first);

	// Obtengo los parámetros
	PopSelParams params(configParams_);

	// Relleno el resto
	for (size_t i = 1; i < newPop.size(); i++)
		newPop[i] = getNewIndividual(pop, params, opsMod, expRandomGen_,
			uniformRandomGen_);

	// Me quedo con la nueva población
	pop.swap(newPop);

	// Mando el tiempo en cambiar a la próxima
	double deltaT = double(clock() - startT) / CLOCKS_PER_SEC;
	pSC_->sendGenData("TimeToNext", deltaT);

	// Marco el cambio de generación
	gen_++;
}

void EvolverStrategyStandard::preSortAction(TPop& pop,
	std::vector<std::pair<double, size_t> >& sortedScores)
{
	// Al ser la estrategia estándar, no realizo ninguna acción acá
}

void EvolverStrategyStandard::postSortAction(TPop& pop)
{
	// Al ser la estrategia estándar, no realizo ninguna acción acá
}

const StatsCollector& EvolverStrategyStandard::getStatsCollector() const
{
	return *pSC_;
}

void EvolverStrategyStandard::reset()
{
	// Actualizo contadores
	run_++;
	gen_ = 0;
}