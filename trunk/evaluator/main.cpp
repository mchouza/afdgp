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
// Creado por Mariano M. Chouza | Creado el 30 de mayo de 2008
//=============================================================================

#include <boost/shared_ptr.hpp>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include "escape.h"
#include "i_mod_spice.h"
#include "module_library.h"

namespace
{
	using std::string;
	using std::vector;

	/// Evalúa una netlist "escaped", devolviendo el resultado de dicha 
	/// evaluación
	string evalEscNetlist(IModSpice& sm, const string& escNetlist)
	{
		vector<char> result;
		bool ret = sm.procNetlist(Util::unescapeString(escNetlist), 
			result, false);
		if (ret)
			return string(&result[0], result.size());
		else
			return "ERROR";
	}
}

/// Entry point
int main(int argc, char* argv[])
{
	// Me fijo que no pasen argumentos
	if (argc != 1)
	{
		std::cerr << "Sintaxis: evaluator\n";
		return 1;
	}

	// Atrapo cualquier excepción que pudiera ocurrir
	try
	{
		// Armo la biblioteca de módulos y obtengo el módulo del SPICE
		Core::ModuleLibrary lib(".;..\\debug;..\\release;..\\dontexist");
		boost::shared_ptr<Module> pSM = lib.getModuleByName("Spice");
		IModSpice& sm = dynamic_cast<IModSpice&>(*pSM);

		// Evalúo cada netlist que se pase por entrada estándar mostrando los
		// resultados por salida estándar
		std::string escNetlist;
		while (std::getline(std::cin, escNetlist))
			std::cout << evalEscNetlist(sm, escNetlist);
	}
	catch (std::exception& e)
	{
		std::cerr << "Excepción: " << e.what() << "\n";
		return 2;
	}
	catch (...)
	{
		std::cerr << "Excepción desconocida\n";
		return 3;
	}

	// OK
	return 0;
}
