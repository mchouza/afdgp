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
// evolver_strategy_standard.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de mayo de 2008
//=============================================================================

#ifndef EVOLVER_STRATEGY_STANDARD_H
#define EVOLVER_STRATEGY_STANDARD_H

#include "core_fwd.h"
#include "evolver_strategy.h"
#include "exp_random_generator.h"
#include "uniform_random_generator.h"
#include <boost/scoped_ptr.hpp>

namespace GP
{
	/// Clase de la estrategia estándar
	class EvolverStrategyStandard : public EvolverStrategy
	{
		/// Parámetros de configuración
		std::map<std::string, std::string> configParams_;

		/// Número de ejecución
		unsigned run_;

		/// Generación
		unsigned gen_;

		/// Generador de números aleatoriso con distribución exponencial
		Util::ExpRandomGenerator expRandomGen_;

		/// Generador de números aleatoriso con distribución uniforme
		Util::UniformRandomGenerator uniformRandomGen_;

		/// Clase del recolector de estadísticas
		class StatsCollector;

		/// Recolector de estadísticas
		boost::scoped_ptr<EvolverStrategyStandard::StatsCollector> pSC_;

	protected:
		/// Realiza una acción depués del sort (nula porque es Standard)
		virtual void postSortAction(TPop& pop);

	public:
		/// Constructor
		EvolverStrategyStandard(const Core::Config& c);

		/// Destructor
		virtual ~EvolverStrategyStandard();

		/// Crea una nueva instancia
		static boost::shared_ptr<EvolverStrategy> 
			create(const Core::Config& c);

		/// Realiza un paso evolutivo
		virtual void evolutionaryStep(TPop& pop, EvalModule& evalMod, 
			OpsModule& opsMod);

		/// Obtiene el recolector de estadísticas
		virtual const GP::StatsCollector& getStatsCollector() const;

		/// Reinicializa la estrategia
		virtual void reset();
	};
}

#endif