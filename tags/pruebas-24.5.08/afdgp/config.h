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
// config.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include <vector>

namespace Core
{
	/// Clase base abstracta de las configuraciones
	class Config
	{
	public:
		/// Lee un valor dada su clave
		virtual std::string readValue(const std::string& key) const = 0;

		/// Lee un valor dada su clave con un valor por defecto
		virtual std::string readValue(const std::string& key, 
			const std::string& defaultValue) const = 0;

		/// Se fija si est� una clave dada presente
		virtual bool hasKey(const std::string& key) const = 0;

		/// Obtiene las claves
		virtual std::vector<std::string> getKeys() const = 0;

		/// Obtiene un map con los pares clave-valor
		virtual std::map<std::string, std::string> getKeyValuePairs() const = 0;

		/// Obtiene una configuraci�n "conteniendo" todos los pares 
		/// clave-valor cuyas claves comiencen con un cierto prefijo
		boost::shared_ptr<Config> getView(const std::string& prefix) const;

		/// Obtiene una configuraci�n que resulta de buscar primero en esta 
		/// configuraci�n y despu�s en baseConfig
		boost::shared_ptr<Config> 
			getCombinedConfig(const Config& baseConfig) const;
	};
}

#endif
