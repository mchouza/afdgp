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
// evolver_strategy_factory.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 30 de abril de 2008
//=============================================================================

#include "evolver_strategy_factory.h"

using namespace GP;

boost::shared_ptr<EvolverStrategy> 
EvolverStrategyFactory::make(const std::string& name, 
							 const Core::Config& config)
{
	TProductLine::iterator it = getProductLine().find(name);
	if (it == getProductLine().end())
		return boost::shared_ptr<EvolverStrategy>();
	else
		return it->second(config);
}

void EvolverStrategyFactory::registrate(const std::string& name, 
										boost::shared_ptr<EvolverStrategy>
										(*factoryFunc)(const Core::Config&))
{
	// No me importa si pisa a una ya existente...
	getProductLine()[name] = factoryFunc;
}

EvolverStrategyFactory::TProductLine& EvolverStrategyFactory::getProductLine()
{
	static TProductLine productLine;
	return productLine;
}
