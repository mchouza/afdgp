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
// panf_genome_iterator.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a NGPD el 6 de abril de 2008
//=============================================================================

#ifndef PANF_GENOME_ITERATOR_H
#define PANF_GENOME_ITERATOR_H

#include <genome.h>
#include <panf_genome.h>
#include <vector>

/// Clase que representa un opcode o un valor
class OCOrVal
{
public:
	/// Operador de cast a entero
	operator unsigned() const;

	/// Operador de cast a opcode
	operator EOpCode() const;

	/// Indica si se trata de un valor
	bool isVal() const
	{
		return !isOC_;
	}

	/// Indica si se trata de un opcode
	bool isOC() const
	{
		return isOC_;
	}

private:
	/// Constructor
	OCOrVal(TGenomeIterator it, bool isOC) : it_(it), isOC_(isOC)
	{
	}

	/// Iterador sobre el genoma "raw"
	TGenomeIterator it_;

	/// Indica si se trata de un opcode
	bool isOC_;

	/// La clase del iterador puede construirla
	friend class PANFGenomeIterator;
};

/// Clase del iterador que recorre el genoma
class PANFGenomeIterator
{
public:
	/// Constructor a partir de un genoma en posición cero
	PANFGenomeIterator(TGenome& genome) : ancestors_(),
		genome_(genome), pos_(0), kind_(EOK_CC)
	{
	}

	/// Constructor  apartir de un genoma en posición distinta a cero
	PANFGenomeIterator(TGenome& genome, size_t pos);

	/// Incremento prefijo
	PANFGenomeIterator& operator ++();

	/// Incremento posfijo
	PANFGenomeIterator operator ++(int);

	/// Desreferenciación
	OCOrVal operator *();

	/// Devuelve la profundidad
	size_t getDepth() const
	{
		return ancestors_.size();
	}

	/// Devuelve la posición
	size_t getPos() const
	{
		return pos_;
	}

	/// Devuelve la clase de elemento
	EOpKind getKind() const
	{
		return kind_;
	}

private:
	/// Ancestros
	std::vector<std::pair<size_t, size_t> > ancestors_;

	/// Referencia al genoma
	TGenome& genome_;

	/// Posición en el genoma
	size_t pos_;

	/// Tipo del opcode donde está parado
	EOpKind kind_;
};

#endif
