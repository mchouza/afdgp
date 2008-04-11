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
// module_library.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#include "module_library.h"
#include "os_dep.h"
#include <boost/filesystem.hpp>

using namespace Core;

ModuleLibrary::ModuleLibrary(const std::string& pathStr)
{
	using boost::filesystem::directory_iterator;
	using boost::filesystem::exists;
	using boost::filesystem::extension;
	using boost::filesystem::path;
	using Util::OSDep::getModulesFileExtension;

	path dirPath(pathStr);

	// Si el path no existe, es un error
	if (!exists(dirPath))
		throw; // FIXME: Lanzar algo más específico

	// Recorro el directorio
	directory_iterator it(dirPath);
	directory_iterator itEnd;
	for (; it != itEnd; ++it)
	{
		// Si es un archivo...
		if (extension(it->path()) == getModulesFileExtension())
			// ...trato de cargarlo
			tryToLoad(it->path().string());
	}
}

void ModuleLibrary::dump(std::ostream& out) const
{
	out << "Módulos cargados: " << (modules_.size() ? "\n" : "ninguno\n");
	
	TModuleContainer::const_iterator it = modules_.begin();
	for (; it != modules_.end(); ++it)
	{
		out << "\t" << it->first << " - Versión: ";
		out << it->second->getVersion() << "\n";
	}
}

void ModuleLibrary::tryToLoad(const std::string& modulePath)
{
	// FIXME: Implementar

	using boost::shared_ptr;
	using std::string;
	using Util::OSDep::ISharedLibrary;

	// Cargo la biblioteca
	shared_ptr<ISharedLibrary> pLib = ISharedLibrary::makeFromPath(modulePath);

	// Me fijo si tiene una función 'getModule()'
	Module* (*pGM)(unsigned) =
		static_cast<Module*(*)(unsigned)>(pLib->getSymbolAddress("getModule"));
	if (!pGM)
		// Si no la tiene, salgo SIN ERROR
		return;

	// Si la tiene, obtengo el módulo con su nombre y versión
	shared_ptr<Module> pMod(pGM(MAGIC_NUMBER));
	if (!pMod)
		return;
	string modName = pMod->getName();
	unsigned modVer = pMod->getVersion();

	// Me fijo si tengo ese módulo
	TModuleContainer::const_iterator it = modules_.find(modName);
	if (it != modules_.end())
	{
		// Si lo tengo y el nuevo tiene versión menor o igual al actual...
		if (it->second->getVersion() > modVer)
			// ...salgo SIN ERROR
			return;
	}

	// Si llegué acá, no tengo el módulo o este módulo es más nuevo que el que
	// tengo.

	// Cargo este en ese lugar
	shLibs_[modName] = pLib;
	modules_[modName] = pMod;
}
