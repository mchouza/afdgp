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
// individual.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "writing_head.h"

/// Clase de los individuos a evaluar
class Individual
{
public:
	/// Constructor sin parámetros
	Individual() {}
	
	/// Constructor de copia
	Individual(const Individual& indiv);

	/// Constructor a partir de una netlist
	Individual(std::istream& is);

	/// Transforma al individuo en una netlist
	void convertToNetlist(std::ostream& os, bool showProbe = true) const;

	/// Renumera los vértices
	void reenumerateVertices();

	/// Limpia al individuo para que el SPICE pueda procesarlo
	void spiceCleanUp();

	/// Devuelve el nodo que tiene una probe
	unsigned getProbedNode() const;

	/// Obtiene las "cabezas de escritura"
	std::list<WritingHead>& getWritingHeads()
	{ return writingHeads_; }

private:
	/// Datos del individuo, básicamente un grafo dirigido que representa
	/// la netlist
	TDigraph netlist_;

	/// "Cabezas de escritura" del individuo. Solo se utilizan durante el
	/// desarrollo.
	std::list<WritingHead> writingHeads_;
};

#endif
