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
// config_file.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#include "config_file.h"
#include "escape.h"
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <boost/regex.hpp>
#include <fstream>

using namespace Core;

ConfigFile::ConfigFile(const std::string& filename)
{
	// Desde un archivo .properties
	loadFromPropertiesFile(filename);
}

ConfigFile::ConfigFile(std::istream& propertiesStream)
{
	// Desde una stream en formato .properties
	loadFromPropertiesStream(propertiesStream);
}

std::string ConfigFile::readValue(const std::string& key) const
{
	TConfigMap::const_iterator it = configMap_.find(key);
	if (it == configMap_.end())
		throw; // FIXME: Lanzar algo con cierto sentido
	return it->second;
}

std::string ConfigFile::readValue(const std::string& key, 
								  const std::string& defaultValue) const
{
	TConfigMap::const_iterator it = configMap_.find(key);
	if (it == configMap_.end())
		return defaultValue;
	else
		return it->second;
}

bool ConfigFile::hasKey(const std::string& key) const
{
	return configMap_.find(key) != configMap_.end();
}

std::vector<std::string> ConfigFile::getKeys() const
{
	using std::string;
	using std::vector;
	
	vector<string> ret;

	TConfigMap::const_iterator it = configMap_.begin();
	for (; it != configMap_.end(); ++it)
		ret.push_back(it->first);

	return ret;
}

std::map<std::string, std::string> ConfigFile::getKeyValuePairs() const
{
	return configMap_;
}

void ConfigFile::loadFromPropertiesStream(std::istream& propStream)
{
	using boost::algorithm::trim;
	using boost::regex;
	using boost::regex_match;
	using boost::smatch;
	using Util::unescapeString;

	// Expresiones regulares para distintos tipos de líneas
	regex reBlankOrCom("^(#.*)|(?:)$");
	regex reAssignment("^([A-Za-z.-]+)\\s*=\\s*(\\S.*)?");
	
	smatch matchRes;

	// Limpio los datos
	configMap_.clear();

	// Lo leo línea a línea
	std::string line;
	while (std::getline(propStream, line))
	{
		// Saco espacios al principio y al final
		trim(line);
		
		// Si está en blanco o es un coemntario, la ignoro
		if (regex_match(line, reBlankOrCom))
			continue;

		// Uso una expresión regular para partirlo en un par clave-valor
		if (!regex_match(line, matchRes, reAssignment))
			// No es ni comentario, ni línea en blanco ni asignación
			throw; // FIXME: Lanzar algo más significativo

		// Guardo ese par en el mapa (no me preocupo de pisar pares anteriores)
		configMap_[matchRes.str(1)] = unescapeString(matchRes.str(2));
	}
}

void ConfigFile::loadFromPropertiesFile(const std::string& filename)
{
	// Trato de abrir el archivo y lanzo una excepción si no está
	std::ifstream propFileStream(filename.c_str());
	if (!propFileStream.is_open())
			throw; // FIXME: Lanzar algo más significativo

	// Cargo a partir de la stream
	loadFromPropertiesStream(propFileStream);
}

void ConfigFile::saveToPropertiesFile(const std::string &filename) const
{
	// Abro el archivo de salida
	std::ofstream out(filename.c_str());
	if (!out.is_open())
		throw; // FIXME: Lanzar algo más significativo

	// Indico que fue generado automáticamente
	out << "# Archivo generado automáticamente\n";

	// Recorro el map guardando los pares clave-valor
	TConfigMap::const_iterator it = configMap_.begin();
	TConfigMap::const_iterator itEnd = configMap_.end();
	for (; it != itEnd; ++it)
		out << it->first << " = " << it->second << "\n";

	// Se cierra al salir de scope
}