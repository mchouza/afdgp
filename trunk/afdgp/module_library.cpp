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
#include <map>
#include <set>

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

	// Completo las dependencias
	fixDependencies();
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

boost::shared_ptr<Module> 
ModuleLibrary::getModuleByName(const std::string& name) const
{
	// Busco un módulo con ese nombre
	TModuleContainer::const_iterator it = modules_.find(name);

	// Lo encontré?
	if (it == modules_.end())
		// No, devuelvo un null pointer
		return boost::shared_ptr<Module>();
	else
		// Si, devuelvo el puntero
		return it->second;
}


void ModuleLibrary::fixDependencies()
{
	using boost::shared_ptr;
	typedef Module::Req TReq;
	using std::map;
	using std::vector;
	typedef TModuleContainer::iterator TMCIt;

	// Para cada módulo
	TMCIt it(modules_.begin()), itEnd(modules_.end());
	for (; it != itEnd; ++it)
	{
		// Si ya encontré que el módulo tenía dependencias imposibles de 
		// satisfacer
		if (!it->second)
			// Sigo con otro
			continue;
		
		// Obtengo sus dependencias
		const vector<TReq>& reqs = it->second->getReqMods();

		// Para cada dependencia
		for (size_t j = 0; j < reqs.size(); j++)
		{
			// No está satisfecha?
			if (modules_.find(reqs[j].first) == modules_.end() ||
				modules_[reqs[j].first]->getVersion() < reqs[j].second)
			{
				// Marco como imposible de utilizar al módulo
				it->second = shared_ptr<Module>();

				// Salgo del for
				break;
			}
		}

		// Encontré dependencias no satisfechas con el módulo?
		if (!it->second)
			// Empiezo desde el principio
			it = modules_.begin(); // ATENCIÓN!! Modifico el iterador del loop
	}

	// Si llegué acá es porque todos los módulos estaban en alguna de dos
	// situaciones:
	// 1) Marcado como imposible de utilizar
	// 2) Con todas sus dependencias posibles de encontrar
	
	// Elimino los módulos imposibles de utilizar
	it = modules_.begin();
	for (; it != itEnd;)
	{
		if (!it->second)
			modules_.erase(it++);
		else
			++it;
	}

	// Satisfago las dependencias
	it = modules_.begin();
	for (; it != itEnd; ++it)
	{
		// Busco los requerimientos
		const vector<TReq>& reqVec = it->second->getReqMods();
		
		// Armo el vector de requerimientos
		vector<shared_ptr<Module> > reqs;
		for (size_t i = 0; i < reqVec.size(); i++)
			reqs.push_back(modules_[reqVec[i].first]);

		// Lo entrego al módulo
		if (!it->second->giveReqMods(reqs))
			throw; // FIXME: Lanzar algo más específico
	}
}