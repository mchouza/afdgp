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
// individual.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "individual.h"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/unordered_set.hpp>
#include <boost/graph/connected_components.hpp>

using boost::connected_components;
using boost::disjoint_sets_with_storage;
using boost::get;
using boost::graph_traits;
using boost::hash_combine;
using boost::lexical_cast;
using boost::property_map;
using boost::tie;
using boost::tokenizer;
using boost::unordered_set;
using boost::vertex_bundle;
using boost::vertex_index_t;
using std::getline;
using std::istream;
using std::istream_iterator;
using std::istringstream;
using std::list;
using std::map;
using std::numeric_limits;
using std::pair;
using std::streamsize;
using std::string;
using std::toupper;
using std::transform;
using std::vector;

/// Namespace anónimo que contiene definiciones y funciones de utilidad
namespace
{
	/// Tipo de los VDs
	typedef graph_traits<TDigraph>::vertex_descriptor TVD;

	/// Tipo de los EDs
	typedef graph_traits<TDigraph>::edge_descriptor TED;

	/// Tipo de los VIs
	typedef graph_traits<TDigraph>::vertex_iterator TVI;

	/// Tipo de los EIs
	typedef graph_traits<TDigraph>::edge_iterator TEI;
	
	/// Tipo del mapa que convierte desde los índices de la NL a los vertex
	/// descriptors
	typedef map<int, TVD> TNLIndex2VDMap;
	
	/// Evalúa el sufijo para determinar el multiplicador
	double evaluateSuffix(const string& suffix)
	{
		switch (suffix[0])
		{
		case 'T':
			return 1e12;
		case 'G':
			return 1e9;
		case 'M':
			if (suffix == "MEG")
				return 1e6;
			else
				return 1e-3;
		case 'K':
			return 1e3;
		case 'U':
			return 1e-6;
		case 'N':
			return 1e-9;
		case 'P':
			return 1e-12;
		default:
			// Cualquier otra cosa se ignora
			return 1.0;
		}
	}

	/// Obtiene los VDs de los nodos indicados. Si algun vértice no existe,
	/// lo crea y devuelve su VD
	pair<TVD, TVD> getVDs(int nlIndex1, int nlIndex2,
		TNLIndex2VDMap& nlIndex2VDMap, TDigraph& digraph)
	{
		// Iterador en el mapa
		TNLIndex2VDMap::iterator it;

		// VDs
		TVD vd1, vd2;
		
		// Busco el primero
		if ((it = nlIndex2VDMap.find(nlIndex1)) == nlIndex2VDMap.end())
		{
			// No está en el grafo
			
			// Lo agrego al digrafo
			vd1 = add_vertex(digraph);

			// Lo marco en el mapa
			nlIndex2VDMap[nlIndex1] = vd1;
		}
		else
			vd1 = it->second;

		// Busco el segundo
		if ((it = nlIndex2VDMap.find(nlIndex2)) == nlIndex2VDMap.end())
		{
			// No está en el grafo
			
			// Lo agrego al digrafo
			vd2 = add_vertex(digraph);

			// Lo marco en el mapa
			nlIndex2VDMap[nlIndex2] = vd2;
		}
		else
			vd2 = it->second;

		// Devuelvo el par de VDs
		return pair<TVD, TVD>(vd1, vd2);
	}

	/// Obtiene el tipo del componente en base al primer caracter del nombre
	EComponentType getComponentType(char ch)
	{
		switch(ch)
		{
		case 'R':
			return RESISTOR;
		case 'L':
			return INDUCTOR;
		case 'C':
			return CAPACITOR;
		case 'Z':
			return WIRE;
		case 'V':
			return VSRC;
		case 'P':
			return PROBE;
		default:
			return INVALID;
		}
	}

	/// Agrega un componente creado en base a 'name' y 'value' entre los
	/// nodos 'firstNode' y 'secondNode' en el grafo 'digraph'
	void addComponent(const string& name, const string& firstNode,
		const string& secondNode, const string& value,
		TNLIndex2VDMap& nlIndex2VDMap, TDigraph& digraph,
		std::list<WritingHead>& whList)
	{
		// Obtengo el primer carácter para determinar el tipo de componente
		char compType = name[0];

		// Convierto los nodos a números
		int firstNodeInt = lexical_cast<int>(firstNode);
		int secondNodeInt = lexical_cast<int>(secondNode);

		// Leo el valor desde el último
		istringstream iss(value);
		double numValue = 0.0;
		iss >> numValue;

		// Obtengo el sufijo
		string suffix;
		iss >> suffix;

		// Modifico el valor del componente de acuerdo al sufijo
		if (suffix.length() > 0)
			numValue *= evaluateSuffix(suffix);

		// Obtengo los VDs, agregando los vértices si es necesario
		TVD vStart, vEnd;
		tie(vStart, vEnd) = getVDs(firstNodeInt, secondNodeInt,
			nlIndex2VDMap, digraph);

		// Agrego la arista
		bool dummy;
		graph_traits<TDigraph>::edge_descriptor ed;
		tie(ed, dummy) = add_edge(vStart, vEnd, digraph);

		// Agrego la propiedad indicando el componente
		digraph[ed] = Component(getComponentType(compType), numValue, name);

		// Si el componente es un cable, creo la correspondiente cabeza de
		// escritura
		if (compType == 'Z')
			whList.push_back(WritingHead(digraph, ed));
	}
}

namespace boost
{
	/// Extiende a boost::hash para que funcione con los EDs
	size_t hash_value(const graph_traits<TDigraph>::edge_descriptor& ed)
	{
		size_t seed = 0;

		// Lo hago con los bytes
		const unsigned char* p = reinterpret_cast<const unsigned char*>(&ed);
		for (int i = 0; i < sizeof(ed); i++)
			hash_combine(seed, *p++);

		// Devuelvo el valor
		return seed;
	}
}

Individual::Individual(istream &is)
{
	// Leo y descarto la primera línea (el título)
	is.ignore(numeric_limits<streamsize>::max(), '\n');

	// Mapa para obtener los VDs en base a lso índices
	TNLIndex2VDMap nlI2VDMap;

	// Línea
	string line;

	// Leo línea a línea
	while (getline(is, line))
	{
		// Transformo la línea a mayúsculas
		transform(line.begin(), line.end(), line.begin(), toupper);
		
		// Creo el tokenizer de la línea
		tokenizer<> tok(line.begin(), line.end());

		// Obtengo los cuatro componentes
		vector<string> tokens;
		for (tokenizer<>::iterator it = tok.begin(); it != tok.end(); ++it)
			tokens.push_back(*it);

		// Si no fueron cuatro, lanzo el error
		if (tokens.size() != 4)
			// FIXME: Lanzar un error más descriptivo
			throw;

		// Creo el componente
		addComponent(tokens[0], tokens[1], tokens[2], tokens[3], nlI2VDMap,
			netlist_, writingHeads_);
	}
}

Individual::Individual(const Individual& indiv) : netlist_(indiv.netlist_)
{
	// Obtengo los iteradores
	list<WritingHead>::const_iterator it, endIt;
	it = indiv.writingHeads_.begin();
	endIt = indiv.writingHeads_.end();

	// Guardo los descriptores en un hash set
	unordered_set<graph_traits<TDigraph>::edge_descriptor> edSet;
	for (; it != endIt; ++it)
		edSet.insert(it->edgeRef_);

	// Obtengo los iteradores de las aristas para el digrafo nuevo y el viejo
	TEI eiOld, eiOldEnd, eiNew, eiNewEnd;
	tie(eiOld, eiOldEnd) = edges(indiv.netlist_);
	tie(eiNew, eiNewEnd) = edges(netlist_);

	// Recorro las aristas del digrafo viejo en paralelo con las del nuevo grafo
	for (; eiOld != eiOldEnd; ++eiOld, ++eiNew)
	{
		// Si ya conozco el descriptor de esta arista
		if (edSet.find(*eiOld) != edSet.end())
		{
			// Agrego una writing head con el descriptor correspondiente
			// del nuevo grafo
			writingHeads_.push_back(WritingHead(netlist_, *eiNew));
		}
	}
}

void Individual::convertToNetlist(std::ostream &os, bool showProbe) const
{
	// Título
	os << "NETLIST\n";

	// Recorro las aristas
	TEI ei, eiEnd;
	tie(ei, eiEnd) = edges(netlist_);
	for (; ei != eiEnd; ++ei)
	{
		// Si es una probe y no quiero que se vea, no la muestro
		if (netlist_[*ei].type_ == PROBE && !showProbe)
			continue;
		
		// Obtengo el fuente
		TVD vdSt = source(*ei, netlist_);

		// Obtengo el destino
		TVD vdEnd = target(*ei, netlist_);

		// Muestro el nombre del componente
		os << netlist_[*ei].name_ << " ";

		// Muestro los números de nodos
		os << netlist_[vdSt] << " " << netlist_[vdEnd] << " ";

		// Si es VSRC, le pongo AC
		if (netlist_[*ei].type_ == VSRC)
			os << " AC ";

		// Muestro el valor
		os << netlist_[*ei].value_ << "\n";
	}
}

void Individual::reenumerateVertices()
{
	// Numero secuencialmente los vértices
	TVI vi, viEnd;
	tie(vi, viEnd) = vertices(netlist_);
	int i = 0;
	for (; vi != viEnd; ++vi)
		netlist_[*vi] = i++;
}

void Individual::spiceCleanUp()
{
	// Par de iteradores sobre vértices
	TVI vi, viEnd;
	// Par de iteradores sobre aristas
	TEI ei, eiEnd;
	
	// Estructura que contiene los conjuntos de nodos conectados por cables
	disjoint_sets_with_storage<> uf(num_vertices(netlist_));

	// Inicializo con los singletons propios de cada nodo
	tie(vi, viEnd) = vertices(netlist_);
	for (; vi != viEnd; ++vi)
		uf.make_set(*vi);

	// Recorro las aristas
	tie(ei, eiEnd) = edges(netlist_);
	for (; ei != eiEnd; ++ei)
	{
		// Si el tipo de la arista es WIRE...
		if (netlist_[*ei].type_ == WIRE)
			// ...los agrego al mismo conjunto
			uf.union_set(source(*ei, netlist_), target(*ei, netlist_));
	}

	// Nuevo grafo
	TDigraph newNL;

	// Mapa para relacionar el viejo grafo con el nuevo
	map<TVD, TVD> oldNL2NewNL;

	// Recorro los padres
	for (size_t i = 0; i < uf.parents().size(); i++)
	{
		// Para cada padre agrego un vértice guardando la asociación en un map
		oldNL2NewNL[uf.parents()[i]] = add_vertex(newNL);
	}

	// Recorro las aristas del grafo original
	tie(ei, eiEnd) = edges(netlist_);
	for (; ei != eiEnd; ++ei)
	{
		// Obtengo los VD correspondientes en el nuevo grafo
		TVD newSrc = oldNL2NewNL[uf.find_set(source(*ei, netlist_))];
		TVD newTgt = oldNL2NewNL[uf.find_set(target(*ei, netlist_))];

		// Si son distintos o si se trata de una PROBE...
		bool dummy;
		TED newEdgeDesc;
		if (newSrc != newTgt || netlist_[*ei].type_ == PROBE)
		{
			// ...la agrego
			tie(newEdgeDesc, dummy) = add_edge(newSrc, newTgt, newNL);

			// Copio las propiedades de la conexión
			newNL[newEdgeDesc] = netlist_[*ei];
		}
	}

	// Intercambio el nuevo con el viejo
	netlist_.swap(newNL);
}

unsigned Individual::getProbedNode() const
{
	// Recorro las aristas
	TEI ei, eiEnd;
	tie(ei, eiEnd) = edges(netlist_);
	for (; ei != eiEnd; ++ei)
	{
		// Si es una arista tipo PROBE...
		if (netlist_[*ei].type_ == PROBE)
		{
			// Devuelvo el atributo del extremo positivo
			return netlist_[source(*ei, netlist_)];
		}
	}

	// Si no la encontró, es un error
	// FIXME: Lanzar algo específico
	throw;
}