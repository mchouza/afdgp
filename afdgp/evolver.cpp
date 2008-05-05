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
// evolver.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 17 de abril de 2008
//=============================================================================

#include "evolver.h"
#include "cached_eval_module.h"
#include "config.h"
#include "module_library.h"
#include "pop_serializer.h"
#include <boost/lexical_cast.hpp>

using namespace GP;

Evolver::Evolver(const Core::ModuleLibrary& lib, 
				 const Core::Config& baseConfig, 
				 const Core::Config& specConfig)
{
	using boost::lexical_cast;
	using boost::shared_dynamic_cast;
	using boost::shared_ptr;
	using Core::Config;
	using std::string;

	// Obtengo la configuración a utilizar
	shared_ptr<Config> pCC = specConfig.getCombinedConfig(baseConfig);

	// Cargo los módulos de evaluación y operaciones que correspondan
	std::string evalModName = pCC->readValue("EvalModule");
	std::string opsModName = pCC->readValue("OpsModule");
	pEvalMod_ =
		shared_dynamic_cast<EvalModule>(lib.getModuleByName(evalModName));
	pOpsMod_ =
		shared_dynamic_cast<OpsModule>(lib.getModuleByName(opsModName));

	// Si me indican usar el que incorpora caché, lo hago
	if (lexical_cast<bool>(pCC->readValue("UseCachedEvalModule", "false")))
	{
		size_t cacheSize =
			lexical_cast<size_t>(pCC->readValue("CachedEvalModule.CacheSize"));
		shared_ptr<EvalModule> pBaseModule = pEvalMod_;
		pEvalMod_.reset(new CachedEvalModule(pBaseModule, cacheSize));
	}

	// Le doy los datos de configuración a cada módulo
	bool retEM, retOM;
	retEM = pEvalMod_->giveConfigData(
		pCC->getView("EvalModule")->getKeyValuePairs());
	retOM = pOpsMod_->giveConfigData(
		pCC->getView("OpsModule")->getKeyValuePairs());
	if (!(retOM && retEM))
		throw; // FIXME: Lanzar algo más específico

	// Inicializo la población
	size_t popSize = lexical_cast<size_t>(pCC->readValue("PopulationSize"));
	pop_.resize(popSize);
	for (size_t i = 0; i < pop_.size(); i++)
		pOpsMod_->randomInit(pop_[i]);

	// FIXME: Inicializar la estrategia evolutiva
}

void Evolver::step()
{
	pEvSt_->evolutionaryStep(pop_, *pEvalMod_, *pOpsMod_);
}

void Evolver::serialize(std::ostream &os) const
{
	// FIXME: Solo serializa a la población
	Util::PopSerializer::serialize(os, pop_);
}

void Evolver::deserialize(std::istream &is)
{
	// FIXME: Solo deserializa a la población
	Util::PopSerializer::deserialize(pop_, is);
}