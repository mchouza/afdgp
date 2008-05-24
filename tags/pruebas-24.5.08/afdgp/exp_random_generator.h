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
// exp_random_generator.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 13 de mayo de 2008
//=============================================================================

#ifndef EXP_RANDOM_GENERATOR_H
#define EXP_RANDOM_GENERATOR_H

#include <boost/random.hpp>

namespace Util
{
	/// Generador de números aleatorios distribuidos exponencialmente
	class ExpRandomGenerator
	{	
		/// Generador interno
		boost::mt19937 intGen_;

	public:
		/// Constructor
		ExpRandomGenerator() :
		intGen_()
		{
		}

		/// Obtiene un entero en el rango [0, max) de acuerdo a una 
		/// distribución exponencial de lambda dado
		template <typename IntType, typename RealType>
		IntType getRandomInteger(IntType max, RealType lambda)
		{	
			using boost::exponential_distribution;

			// Obtengo el valor aleatorio
			IntType ret = static_cast<IntType>(
				exponential_distribution<RealType>(lambda)(intGen_));

			// Si se pasa, lo reduzco al rango
			if (ret >= max)
				ret %= max;

			// Devuelvo el valor
			return ret;
		}
	};
}

#endif
