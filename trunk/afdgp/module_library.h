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
// module_library.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#ifndef MODULE_LIBRARY_H
#define MODULE_LIBRARY_H

#include "module.h"
#include "util_fwd.h"
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

namespace Core
{
	/// Maneja el conjunto de módulos asignado a la aplicación
	class ModuleLibrary
	{
		/// Tipo del contenedor de bibliotecas compartidas 
		typedef 
			std::map<	std::string,
						boost::shared_ptr<Util::OSDep::ISharedLibrary> >
			TShLibContainer;
		
		/// Contenedor de bibliotecas compartidas indexado por nombre de módulo
		TShLibContainer shLibs_;
		
		/// Tipo del contenedor de módulos
		typedef std::map<std::string, boost::shared_ptr<Module> >
			TModuleContainer;
		
		/// Contenedor de módulos indexados por nombre
		TModuleContainer modules_;

		/// Arregla las dependencias
		void fixDependencies();

		/// Intenta cargar un módulo
		void tryToLoad(const std::string& modulePath);

	public:
		/// Construye a partir de una serie de paths de directorio
		ModuleLibrary(const std::string& paths);

		/// Obtiene un módulo según el nombre
		boost::shared_ptr<Module> 
			getModuleByName(const std::string& name) const;

		/// Muestra los módulos cargados con sus correspondientes versiones
		void dump(std::ostream& out) const;
	};
}

#endif
