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
// evolver.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 17 de abril de 2008
//=============================================================================

#ifndef EVOLVER_H
#define EVOLVER_H

#include "core_fwd.h"
#include "eval_module.h"
#include "evolver_strategy.h"
#include "gp_fwd.h"
#include "ops_module.h"
#include "population.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace GP
{
	/// Clase encargada de manejar el proceso evolutivo
	class Evolver
	{
		/// Poblaci�n
		TPop pop_;

		/// Tama�o de la poblaci�n
		size_t popSize_;

		/// M�dulo para realizar operaciones con la poblaci�n
		boost::shared_ptr<OpsModule> pOpsMod_;

		/// M�dulo para evaluar a los individuos
		boost::shared_ptr<EvalModule> pEvalMod_;

		/// Estrategia a utilizar
		boost::shared_ptr<EvolverStrategy> pEvSt_;

	public:
		/// Construye en base a una configuraci�n base y una espec�fica
		Evolver(const Core::ModuleLibrary& lib, 
			const Core::Config& baseConfig, 
			const Core::Config& specConfig);

		/// Realiza un "paso evolutivo" (puede significar distintas cosas de 
		/// acuerdo a la estrategia adoptada, pero debe ser at�mico)
		void step();

		/// Obtiene el recolector de estad�sticas
		const StatsCollector& getStatsCollector() const;

		/// Nueva run
		void newRun();

		/// Serializa todo
		void serialize(std::ostream& os) const;

		/// Deserializa todo
		void deserialize(std::istream& is);
	};
}

#endif
