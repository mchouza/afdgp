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
// panf_genome_exec_wrapper.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "component_namer.h"
#include "component_value_transl.h"
#include "panf_genome_exec_wrapper.h"
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using std::istream;
using std::istream_iterator;
using std::list;
using std::ostream;
using std::string;

void PANFGenomeExecWrapper::execute(list<WritingHead> &writingHeads,
								const ComponentNamer& embryoCompNamer,
								const ComponentValueTransl& transl) const
{
	// Obtengo los iteradores del genoma
	TGenomeConstIterator itBegin = genome_.begin();
	TGenomeConstIterator itEnd = genome_.end();

	// Creo el objeto encargado de asignar nombres a los componentes
	ComponentNamer compNamer(embryoCompNamer);

	// Ejecuto en forma recursiva sobre cada cabeza
	list<WritingHead>::iterator itWHL = writingHeads.begin();
	list<WritingHead>::iterator itWHLEnd = writingHeads.end();
	for (; itWHL != itWHLEnd;)
		recExecuteCC(itBegin, itEnd, writingHeads, itWHL++, compNamer, transl);
}

void PANFGenomeExecWrapper::recExecuteCC(TGenomeConstIterator& itBegin,
									 TGenomeConstIterator itEnd,
									 list<WritingHead> &writingHeads,
									 list<WritingHead>::iterator itWHL,
									 ComponentNamer& componentNamer,
									 const ComponentValueTransl& transl,
									 bool onlyCheck)
{
	// FIXME: Hacer que si marco onlyCheck no tenga side effects
	
	// Me fijo que no se haya pasado
	if (itBegin == itEnd)
		// FIXME: Lanzar algo más específico
		throw;
	
	// Leo el código
	EOpCode opCode = static_cast<EOpCode>(*itBegin++);

	// Hago el gran switch
	switch (opCode)
	{
	case TWO_LEAD:
	{
		// Quiere insertar un componente
		
		// Obtengo los dos primeros argumentos
		EComponentType compType = recExecuteCT(itBegin, itEnd);
		double value = recExecuteVal(itBegin, itEnd, transl);

		// Construyo el componente y obtengo el iterador para seguir la
		// construcción
		WritingHead::TWHLIt itCC = WritingHead::setComponent(writingHeads,
			itWHL, Component(compType, value,
							componentNamer.getComponentName(compType)));

		// Continúo la construcción
		recExecuteCC(itBegin, itEnd, writingHeads, itCC, componentNamer,
			transl);
	}
	break;
	case SERIES:
	{
		// Quiere hacer una división serie

		// Hago la división y obtengo el par de iteradores
		WritingHead::TWHLItPair itCCPair =
			WritingHead::seriesDiv(writingHeads, itWHL, componentNamer);

		// Continúo la construcción con cada uno de ellos
		recExecuteCC(itBegin, itEnd, writingHeads, itCCPair.get<0>(),
			componentNamer, transl);
		recExecuteCC(itBegin, itEnd, writingHeads, itCCPair.get<1>(),
			componentNamer, transl);
	}
	break;
	case PARALLEL:
	{
		// Quiere hacer una división en paralelo
		
		// FIXME: Considerar que hay más de una posibilidad para la división
		// en paralelo

		// Hago la división y obtengo el quad de iteradores
		WritingHead::TWHLItQuad itCCQuad =
			WritingHead::parallelDiv(writingHeads, itWHL, componentNamer);

		// Continúo la construcción en los cuatro
		recExecuteCC(itBegin, itEnd, writingHeads, itCCQuad.get<0>(),
			componentNamer, transl);
		recExecuteCC(itBegin, itEnd, writingHeads, itCCQuad.get<1>(),
			componentNamer, transl);
		recExecuteCC(itBegin, itEnd, writingHeads, itCCQuad.get<2>(),
			componentNamer, transl);
		recExecuteCC(itBegin, itEnd, writingHeads, itCCQuad.get<3>(),
			componentNamer, transl);
	}
	break;
	case TWO_GROUND:
	{
		// Es igual que una división serie, pero conecta el nodo creado a
		// tierra
		// Hago la división y obtengo el par de iteradores
		WritingHead::TWHLItPair itCCPair =
			WritingHead::twoGround(writingHeads, itWHL, componentNamer);

		// Continúo la construcción con cada uno de ellos
		recExecuteCC(itBegin, itEnd, writingHeads, itCCPair.get<0>(),
			componentNamer, transl);
		recExecuteCC(itBegin, itEnd, writingHeads, itCCPair.get<1>(),
			componentNamer, transl);
	}
	break;
	case FLIP:
	{
		// Da vuelta una arista

		// Hago el trabajo sobre la arista
		WritingHead::TWHLIt itCC = WritingHead::flip(writingHeads, itWHL);

		// Continúo la construcción
		recExecuteCC(itBegin, itEnd, writingHeads, itCC, componentNamer,
			transl);
	}
	break;
	case END:
	{
		// Termino la construcción tal como está
		// FIXME: Borrar la cabeza de escritura
	}
	break;
	case SAFE_CUT:
	{
		// Si puede, corta la arista

		// Llamo a la función que hace este trabajo
		WritingHead::safeCut(writingHeads, itWHL);

		// Termino la ejecución
	}
	break;
	default:
		// Opcode erróneo
		// FIXME: lanzar algo más específico
		throw;
	}
}

double PANFGenomeExecWrapper::recExecuteVal(TGenomeConstIterator &itBegin,
										TGenomeConstIterator itEnd,
										const ComponentValueTransl& transl,
										bool onlyCheck)
{
	// FIXME: Hacer que si marco onlyCheck no tenga side effects
	// FIXME: En caso de que 4 bytes sea muy largo en general, ver de
	// implementar alguna codificación de longitud variable

	// Índice en la tabla de componentes
	unsigned index = 0;

	// Reuno 4 bytes
	for (int i = 0; i < 4; i++)
	{
		// Si me paso es un error
		if (itBegin == itEnd)
			// FIXME: Lanzar algo más específico
			throw;

		// Agrego el byte actual
		index = (index << CHAR_BIT) | *itBegin;

		// Avanzo la posición
		++itBegin;
	}

	// Devuelvo el valor que corresponda
	return transl.getComponentValue(index);
}

EComponentType PANFGenomeExecWrapper::recExecuteCT(TGenomeConstIterator &itBegin,
											   TGenomeConstIterator itEnd,
											   bool onlyCheck)
{
	// FIXME: Hacer que si marco onlyCheck no tenga side effects

	// Me fijo no haberme pasado
	if (itBegin == itEnd)
		// FIXME: Lanzar algo más específico
		throw;

	// Elijo segùn la clase de componente
	switch (*itBegin++)
	{
	case R:
		return RESISTOR;
	case C:
		return CAPACITOR;
	case L:
		return INDUCTOR;
	default:
		// FIXME: Lanzar algo más específico
		// FIXME: Ver si todas estas conversiones no podrína centralizarse un poco
		throw;
	}
}

