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
// cached_eval_module.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 25 de abril de 2008
//=============================================================================

#ifndef CACHED_EVAL_MODULE_H
#define CACHED_EVAL_MODULE_H

#include "eval_module.h"
#include "mm2_wrapper.h"
#include "plat_dep.h"
#include <boost/scoped_array.hpp>

/// Módulo de evaluación con caché. Encapsula un módulo de evaluación pero 
/// realiza la función de caché transparente
class MODULE_API CachedEvalModule : public EvalModule
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

public:
	/// Construye a partir de un módulo de evaluación normal, 
	/// dándole el tamaño del caché
	CachedEvalModule(boost::shared_ptr<EvalModule> pEvalMod,
		size_t cacheSize);

	/// Destructor
	virtual ~CachedEvalModule();

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
