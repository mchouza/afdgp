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
// afdgp_app.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de abril de 2008
//=============================================================================


#include "afdgp_app.h"
#include "afd_job.h"
#include "config_file.h"
#include "module_library.h"
#include "os_dep.h"
#include <iostream>

using namespace Core;

/// Implementaci�n de la aplicaci�n
class AFDGPApp::Impl
{
	/// Configuraci�n general de la app
	ConfigFile config_;

	/// M�dulos de la app
	ModuleLibrary modules_;

	/// Trabajo a ejecutar
	boost::scoped_ptr<AFDJob> pJob_;

	/// Indica si tiene que salir
	bool need2Exit_;

	/// Timer
	Util::OSDep::ProcessTimer timer_;

public:
	Impl(int argc, char* argv[]) :
	config_("config.properties"),
	modules_(config_.readValue("ModulesPath")),
	need2Exit_(false) // No necesita salir cuando empieza!
	{
		// Si tiene m�s de un argumento, sale
		if (argc > 2)
			throw 0; // FIXME: Lanzar algo m�s espec�fico

		// Pongo std::cout como unbuffered para evitar dejar "colgados" a los
		// scripts de testing
		std::cout.setf(std::ios_base::unitbuf);

		// Crea el trabajo
		if (argc == 2)
			pJob_.reset(new AFDJob(modules_, config_, argv[1], std::cout));
		else
			pJob_.reset(new AFDJob(modules_, config_, std::cin, std::cout));
	}

	void run()
	{
		// Mientras no tenga que salir
		while (!need2Exit_)
		{
			// Avanzo un paso
			if (!pJob_->step())
				break;
		}

		// Muestro resultados
		pJob_->printResults();

		// Antes de salir, guardo
		pJob_->makeCheckPoint();
	}

	void exit()
	{
		// FIXME: MUTEX!!!
		need2Exit_ = true;
	}
};

AFDGPApp::AFDGPApp(int argc, char* argv[])
{
	pImpl_.reset(new Impl(argc, argv));
}

AFDGPApp::~AFDGPApp()
{
}

void AFDGPApp::run()
{
	// Delego
	pImpl_->run();
}

void AFDGPApp::exit()
{
	// Delego
	pImpl_->exit();
}