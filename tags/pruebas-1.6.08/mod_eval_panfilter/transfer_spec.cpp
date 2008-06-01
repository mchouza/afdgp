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
// transfer_spec.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "transfer_spec.h"
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>

using std::istringstream;
using std::lower_bound;
using std::pair;
using std::sort;
using std::string;
using std::vector;

TransferSpec::TransferSpec(const string& sTS)
{
	// FIXME: Asume que no hay errores

	// Leo de a pares
	istringstream issTS(sTS);
	do
	{
		// El par frecuencia tensión
		pair<double, double> freqVoltagePair;

		// Leo la frecuencia
		issTS >> freqVoltagePair.first;

		// Leo la tensión
		issTS >> freqVoltagePair.second;

		// Guardo el par
		freqVoltage_.push_back(freqVoltagePair);

	} while (!issTS.eof());

	// Ordeno los pares por la frecuencia en orden creciente
	sort(freqVoltage_.begin(), freqVoltage_.end());
}

double TransferSpec::operator ()(double freq) const
{
	// Busco donde podría insertar esta frecuencia
	pair<double, double> tmp(freq, 0);
	vector<pair<double, double> >::const_iterator it =
		lower_bound(freqVoltage_.begin(), freqVoltage_.end(), tmp);

	// Si es al final o al principio, no tengo datos para interpolar
	if (it == freqVoltage_.begin() || it == freqVoltage_.end())
		// FIXME: Lanzar algo más específico
		throw;

	// Caso contrario, interpolo
	return 	(freq - (it-1)->first) /
			(it->first - (it-1)->first) * (it-1)->second +
			(it->first - freq) /
			(it->first - (it-1)->first) * it->second;
}