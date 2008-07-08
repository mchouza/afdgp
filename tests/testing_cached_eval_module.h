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
// testing_cached_eval_module.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de julio de 2008
//=============================================================================

#ifndef TESTING_CACHED_EVAL_MODULE_H
#define TESTING_CACHED_EVAL_MODULE_H

#include "common.h"
#include "eval_module.h"

#include <boost/scoped_array.hpp>

class MODULE_API TestingCachedEvalModule : public EvalModule
{
	/// Módulo normal
	boost::shared_ptr<EvalModule> pEvalMod_;

	/// Tipo de la entrada en la tabla de resultados anteriores
	typedef std::pair<uint64_t, double> TCacheEntry;

	/// Tipo de la tabla de resultados anteriores
	typedef boost::scoped_array<TCacheEntry> TCache;

	/// Tabla de resultados anteriores
	mutable TCache cache_;

	/// Tamaño de la tabla de resultados anteriores
	size_t cacheSize_;

	/// Función de hash
	uint64_t (*pHF_)(const std::vector<unsigned char>&);

public:
	/// Construye a partir de un módulo de evaluación normal, 
	/// dándole el tamaño del caché
	TestingCachedEvalModule(boost::shared_ptr<EvalModule> pEvalMod,
		size_t cacheSize, 
		uint64_t (*hf)(const std::vector<unsigned char>&));

	/// Destructor
	virtual ~TestingCachedEvalModule();

	// De las clases base
	virtual double evaluateGenome(const TGenome& genome) const;
	virtual void showIndiv(std::ostream& os, const TGenome& genome) const;
	virtual const std::string& getName() const;
	virtual unsigned int getVersion() const;
	virtual const std::vector<Req>& getReqMods() const;
	virtual bool
		giveConfigData(const std::map<std::string, std::string>& 
			configData);
	virtual bool 
		giveReqMods(const std::vector<boost::shared_ptr<Module> >&
			reqMods);
};

#endif
