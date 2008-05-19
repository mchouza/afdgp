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
// component_value_transl.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "component_value_transl.h"
#include <cmath>

using std::pow;

/// Namespace anónimo para ocultar las tablas
namespace
{
	// Todas las tablas están basadas en las del estándar IEC 606063.
	// Ver más información en http://en.wikipedia.org/wiki/Preferred_number.

	/// Tabla de valores de la serie de 5 % (E24)
	int fivePercentTable[] =
	{
		100, 110, 120, 130, 150, 160, 180, 200, 220, 240, 270, 300,
        330, 360, 390, 430, 470, 510, 560, 620, 680, 750, 820, 910
	};
}

ComponentValueTransl::ComponentValueTransl(int tolerance)
{
	// Se fija si es uno de los valores permitidos
	switch (tolerance)
	{
	case 50:
		// La tabla es la del 5 %
		table_ = fivePercentTable;
		valuesPerDec_ = sizeof(fivePercentTable) / sizeof(int);
		break;
	default:
		// No es una tolerancia válida
		// FIXME: lanzar algo apropiado
		throw;
	}

	// En todos los casos válidos guardo el valor de la tolerancia
	tolerance_ = tolerance;

	// En todos los casos tengo 12 décadas
	// R: 0.1 - 1 - 10 - 100 - 1k - 10k - 100k - 1M - 100M - 1G - 10G - 100G
	// C/L: 0.1p - 1p - 10p - 100p - 1n - 10n - 100n - 1u - 10u - 100u - 1m -
	// 10m - 100m
	cantDec_ = 12;
}

double ComponentValueTransl::getComponentValue(unsigned index) const
{
	// Primero escalo el número al rango apropiado
	index %= (cantDec_ * valuesPerDec_);

	// Obtengo la década y el offset adentro de ella
	int dec = index / valuesPerDec_;
	int offset = index % valuesPerDec_;

	// Obtengo el multiplicador representando a las décadas
	double mult = pow(10.0, dec);

	// Obtengo el valor correspondiente de la tabla
	double offsetValue = table_[offset] / 1000.0;

	// Devuelvo el resultado (asume que es una resistencia,
	// sino debe escalarse)
	return offsetValue * mult;
}

int ComponentValueTransl::getMaxIndex() const
{
	// FIXME: Hacer que funcione
	return -1;
}

