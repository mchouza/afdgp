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
// component_namer.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef COMPONENT_NAMER_H
#define COMPONENT_NAMER_H

#include <panf_genome.h>
#include <map>
#include <sstream>
#include <string>

/// Se ocupa de darle nombre a los componentes
class ComponentNamer
{
public:
	/// Constructor
	// FIXME: Arreglar para que siga la secuencia de los nombres ya
	// existentes en el embrión. (Usar constructor de copia?)
	ComponentNamer()
	{
		charByComp_[RESISTOR] = 'R';
		charByComp_[CAPACITOR] = 'C';
		charByComp_[INDUCTOR] = 'L';
		charByComp_[WIRE] = 'Z';
		charByComp_[VSRC] = 'V';
		charByComp_[PROBE] = 'P';
		nextNum_[RESISTOR] = 1;
		nextNum_[CAPACITOR] = 1;
		nextNum_[INDUCTOR] = 1;
		nextNum_[WIRE] = 1;
		nextNum_[VSRC] = 1;
		nextNum_[PROBE] = 1;
	}

	/// Avanza para evitar colisiones con una netlist que se le pasa como
	/// parámetro
	void advance(std::istream& is)
	{
		// Leo y descarto la primera línea (el título)
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Leo línea a línea
		std::string line;
		while (getline(is, line))
		{			
			// Transformo la línea a mayúsculas
			transform(line.begin(), line.end(), line.begin(), toupper);
			
			// Obtengo el primer caracter
			std::istringstream lineSS(line);
			char compTypeCh;
			lineSS >> compTypeCh;

			// Obtengo el número
			unsigned number;
			lineSS >> number;

			// De acuerdo al tipo de componente, actualizo
			// FIXME: Ver si no se puede centralizar mucho de este trabajo
			EComponentType compType;
			switch (compTypeCh)
			{
			case 'R':
				compType = RESISTOR;
				break;
			case 'C':
				compType = CAPACITOR;
				break;
			case 'L':
				compType = INDUCTOR;
				break;
			case 'Z':
				compType = WIRE;
				break;
			case 'V':
				compType = VSRC;
				break;
			case 'P':
				compType = PROBE;
				break;
			default:
				// FIXME: Lanzar algo más específico
				throw;
			}
			nextNum_[compType] = std::max(nextNum_[compType], number + 1);
		}
	}

	/// Obtiene el próximo nombre
	std::string getComponentName(EComponentType compType)
	{
		// Donde armo la salida
		std::ostringstream oss;
		
		// Devuelvo el que sigue
		oss << charByComp_.find(compType)->second << nextNum_[compType]++;

		// Devuelvo el valor
		return oss.str();
	}	

private:
	/// Mapa que guarda el siguiente número a asignar
	std::map<EComponentType, unsigned> nextNum_;

	/// Mapa que guarda las letras por cada tipo de componente
	std::map<EComponentType, char> charByComp_;
};	

#endif
