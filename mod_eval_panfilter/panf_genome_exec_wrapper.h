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
// panf_genome_exec_wrapper.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef PANF_GENOME_WRAPPER_H
#define PANF_GENOME_WRAPPER_H

#include "writing_head.h"
#include <evalmodule.h>

/// Actua como un "wrapper" del genoma, permitiendo su ejecución sobre el
/// embrión
class PANFGenomeExecWrapper
{
public:
	/// Constructor
	PANFGenomeExecWrapper(const TGenome& genome) :
	  genome_(genome)
	{
	}

	/// Ejecuta el genoma. Aplica los efectos de la ejecución sobre un
	/// conjunto de cabezas de escritura.
	void execute(std::list<WritingHead>& writingHeads,
		const ComponentNamer& embryoCompNamer,
		const ComponentValueTransl& transl) const;

protected:
	/// Ejecuta una parte del genoma encargada de constinuar la construcción
	/// en forma recursiva
	static void recExecuteCC(TGenomeConstIterator& itBegin,
		TGenomeConstIterator itEnd,
		std::list<WritingHead>& writingHeads,
		std::list<WritingHead>::iterator itWHL,
		ComponentNamer& compNamer,
		const ComponentValueTransl& transl,
		bool onlyCheck = false);

	/// Ejecuta una parte del genoma destinada a determinar un tipo de
	/// componente
	static EComponentType recExecuteCT(TGenomeConstIterator& itBegin,
		TGenomeConstIterator itEnd,
		bool onlyCheck = false);

	/// Ejecuta una parte del genoma destinada a determinar un valor del
	/// componente
	static double recExecuteVal(TGenomeConstIterator& itBegin,
		TGenomeConstIterator itEnd,
		const ComponentValueTransl& transl,
		bool onlyCheck = false);

private:
	/// Referencia al genoma original
	const TGenome& genome_;
};

#endif
