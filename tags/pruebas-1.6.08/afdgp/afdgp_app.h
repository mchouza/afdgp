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
// afdgp_app.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de abril de 2008
//=============================================================================

#ifndef AFDGP_APP_H
#define AFDGP_APP_H

#include "core_fwd.h"
#include <boost/scoped_ptr.hpp>

namespace Core
{
	/// Representa a la aplicación encargada de realizar el diseño de un filtro 
	/// analógico utilizando programación genética. Su ejecución puede ser 
	/// interrumpida llamando a exit().
	class AFDGPApp
	{
		/// Clase de la implementación
		class Impl;

		/// Puntero a la implementación
		boost::scoped_ptr<Impl> pImpl_;

		// Impido la copia
		AFDGPApp(const AFDGPApp&);

	public:
		/// Construye la aplicación a partir de la línea de comandos
		AFDGPApp(int argc, char* argv[]);

		/// Destructor
		virtual ~AFDGPApp();

		/// Comienza la ejecución de la aplicación
		void run();

		/// Termina la ejecución de la aplicación
		void exit();
	};
}

#endif
