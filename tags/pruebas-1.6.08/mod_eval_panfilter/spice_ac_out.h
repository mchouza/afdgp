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
// spice_ac_out.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef SPICE_AC_OUT_H
#define SPICE_AC_OUT_H

#include <complex>
#include <iosfwd>
#include <set>
#include <vector>

/// Clase encargada de mostrarnos la salida de un análisis AC hecho con
/// SPICE
class SpiceACOut
{
public:
	/// Construye la salida en base a la salida textual
	SpiceACOut(const std::vector<char>& spiceRawOutput);

	/// Muestra la salida en forma de texto
	friend std::ostream& operator<<(std::ostream& os, const SpiceACOut& out);

	/// Obtiene el vector de valores de voltaje en un nodo dado
	void getNodeVoltages(const std::string& node,
		std::vector<std::complex<double> >& voltages) const;

	/// Obtiene el vector de frecuencias
	const std::vector<double>& getFreqs() const;

protected:
	/// Rellena los vectores de la clase
	void fillVectors(const std::vector<char>& spiceRawOutput);

private:
	/// Vector de vectores. Las filas representan cada nodo y las columnas
	/// cada frecuencia
	std::vector<std::vector<std::complex<double> > > data_;

	/// Vector indicando las frecuencias
	std::vector<double> freqs_;

	/// Vector indicando los nodos (siempre uso números, no strings)
	std::vector<unsigned> nodes_;
};

#endif
