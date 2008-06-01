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
// config_file.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include "config.h"

namespace Core
{
	/// Configuración a partir de un archivo
	class ConfigFile : public Config
	{
		/// Tipo del mapa con los datos
		typedef std::map<std::string, std::string> TConfigMap;
		
		/// Mapa con los datos
		TConfigMap configMap_;

		/// Carga el mapa con los datos desde un archivo .properties
		void loadFromPropertiesFile(const std::string& filename);

		/// Carga el mapa con los datos desde una stream en formato .properties
		void loadFromPropertiesStream(std::istream& propStream);

		/// Guarda el mapa en un archivo .properties
		void saveToPropertiesFile(const std::string& filename) const;

	public:
		/// Constructor a partir de un archivo
		ConfigFile(const std::string& filename);

		/// Constructor a partir de una istream
		ConfigFile(std::istream& is);

		/// Lee un valor dada su clave
		virtual std::string readValue(const std::string& key) const;

		/// Lee un valor dada su clave con un valor por defecto
		virtual std::string readValue(const std::string& key, 
			const std::string& defaultValue) const;

		/// Observa si tiene una nueva clave
		virtual bool hasKey(const std::string& key) const;

		/// Obtiene las claves
		virtual std::vector<std::string> getKeys() const;

		/// Obtiene los pares clave-valor
		virtual std::map<std::string, std::string> getKeyValuePairs() const;
	};
}

#endif
