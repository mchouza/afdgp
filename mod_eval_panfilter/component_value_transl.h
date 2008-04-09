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
// component_value_transl.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef COMPONENT_VALUE_TRANSL_H
#define COMPONENT_VALUE_TRANSL_H

/// Clase encargada de traducir un entero representando una posición en una
/// serie de valores de componentes al valor efectivo
class ComponentValueTransl
{
public:
	/// Construye una instancia en base a una tolerancia en partes por mil
	/// dada
	ComponentValueTransl(int tolerance);

	/// Devuelve el valor del componente en base a un índice teniendo
	/// en cuenta la tolerancia previamente establecida
	double getComponentValue(unsigned index) const;

	/// Obtengo el máximo índice para la tolerancia establecida
	int getMaxIndex() const;

private:
	/// Tolerancia
	int tolerance_;

	/// Puntero indicando la tabla utilizada
	int* table_;

	/// Cantidad de valores por década que tiene la tabla utilizada
	int valuesPerDec_;

	/// Cantidad de décadas soportada
	int cantDec_;
};

#endif
