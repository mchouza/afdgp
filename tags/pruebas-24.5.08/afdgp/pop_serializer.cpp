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
// pop_serializer.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 5 de mayo de 2008
//=============================================================================

#include "pop_serializer.h"
#include "bin_rw.h"

using namespace Util;

void PopSerializer::serialize(std::ostream& os, const TPop& pop)
{
	// Escribo el tamaño de la población
	writeToBinStream<size_t>(os, pop.size());

	// Para cada elemento de la población...
	for (size_t i = 0; i < pop.size(); i++)
	{
		// Escribo el tamaño en bytes
		writeToBinStream<size_t>(os, pop[i].size());

		// Escribo el individuo
		writeToBinStream(os, &pop[i][0], pop[i].size());
	}
}

void PopSerializer::deserialize(TPop& pop, std::istream& is)
{
	// Leo el tamaño de la población
	size_t popSize = readFromBinStream<size_t>(is);

	// Reservo memoria para la población
	pop.resize(popSize);

	// Leo esa cantidad de individuos
	for (size_t i = 0; i < popSize; i++)
	{
		// Leo el tamaño del individuo
		size_t indivSize = readFromBinStream<size_t>(is);

		// Reservo memoria para el individuo
		pop[i].resize(indivSize);

		// Leo al individuo
		readFromBinStream(is, &pop[i][0], indivSize);
	}
}
