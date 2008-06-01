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
// spice_ac_out.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "profiling.h"
#include "spice_ac_out.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using std::complex;
using std::endl;
using std::map;
using std::ostream;
using std::search;
using std::string;
using std::vector;

/// Namespace anónimo para funciones varias
namespace
{
	/// Constante de cadena para hallar el comienzo de la descripción de las
	/// variables
	const string varStr = "Variables:\n";

	/// Constante de cadena para hallar el comienzo de los datos
	const string binStr = "Binary:\n";

	/// Lee la descripción de una variable de voltaje para conectar los
	/// números de línea con los correspondientes números de nodo
	void readVoltageVar(vector<char>::const_iterator& it, unsigned lineNum,
		map<unsigned,unsigned>& line2NodeMap)
	{
		// Salto el TAB, el número y el TAB
		assert(*it == '\t');
		++it;
		while (std::isdigit(*it))
			++it;
		assert(*it == '\t');
		++it;

		// Salto la 'V' y el '('
		if (*it++ != 'V' || *it++ != '(')
		{
			// No es un voltaje de nodo: lo salteo
			while (*it++ != '\n');
			return;
		}

		// Leo el número y salto el ')'
		int nodeNum;
		char* pEnd;
		nodeNum = std::strtol(&*it, &pEnd, 10);
		it += pEnd - &*it;
		++it;

		// Salto el TAB
		assert(*it == '\t');
		++it;

		// Salto la palabra
		while (std::islower(*it))
			++it;

		// Salto el '\n'
		assert(*it == '\n');
		++it;

		// Guardo en el mapa
		line2NodeMap[lineNum] = nodeNum;
	}

	void readFreqBlock(vector<char>::const_iterator itData,
		double& freq,
		vector<complex<double> >& data,
		const map<unsigned, unsigned>& line2NodeMap)
	{
		// Convierto el iterador en un puntero a double
		const double* pData = reinterpret_cast<const double*>(&*itData);

		// Leo la frecuencia
		freq = *pData;

		// Recorro el mapa
		map<unsigned,unsigned>::const_iterator it, itEnd;
		it = line2NodeMap.begin();
		itEnd = line2NodeMap.end();
		for (int i = 0; it != itEnd; ++it)
			data[i++] = complex<double>(pData[it->first * 2],
				pData[it->first * 2 + 1]);
	}
}

SpiceACOut::SpiceACOut(const vector<char>& spRO)
{
	MyProfiler p("SpiceACOut::SpiceACOut");
	
	// Relleno los vectores
	fillVectors(spRO);
}

ostream& operator<<(ostream& os, const SpiceACOut& out)
{	
	// Muestro los nodos
	os << "Nodos:" << endl;
	for (size_t i = 0; i < out.nodes_.size(); i++)
		os << out.nodes_[i] << endl;
	
	// Muestro los datos
	for (size_t i = 0; i < out.freqs_.size(); i++)
	{
		os << "Frecuencia: " << out.freqs_[i] << endl;

		for (size_t j = 0; j < out.nodes_.size(); j++)
			os << "V(" << out.nodes_[j] << ") = " << out.data_[i][j] << endl;
	}

	// Devuelvo la stream
	return os;
}


const vector<double>& SpiceACOut::getFreqs() const
{
	return freqs_;
}


void SpiceACOut::getNodeVoltages(const string &node,
								 vector<complex<double> >& voltages) const
{
	// Obtengo el número que representa al nodo
	unsigned intNode = lexical_cast<unsigned>(node);

	// Busco el nodo
	vector<unsigned>::const_iterator it =
		std::find(nodes_.begin(), nodes_.end(), intNode);
	if (it == nodes_.end())
		// FIXME: Lanzar algo más específico
		throw;

	// Guardo los valores que correspondan a ese nodo

	// Recorro el vector que nos da los valores por frecuencia
	for (size_t i = 0; i < data_.size(); i++)
	{
		// Almaceno el valor correspondiente
		voltages.push_back(data_[i][it - nodes_.begin()]);
	}
}

void SpiceACOut::fillVectors(const vector<char>& spRO)
{
	// Si no hay entrada, salgo
	if (spRO.empty())
		// FIXME: lanzar algo más específico
		throw 1;
	
	// Busco la secuencia "Variables:\n"
	vector<char>::const_iterator itVar = 
		search(spRO.begin(), spRO.end(), varStr.begin(), varStr.end());

	// Si no encuentro la secuencia buscada, salgo
	if (itVar == spRO.end())
		throw 1;

	// Lo apunto al final de la secuencia
	itVar += varStr.size();

	// Busco la secuencia "Binary:\n"
	vector<char>::const_iterator itData =
		search(spRO.begin(), spRO.end(), binStr.begin(), binStr.end());
	
	// Si no encuentro la secuencia buscada, salgo
	if (itData == spRO.end())
		throw 1;
	
	// No lo incremento, primero leo las variables

	// Salto esta línea, ya que se que tiene solo la frecuencia
	while (*itVar++ != '\n');

	// Mapa que va desde el número de la línea hasta el valor del nodo
	map<unsigned, unsigned> line2NodeMap;

	// Mientras no me pase, leo variables de voltaje
	int line = 1;
	while (itVar < itData)
		readVoltageVar(itVar, line++, line2NodeMap);
	assert(itVar == itData);

	// Guardo los nodos
	nodes_.resize(line2NodeMap.size());
	map<unsigned,unsigned>::const_iterator itMap, itMapEnd;
	itMap = line2NodeMap.begin();
	itMapEnd = line2NodeMap.end();
	for (int i = 0; itMap != itMapEnd; ++itMap, ++i)
		nodes_[i] = itMap->second;

	// Posiciono el iterador de los datos donde corresponde
	itData += binStr.size();

	// Compruebo que se pueda leer el resto
	assert((spRO.end() - itData) % (line * 2 * sizeof(double)) == 0);

	// Obtengo la cantidad de puntos
	unsigned pointNum = static_cast<unsigned>(spRO.end() - itData) /
		(line * 2 * sizeof(double));

	// Reservo con los tamaños apropiados
	freqs_.resize(pointNum);
	data_.resize(pointNum);
	for (unsigned i = 0; i < pointNum; i++)
		data_[i].resize(nodes_.size());

	// Para esa cantidad, leo bloques
	for (unsigned i = 0; i < pointNum; i++)
	{
		readFreqBlock(itData, freqs_[i], data_[i], line2NodeMap);
		itData += line * 2 * sizeof(double);
	}
}
