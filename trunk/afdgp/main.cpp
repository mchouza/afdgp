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
	/// Señal de Boost representando SIGINT
	boost::signal<void ()> sigint;
	
	/// Maneja una señal SIGINT
	void sigintHandler(int)
	{
		using std::cerr;
		
		// Indico que recibí el mensaje
		cerr << "\nPedido de terminación recibido...\n";
		
		// Señala usando la señal de Boost
		sigint();

		// Continúo manejando la señal
		signal(SIGINT, sigintHandler);
	}

	/// Conecta SIGINT a la aplicación
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
		
		// Conecto la señal SIGINT al handler
		signal(SIGINT, sigintHandler);

		// Conecto la aplicación a la señal de boost
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
		// Construyo la aplicación
		AFDGPApp app(argc, argv);

		// Inicializa manejo d eseñales
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
		// Muestro un mensaje de error. No es demasiado específico, pero es
		// por que no puedo serlo
		cerr << "Se ha producido un error desconocido." << endl;
	}

	// OK
	return 0;
}
