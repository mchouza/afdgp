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
// ops_module.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef OPS_MODULE_H
#define OPS_MODULE_H

#include "module.h"
#include <genome.h>

/// Base abstracta para los módulos encargados de generar, mutar y cruzar los
/// genomas
class MODULE_API OpsModule : public Module
{
public:
	/// Inicializa un genoma en forma aleatoria
	virtual void randomInit(TGenome& genome) const = 0;

	/// Muta un genoma
	virtual void mutate(TGenome& genome) const = 0;

	/// Cruza un genoma con otro
	virtual void cross(TGenome& genome1, TGenome& genome2) const = 0;

	/// Realiza un operación de alteración
	virtual void altOp(TGenome& genome) const = 0;

	/// Guarda un genoma en una stream
	virtual void save(std::ostream& os, TGenome& genome,
		bool textualFormat = true) const = 0;

	/// Carga un genoma a partir de una stream
	virtual void load(std::istream& is, TGenome& genome,
		bool textualFormat = true) const = 0;
};

#endif