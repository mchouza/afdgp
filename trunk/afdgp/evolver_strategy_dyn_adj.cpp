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
// evolver_strategy_dyn_adj.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de mayo de 2008
//=============================================================================

#include "config.h"
#include "evolver_strategy.h"
#include "evolver_strategy_factory.h"
#include "evolver_strategy_standard.h"
#include "registrator.h"
#include <boost/lexical_cast.hpp>

namespace GP {

/// Clase de la estrategia derivada
class EvolverStrategyDynAdj : public EvolverStrategy
{
	/// Probabilidad de ejecuci�n para los individuos por encima del l�mite
	double tarpeianExecProb_;

	/// Evolver standard para el manejo general
	EvolverStrategyStandard standardEvSt_;

public:
	/// Constructor
	EvolverStrategyDynAdj(const Core::Config& c) :
	  tarpeianExecProb_(
		  boost::lexical_cast<double>(c.readValue("tarpeianExecProb"))),
	  standardEvSt_(c)
	{
	}

	/// Crea una nueva instancia
	static boost::shared_ptr<EvolverStrategy> create(const Core::Config& c)
	{
		return boost::shared_ptr<EvolverStrategy>(
			new EvolverStrategyDynAdj(c));
	}

	/// Realiza un paso evolutivo
	virtual void evolutionaryStep(TPop& pop, EvalModule& evalMod, 
		OpsModule& opsMod)
	{
		// FIXME: Implementar!!
	}
};

namespace
{
	// Registra la clase
	Util::Registrator<EvolverStrategyFactory, EvolverStrategyDynAdj>
		r("DynamicAdjustment");
}

} // Namespace GP
