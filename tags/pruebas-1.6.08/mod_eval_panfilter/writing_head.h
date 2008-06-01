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
// writing_head.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef WRITING_HEAD_H
#define WRITING_HEAD_H

#include "component.h"
#include <boost/tuple/tuple.hpp>

/// Clase de las cabezas de escritura. Se encargan de modificar componentes en
/// desarrollo del embrión. Muchas de las operaciones son destructivas, lo que
/// significa que invalidan la cabeza de escritura original.
class WritingHead
{
public:
	// Para simplificar la interfaz
	typedef std::list<WritingHead> TWHList;
	typedef std::list<WritingHead>::iterator TWHLIt;
	typedef std::list<WritingHead>::const_iterator TWHLCIt;
	typedef boost::tuple<TWHLIt, TWHLIt> TWHLItPair;
	typedef boost::tuple<TWHLIt, TWHLIt, TWHLIt> TWHLItTriple;
	typedef boost::tuple<TWHLIt, TWHLIt, TWHLIt, TWHLIt> TWHLItQuad;
	
	/// Constructor
	WritingHead(TDigraph& digraph,
		boost::graph_traits<TDigraph>::edge_descriptor& edge) :
		digraph_(digraph), edgeRef_(edge)
	{
	}

	// FIXME: Considerar las dos posibilidades!
	/// Divide en paralelo. Es destructiva.
	static TWHLItQuad parallelDiv(TWHList& whList, TWHLIt whIt,
		ComponentNamer& compNamer);

	/// Divide en serie. Es destructiva.
	static TWHLItPair seriesDiv(TWHList& whList, TWHLIt whIt,
		ComponentNamer& compNamer);

	/// Conecta a tierra. Es destructiva.
	static TWHLItPair twoGround(TWHList& whList, TWHLIt whItconst,
		ComponentNamer& compNamer);

	/// Pone un componente asociado a la arista. Es destrutiva.
	static TWHLIt setComponent(TWHList& whList, TWHLIt whIt,
		const Component& comp);

	/// Corta la arista en caso de que sea seguro, sino es los mismo que
	/// terminar
	static void safeCut(TWHList& whList, TWHLIt whIt);

	/// Da vuelta una arista
	static TWHLIt flip(TWHList& whList, TWHLIt whIt);

	/// Permite que los individuos puedan copiarlas modificando el digrafo
	friend class Individual;

private:
	/// Referencia al digrafo
	TDigraph& digraph_;

	/// Referencia a la arista
	boost::graph_traits<TDigraph>::edge_descriptor edgeRef_;
};

#endif
