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

using namespace Util;

void PopSerializer::serialize(std::ostream& os, const TPop& pop)
{
	// Escribo el tama�o de la poblaci�n
	size_t popSize = pop.size();
	os.write(reinterpret_cast<const char*>(&popSize), sizeof(popSize));

	// Para cada elemento de la poblaci�n...
	for (size_t i = 0; i < pop.size(); i++)
	{
		// Escribo el tama�o en bytes
		size_t indivSize = pop[i].size();
		os.write(reinterpret_cast<const char*>(&indivSize),
			sizeof(indivSize));

		// Escribo el individuo
		os.write(reinterpret_cast<const char*>(&pop[i][0]), 
			static_cast<std::streamsize>(pop[i].size()));
	}
}

void PopSerializer::deserialize(TPop& pop, std::istream& is)
{
	// Leo el tama�o de la poblaci�n
	size_t popSize;
	is.read(reinterpret_cast<char*>(&popSize), sizeof(popSize));

	// Reservo memoria para la poblaci�n
	pop.resize(popSize);

	// Leo esa cantidad de individuos
	for (size_t i = 0; i < popSize; i++)
	{
		// Leo el tama�o del individuo
		size_t indivSize;
		is.read(reinterpret_cast<char*>(&indivSize), sizeof(indivSize));

		// Reservo memoria para el individuo
		pop[i].resize(indivSize);

		// Leo al individuo
		is.read(reinterpret_cast<char*>(&pop[i][0]), 
			static_cast<std::streamsize>(indivSize));
	}
}
