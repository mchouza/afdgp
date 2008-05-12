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
// cached_eval_module.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 25 de abril de 2008
//=============================================================================

#include "cached_eval_module.h"
#include <algorithm>

using namespace GP;

namespace
{
	/// Hash inválido
	const boost::uint64_t INVALID_HASH = ~0;
}

CachedEvalModule::CachedEvalModule(boost::shared_ptr<EvalModule> pEvalMod, 
								   size_t cacheSize) :
pEvalMod_(pEvalMod),
cache_(new TCacheEntry[cacheSize]),
cacheSize_(cacheSize)
{
	if (cacheSize_ < 1024)
		throw; // FIXME: Lanzar algo más específico

	std::fill(cache_.get(), cache_.get() + cacheSize_, 
		TCacheEntry(INVALID_HASH, 0.0));
}

CachedEvalModule::~CachedEvalModule()
{
}

double CachedEvalModule::evaluateGenome(const TGenome& genome) const
{
	boost::uint64_t hashedGenome = Util::mmHash2(&genome[0], genome.size());

	// Si el hash colisiona con el inválido, lo evalúo siempre
	if (hashedGenome == INVALID_HASH)
		return pEvalMod_->evaluateGenome(genome);

	TCacheEntry& cacheEntry = 
		cache_[static_cast<ptrdiff_t>(hashedGenome % cacheSize_)];

	if (cacheEntry.first != hashedGenome)
	{
		cacheEntry.first = hashedGenome;
		cacheEntry.second = pEvalMod_->evaluateGenome(genome);
	}

	return cacheEntry.second;
}

void CachedEvalModule::showIndiv(std::ostream& os, const TGenome& genome) const
{
	return pEvalMod_->showIndiv(os, genome);
}

const std::vector<Module::Req>& CachedEvalModule::getReqMods() const
{
	return pEvalMod_->getReqMods();
}

const std::string& CachedEvalModule::getName() const
{
	return pEvalMod_->getName();
}

unsigned int CachedEvalModule::getVersion() const
{
	return pEvalMod_->getVersion();
}

bool CachedEvalModule::giveConfigData(const std::map<std::string,std::string>& 
									  configData)
{
	return pEvalMod_->giveConfigData(configData);
}

bool 
CachedEvalModule::giveReqMods(const std::vector<boost::shared_ptr<Module> >&
							  reqMods)
{
	return pEvalMod_->giveReqMods(reqMods);
}
