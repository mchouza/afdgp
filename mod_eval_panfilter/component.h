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
// component.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef COMPONENT_H
#define COMPONENT_H

#include "component_namer.h"
#include <panf_genome.h>

// FIXME: Tener en cuenta la tabla de valores al dividir o multiplicar...
/// Struct que representa un componente
struct Component
{
	/// Constructor sin parámetros
	Component() : type_(INVALID), value_(0.0), name_()
	{
	}

	/// Constructor con parámetros
	Component(EComponentType type, double value, const std::string& name,
		bool adaptValue = true) :
		type_(type), value_(value), name_(name)
	{
		// El valor de los capacitores y de lso inductores se expresa en pico
		if (adaptValue && (type_ == CAPACITOR || type_ == INDUCTOR))
			value_ *= 1e-12;
	}
	
	/// Modifica el valor de este componente y retorna otro de modo de que la
	/// combinación serie entre ambos nos de el mismo valor que el del 
	/// componente original
	Component seriesDivide(ComponentNamer& namer)
	{
		// Modifico el valor
		switch (type_)
		{
		case RESISTOR:
		case INDUCTOR:
			value_ /= 2.0;
			break;
		case CAPACITOR:
			value_ *= 2.0;
			break;
		case WIRE:
		case PROBE:
			// El valor no tiene importancia...
			break;
		default:
			// Atrapo casos perdidos
			// FIXME: Hacer más específico
			throw;
		}

		// Devuelvo uno nuevo
		return Component(type_, value_, namer.getComponentName(type_), false);
	}

	/// Modifica el valor de este componente y retorna otro de modo de que la
	/// combinación paralelo entre ambos nos de el mismo valor que el del 
	/// componente original
	Component parallelDivide(ComponentNamer& namer)
	{
		// Modifico el valor
		switch (type_)
		{
		case RESISTOR:
		case INDUCTOR:
			value_ *= 2.0;
			break;
		case CAPACITOR:
			value_ /= 2.0;
			break;
		case WIRE:
		case PROBE:
			// El valor no tiene importancia...
			break;
		default:
			// Atrapo casos perdidos
			// FIXME: Hacer más específico
			throw;
		}

		// Devuelvo uno nuevo
		return Component(type_, value_, namer.getComponentName(type_), false);
	}

	/// Tipo del componente
	EComponentType type_;

	/// Valor del componente
	double value_;

	/// Nombre del componente
	std::string name_;
};

#endif
