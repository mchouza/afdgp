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
// evolver_strategy_factory.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 30 de abril de 2008
//=============================================================================

#ifndef EVOLVER_STRATEGY_FACTORY_H
#define EVOLVER_STRATEGY_FACTORY_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include "core_fwd.h"
#include "gp_fwd.h"

namespace GP
{
	/// Se ocupa de crear la estrategia que corresponda para manejar el proceso
	/// evolutivo
	class EvolverStrategyFactory
	{
		/// Tipo de la "línea de productos", o sea un mapa que asocia los 
		/// nombres con los punteros a las "factory functions"
		typedef std::map<std::string,
			boost::shared_ptr<EvolverStrategy> (*)(const Core::Config&)>
			TProductLine;

		/// Obtiene la línea de productos
		static TProductLine& getProductLine();

	public:
		/// Crea el que esté registrado con ese nombre o devuelve 0 si no lo 
		/// está
		static boost::shared_ptr<EvolverStrategy> 
			make(const std::string& name, const Core::Config& config);

		/// Registra uno nuevo
		static void registrate(const std::string& name, 
			boost::shared_ptr<EvolverStrategy>
			(*factoryFunc)(const Core::Config&));
	};
}

#endif
