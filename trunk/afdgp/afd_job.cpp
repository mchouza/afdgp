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
// afd_job.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de abril de 2008
//=============================================================================

#include "afd_job.h"
#include "config.h"
#include "evolver.h"

using namespace Core;

AFDJob::AFDJob(const Config& baseConfig, 
			   const std::string& filename) :
pConfig_(baseConfig.getView("Job")),
filename_(filename)
{
	pEvolver_.reset(new GP::Evolver(*pConfig_));

	// Carga los datos de la ejecuci�n anterior, si es necesario
	resumeIfPossible();
}

AFDJob::~AFDJob()
{
}

void AFDJob::makeCheckPoint() const
{
	// FIXME: Implementar

	// Serializa los datos del evolver a los archivos asociados
}

void AFDJob::step()
{
	// FIXME: Implementar
}

/// Contin�a la ejecuci�n anterior, si es posible
void AFDJob::resumeIfPossible()
{
	// FIXME: Implementar

	// Se fija si est�n los archivos con los datos

		// Si no est�n, sale

	// Si est�n los carga en el evolver
}