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
// mod_spice.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 10 de abril de 2008
//=============================================================================

#include "mod_spice.h"
#include <cassert>
#include <fstream>

extern "C"
{
	#include "memfile.h"
}

using boost::shared_ptr;
using std::map;
using std::string;
using std::vector;

const string ModSpice::name_ = "Spice";
const unsigned int ModSpice::version_ = 0;
// No requiere otros módulos; es "standalone".
const std::vector<Module::Req> ModSpice::reqs_ = vector<Req>();

// Exporto al módulo
EXPORT_MODULE(ModSpice)

extern "C" int processNetList(const char* netlist,
							  const char* outputFilename, FILE* nulFP,
							  int binaryResults);
extern "C" void free_allocated_memory();

bool ModSpice::giveReqMods(const vector<shared_ptr<Module> > &reqMods)
{
	// No requiere módulos, devuelvo error si me los dan
	return reqMods.size() == 0;
}

bool ModSpice::giveConfigData(const map<string,string>& configData)
{
	// FIXME: No hace nada
	return true;
}

bool ModSpice::procNetlist(const string& netlist, vector<char>& result,
						   bool binaryResults) const
{
	// FIXME: Agregar detección de errores
	//MyProfiler q("ProcNetlist C++ wrapper");
	// Código de error
	int errCode = 0;
	
	// Llama al SPICE para que haga el trabajo y lo guardo en el MEMFILE
	// llamado 'a'
	{
		//MyProfiler p("ProcessNetList");
		errCode = processNetList(	netlist.c_str(),
									"a", nulFP_, binaryResults ? 1 : 0);
	}

	// Obtengo los datos
	MEMFILE* mfp = memfopen("a", "r");
	size_t outSize = memfgetsize(mfp);
	if (outSize > 0 && errCode == 0)
	{
		result.resize(outSize);
		memfread(&result[0], outSize, 1, mfp);
	}

	// Libero a los archivos en memoria
	memfgencleanup();

	// Limpio lo que haya dejado el SPICE
	free_allocated_memory();
	
	// OK
	return true;
}

ModSpice::ModSpice()
{
	nulFP_ = fopen("NUL", "w");
}

ModSpice::~ModSpice()
{
	//MyProfiler::generate_report();
	fclose(nulFP_);
}
