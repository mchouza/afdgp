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
// writing_head.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "component_namer.h"
#include "writing_head.h"

using boost::add_edge;
using boost::graph_traits;
using boost::remove_edge;

WritingHead::TWHLIt WritingHead::setComponent(WritingHead::TWHList &whList,
											  WritingHead::TWHLIt whIt,
											  const Component &comp)
{
	// Modifico la propiedad de la arista asociada
	whIt->digraph_[whIt->edgeRef_] = comp;

	// El iterador no cambia, así que lo devuelvo tal cual
	return whIt;
}

WritingHead::TWHLIt WritingHead::flip(WritingHead::TWHList &whList,
									  WritingHead::TWHLIt whIt)
{
	// Obtengo lso descriptores de salida y llegada
	graph_traits<TDigraph>::vertex_descriptor srcVD, tgtVD;
	srcVD = source(whIt->edgeRef_, whIt->digraph_);
	tgtVD = target(whIt->edgeRef_, whIt->digraph_);
	
	// Agrego una nueva arista al revés que la anterior y con sus mismas
	// propiedades
	graph_traits<TDigraph>::edge_descriptor newED;
	bool dummy;
	tie(newED, dummy) = add_edge(tgtVD, srcVD,
		whIt->digraph_[whIt->edgeRef_], whIt->digraph_);

	// Borro la arista original
	remove_edge(whIt->edgeRef_, whIt->digraph_);

	// Cambio la referencia para que apunte a la nueva arista
	whIt->edgeRef_ = newED;

	// El iterador es el mismo, ya que la writing head misma fue modificada
	return whIt;
}

void WritingHead::safeCut(WritingHead::TWHList &whList,
						  WritingHead::TWHLIt whIt)
{
	// Obtengo ambos vértices
	graph_traits<TDigraph>::vertex_descriptor srcVD, tgtVD;
	srcVD = source(whIt->edgeRef_, whIt->digraph_);
	tgtVD = target(whIt->edgeRef_, whIt->digraph_);
	
	// Obtengo el grado total de ambos vértices
	size_t srcDegree = in_degree(srcVD, whIt->digraph_) +
		out_degree(srcVD, whIt->digraph_);
	size_t tgtDegree = in_degree(tgtVD, whIt->digraph_) +
		out_degree(tgtVD, whIt->digraph_);

	// Si el grado de ambos es mayor o igual que 3, es seguro cortar
	if (srcDegree >= 3 && tgtDegree >= 3)
	{
		// Es el caso, borro la arista
		remove_edge(whIt->edgeRef_, whIt->digraph_);
	}

	// De todas formas, se elimina de las cabezas de escritura y no devuelve
	// nada
	whList.erase(whIt);
}

WritingHead::TWHLItPair WritingHead::seriesDiv(WritingHead::TWHList &whList,
											   WritingHead::TWHLIt whIt,
											   ComponentNamer& compNamer)
{
	// Crea un nuevo vértice
	graph_traits<TDigraph>::vertex_descriptor newVD;
	newVD = add_vertex(whIt->digraph_);

	// Obtiene los otros dos vértices
	graph_traits<TDigraph>::vertex_descriptor srcVD, tgtVD;
	srcVD = source(whIt->edgeRef_, whIt->digraph_);
	tgtVD = target(whIt->edgeRef_, whIt->digraph_);

	// Arma las dos nuevas aristas
	graph_traits<TDigraph>::edge_descriptor firstED, secondED;
	bool dummy;
	tie(firstED, dummy) = add_edge(srcVD, newVD, whIt->digraph_);
	tie(secondED, dummy) = add_edge(newVD, tgtVD, whIt->digraph_);

	// Les agrega una nueva propiedad creada con el valor del componente
	// adecuadamente modificado
	Component comp = whIt->digraph_[whIt->edgeRef_];
	Component newComp = comp.seriesDivide(compNamer);
	whIt->digraph_[firstED] = comp;
	whIt->digraph_[secondED] = newComp;

	// Borra la vieja arista
	remove_edge(whIt->edgeRef_, whIt->digraph_);

	// Agrega dos cabezas nuevas a la lista, una para cada arista
	whList.push_back(WritingHead(whIt->digraph_, firstED));
	whList.push_back(WritingHead(whIt->digraph_, secondED));

	// Borra la cabeza de escritura original
	whList.erase(whIt);

	// Devuelve el par con los dos iteradores
	TWHLIt it = whList.end();
	TWHLItPair ret;
	get<1>(ret) = --it;
	get<0>(ret) = --it;
	return ret;
}

WritingHead::TWHLItQuad WritingHead::parallelDiv(WritingHead::TWHList &whList,
												 WritingHead::TWHLIt whIt,
												 ComponentNamer& compNamer)
{
	// Crea dos nuevos vértices
	graph_traits<TDigraph>::vertex_descriptor newSrcVD, newTgtVD;
	newSrcVD = add_vertex(whIt->digraph_);
	newTgtVD = add_vertex(whIt->digraph_);

	// Obtiene los otros dos vértices
	graph_traits<TDigraph>::vertex_descriptor srcVD, tgtVD;
	srcVD = source(whIt->edgeRef_, whIt->digraph_);
	tgtVD = target(whIt->edgeRef_, whIt->digraph_);

	// FIXME: No cambia donde están conectados los otros componentes
	// Eso puede tener alguna consecuencia negativa  en cuanto a la variedad
	// de topologías que puedan ser alcanzadas.

	// Agrega la arista con el otro componente y las dos aristas de
	// interconexión
	graph_traits<TDigraph>::edge_descriptor newCompED, connWithSrcED,
		connWithTgtED;
	bool dummy;
	tie(newCompED, dummy) = add_edge(newSrcVD, newTgtVD, whIt->digraph_);
	tie(connWithSrcED, dummy) = add_edge(srcVD, newSrcVD, whIt->digraph_);
	tie(connWithTgtED, dummy) = add_edge(tgtVD, newTgtVD, whIt->digraph_);

	// Pone el nuevo valor de los componentes
	Component newComp =
		whIt->digraph_[whIt->edgeRef_].parallelDivide(compNamer);
	Component srcWire(WIRE, 0.0, compNamer.getComponentName(WIRE));
	Component tgtWire(WIRE, 0.0, compNamer.getComponentName(WIRE));
	whIt->digraph_[newCompED] = newComp;
	whIt->digraph_[connWithSrcED] = srcWire;
	whIt->digraph_[connWithTgtED] = tgtWire;

	// Agrego tres cabezas nuevas
	whList.push_back(WritingHead(whIt->digraph_, newCompED));
	whList.push_back(WritingHead(whIt->digraph_, connWithSrcED));
	whList.push_back(WritingHead(whIt->digraph_, connWithTgtED));

	// Devuelvo cuatro iteradores
	TWHLItQuad ret;
	TWHLIt it = whList.end();
	get<3>(ret) = --it;
	get<2>(ret) = --it;
	get<1>(ret) = --it;
	get<0>(ret) = whIt;
	return ret;
}

WritingHead::TWHLItPair WritingHead::twoGround(WritingHead::TWHList &whList,
											   WritingHead::TWHLIt whIt,
											   ComponentNamer& compNamer)
{
	// Crea un nuevo vértice
	graph_traits<TDigraph>::vertex_descriptor newVD;
	newVD = add_vertex(whIt->digraph_);

	// Obtiene los otros dos vértices
	graph_traits<TDigraph>::vertex_descriptor srcVD, tgtVD;
	srcVD = source(whIt->edgeRef_, whIt->digraph_);
	tgtVD = target(whIt->edgeRef_, whIt->digraph_);

	// Obtiene el vértice de tierra
	graph_traits<TDigraph>::vertex_descriptor groundVD;
	groundVD = 0;

	// Arma las dos nuevas aristas igual que el serie
	graph_traits<TDigraph>::edge_descriptor firstED, secondED;
	bool dummy;
	tie(firstED, dummy) = add_edge(srcVD, newVD, whIt->digraph_);
	tie(secondED, dummy) = add_edge(newVD, tgtVD, whIt->digraph_);

	// Arma una arista a tierra 
	graph_traits<TDigraph>::edge_descriptor toGroundED;
	tie(toGroundED, dummy) = add_edge(newVD, groundVD, whIt->digraph_);

	// Les agrega una nueva propiedad creada con el valor del componente
	// adecuadamente modificado
	Component comp = whIt->digraph_[whIt->edgeRef_];
	Component newComp = comp.seriesDivide(compNamer);
	whIt->digraph_[firstED] = comp;
	whIt->digraph_[secondED] = newComp;

	// Agrega la propiedad a la arista a tierra
	whIt->digraph_[toGroundED] =
		Component(WIRE, 0.0, compNamer.getComponentName(WIRE));

	// Borra la vieja arista
	remove_edge(whIt->edgeRef_, whIt->digraph_);

	// Agrega dos cabezas nuevas a la lista, una para cada arista
	whList.push_back(WritingHead(whIt->digraph_, firstED));
	whList.push_back(WritingHead(whIt->digraph_, secondED));

	// Borra la cabeza de escritura original
	whList.erase(whIt);

	// Devuelve el par con los dos iteradores
	TWHLIt it = whList.end();
	TWHLItPair ret;
	get<1>(ret) = --it;
	get<0>(ret) = --it;
	return ret;
}