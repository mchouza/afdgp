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
// uniform_random_generator.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 13 de mayo de 2008
//=============================================================================

#ifndef UNIFORM_RANDOM_GENERATOR_H
#define UNIFORM_RANDOM_GENERATOR_H

#include <boost/random.hpp>

namespace Util
{
	/// Generador de n�meros aleatorios distribuidos uniformemente
	class UniformRandomGenerator
	{
		/// Generador interno
		boost::mt19937 intGen_;

	public:
		/// Construye a partir del m�nmo y del m�ximo
		UniformRandomGenerator() :
		intGen_()
		{
		}

		/// Obtiene un  n�mero en el rango [min, max) con una distribuci�n 
		/// uniforme
		template <typename FloatType>
		FloatType getRandomFloat(FloatType min, FloatType max)
		{
			using boost::uniform_real;

			// Obtengo el valor aleatorio
			FloatType ret = uniform_real<FloatType>(min, max)(intGen_);

			// Devuelvo el valor
			return ret;
		}
	};
}

#endif
