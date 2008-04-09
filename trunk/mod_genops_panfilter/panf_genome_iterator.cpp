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
// panf_genome_iterator.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a NGPD el 6 de abril de 2008
//=============================================================================

#include "panf_genome_iterator.h"

using std::pair;

namespace
{
	/// Devuelve la cantidad de argumentos de un opcode
	size_t getCantArgs(EOpCode opCode)
	{
		switch (opCode)
		{
		case END:
		case SAFE_CUT:
			return 0;
		case FLIP:
			return 1;
		case SERIES:
		case TWO_GROUND:
			return 2;
		case TWO_LEAD:
			return 3;
		case PARALLEL:
			return 4;
		default:
			// FIXME: lanzar algo más específico
			throw;
		}
	}
}

PANFGenomeIterator::PANFGenomeIterator(TGenome& genome, size_t pos) :
	ancestors_(), genome_(genome), kind_(EOK_CC), pos_(0)
{
	// Avanzo hasta llegar o pasarme
	for (; pos_ < pos; ++*this);
}

// FIXME: Depende del tamaño
PANFGenomeIterator& PANFGenomeIterator::operator ++()
{
	// Si estoy en el final, lanza una excepción
	if (pos_ == genome_.size())
		// FIXME: Lanzar algo más específico
		throw;
	
	// De acuerdo a la clase de operador sobre el que esté parado
	switch (kind_)
	{
	case EOK_CC:
		// Si es un terminal
		if (genome_[pos_] == END || genome_[pos_] == SAFE_CUT)
		{
			// Si tiene ancestros
			if (ancestors_.size() > 0)
			{			
				// Si después de decrementar al ancestro tengo 0, significa
				// que terminé con los argumentos de él y por lo tanto él
				// también terminó. Por ello lo saco del vector y le aplico
				// el mismo procedimiento a su ancestro y así hasta que
				// llegue a un ancestro con argumentos por procesar o me
				// quede sin nada en el vector de ancestros
				do
				{
					assert(ancestors_.back().second > 0);
					
					// Decremento la cuenta del ancestro
					--ancestors_.back().second;

					// Si llego a cero lo saco
					if (ancestors_.back().second == 0)
						ancestors_.pop_back();
					// Sino no hay nada más que hacer
					else
						break;

					// Mientras queden ancestros
				} while (ancestors_.size() > 0);
			}
			// Si no tiene ancestros solo incremento...
		}
		// Si no es un terminal
		else
		{
			// Continúo la construcción
			ancestors_.push_back(
				pair<size_t, size_t>(pos_,
					getCantArgs(static_cast<EOpCode>(genome_[pos_]))));
			
			// En caso de tratarse de un TWO_LEAD, debo cambiar el tipo
			if (genome_[pos_] == TWO_LEAD)
				kind_ = EOK_CT;
		}

		// Incremento la posición
		++pos_;
		break;

	case EOK_CT:
		// Estoy en el tipo, sigue el valor
		kind_ = EOK_VAL;

		// El padre debe tener los 3 argumentos y ser un TWO_LEAD
		assert(genome_[ancestors_.back().first] == TWO_LEAD && ancestors_.back().second == 3);

		// Consumo un argumento
		--ancestors_.back().second;

		// Avanzo uno
		++pos_;
		break;

	case EOK_VAL:
		// Estoy en el valor, sigue continuando la construcción
		kind_ = EOK_CC;

		// El padre debe tener los 2 argumentos y ser un TWO_LEAD
		assert(genome_[ancestors_.back().first] == TWO_LEAD && ancestors_.back().second == 2);

		// Consumo un argumento
		--ancestors_.back().second;

		// Avanzo 4
		pos_ += 4;
		break;

	default:
		// No debería llegar acá
		assert(0);
	}

	// Devuelvo la referencia
	return *this;
}

PANFGenomeIterator PANFGenomeIterator::operator ++(int)
{
	PANFGenomeIterator tmp(*this);
	++*this;
	return tmp;
}

OCOrVal PANFGenomeIterator::operator *()
{
	return OCOrVal(TGenomeIterator(&genome_[pos_], &genome_),
		(kind_ == EOK_VAL) ? false : true);
}