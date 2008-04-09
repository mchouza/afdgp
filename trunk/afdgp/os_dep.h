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
// os_dep.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#ifndef OS_DEP_H
#define OS_DEP_H

#include <boost/shared_ptr.hpp>
#include <string>

namespace Util
{
	/// Contiene elementos cuya implemnetación es dependiente del sistema
	namespace OSDep
	{
		/// Interfaz de una biblioteca compartida
		class ISharedLibrary
		{
			// Prohibo copia
			ISharedLibrary(const ISharedLibrary&);
			const ISharedLibrary& operator=(const ISharedLibrary&);

		protected:
			// Prohibo construcción "desde afuera"
			ISharedLibrary()
			{
			}

			virtual ~ISharedLibrary()
			{
			}

		public:
			/// Construye una a partir de un path
			static boost::shared_ptr<ISharedLibrary> 
				makeFromPath(const std::string& path);

			/// Obtiene una dirección
			virtual void* 
				getSymbolAddress(const std::string& symbolName) const = 0;
		};

		/// Devuelve la extensión correspondiente a los módulos
		const std::string& getModulesFileExtension();
	}
}

#endif
