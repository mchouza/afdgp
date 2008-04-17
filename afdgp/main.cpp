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
// main.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#include "afdgp_app.h"
#include <boost/signal.hpp>
#include <csignal>
#include <iostream>

namespace
{
	/// Se�al de Boost representando SIGINT
	boost::signal<void ()> sigint;
	
	/// Maneja una se�al SIGINT
	void sigintHandler(int)
	{
		using std::cerr;
		
		// Indico que recib� el mensaje
		cerr << "\nPedido de terminaci�n recibido...\n";
		
		// Se�ala usando la se�al de Boost
		sigint();

		// Contin�o manejando la se�al
		signal(SIGINT, sigintHandler);
	}

	/// Conecta SIGINT a la aplicaci�n
	void connectSIGINT2App(Core::AFDGPApp& app)
	{
		using Core::AFDGPApp;

		// Functor privado para invocar a exit
		struct AppExitFunctor
		{
			AFDGPApp& app_;

			AppExitFunctor(AFDGPApp& app) :
			app_(app)
			{
			}

			void operator()()
			{
				app_.exit();
			}
		} appExitFunctor(app);
		
		// Conecto la se�al SIGINT al handler
		signal(SIGINT, sigintHandler);

		// Conecto la aplicaci�n a la se�al de boost
		sigint.connect(appExitFunctor);
	}
}

int main(int argc, char* argv[])
{
	using Core::AFDGPApp;
	using std::cerr;
	using std::endl;

	// Atrapo las excepciones que lleguen a este nivel para, al menos, indicar
	// un mensaje de error con sentido
	try
	{
		// Construyo la aplicaci�n
		AFDGPApp app(argc, argv);

		// Inicializa manejo d ese�ales
		connectSIGINT2App(app);

		// Ejecuto el trabajo que tenga que hacer
		app.run();
	}
	catch (std::exception& e)
	{
		// Indico la clase de error producido
		cerr << "ERROR IRRECUPERABLE: " << e.what() << endl;
	}
	catch (...)
	{
		// Muestro un mensaje de error. No es demasiado espec�fico, pero es
		// por que no puedo serlo
		cerr << "Se ha producido un error desconocido." << endl;
	}

	// OK
	return 0;
}
