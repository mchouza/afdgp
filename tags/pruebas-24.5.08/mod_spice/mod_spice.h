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
// mod_spice.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 10 de abril de 2008
//=============================================================================

#ifndef MOD_SPICE_H
#define MOD_SPICE_H

#include <i_mod_spice.h>

class MODULE_API ModSpice : public IModSpice
{
public:
	// Constructor sin par�metros
	ModSpice();

	// Destructor
	virtual ~ModSpice();

	// Definidas por la interfaz
	virtual const std::string& getName() const {return name_;};
	virtual const std::vector<Req>& getReqMods() const {return reqs_;}
	virtual unsigned int getVersion() const {return version_;}
	virtual bool giveConfigData(const std::map<std::string, std::string>& configData);
	virtual bool giveReqMods(const std::vector<boost::shared_ptr<Module> >& reqMods);

	/// Procesa una netlist
	virtual bool procNetlist(const std::string& netlist,
		std::vector<char>& result, bool binaryResults) const;

private:
	/// Nombre del m�dulo
	static const std::string name_;

	/// Versi�n del m�dulo
	static const unsigned int version_;

	/// IDs de los m�dulos requeridos
	static const std::vector<Req> reqs_;

	/// File pointer del archivo nulo
	FILE* nulFP_;
};

#endif
