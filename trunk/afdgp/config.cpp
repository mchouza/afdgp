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
// config.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#include "config.h"

using namespace Core;

namespace
{
	/// Clase encargada de presentar una vista con un cierto prefijo de config
	class ConfigView : public Config
	{
		/// Prefijo
		std::string prefix_;

		/// Referencia a la configuración base
		const Config& baseConfig_;

	public:
		/// Constructor a partir de un archivo
		ConfigView(const Config& baseConfig, const std::string& prefix) :
		  prefix_(prefix),
		  baseConfig_(baseConfig)
		{
		}

		/// Lee un valor dada su clave
		virtual std::string readValue(const std::string& key) const
		{
			return baseConfig_.readValue(prefix_ + "." + key);
		}

		/// Lee un valor dada su clave con un valor por defecto
		virtual std::string readValue(const std::string& key, 
			const std::string& defaultValue) const
		{
			return baseConfig_.readValue(prefix_ + "." + key, defaultValue);
		}

		/// Se fija si está presente una clave
		virtual bool hasKey(const std::string& key) const
		{
			return baseConfig_.hasKey(prefix_ + "." + key);
		}

		/// Obtiene las claves
		virtual std::vector<std::string> getKeys() const
		{
			using std::string;
			using std::vector;

			string prefixWDot(prefix_ + ".");
			vector<string> baseMapKeys(baseConfig_.getKeys());
			vector<string> ret;

			for (size_t i = 0; i < baseMapKeys.size(); i++)						
				if (baseMapKeys[i].compare(0, prefixWDot.length(), prefixWDot)
					== 0)
					ret.push_back(baseMapKeys[i].substr(prefixWDot.length()));
				
			return ret;
		}

		/// Obtiene los pares clave-valor
		virtual std::map<std::string, std::string> getKeyValuePairs() const
		{
			using std::map;
			using std::string;
			using std::vector;

			// FIXME: Performance deficiente, aunque es simple

			map<string, string> ret;
			vector<string> keys = getKeys();
			for (size_t i = 0; i < keys.size(); i++)
				ret[keys[i]] = readValue(keys[i]);

			return ret;
		}
	};

	/// Clase encargada de combinar configuraciones
	class JoinedConfig : public Config
	{
		/// Configuración primaria
		const Config& primaryConfig_;

		/// Configuración base
		const Config& baseConfig_;

	public:
		/// Construye a partir de las dos configuraciones
		JoinedConfig(const Config& p, const Config& b) :
		  primaryConfig_(p),
		  baseConfig_(b)
		{
		}
		
		/// Lee un valor dada su clave
		virtual std::string readValue(const std::string& key) const
		{
			if (primaryConfig_.hasKey(key))
				return primaryConfig_.readValue(key);
			else if (baseConfig_.hasKey(key))
				return baseConfig_.readValue(key);
			else
				throw 0; // FIXME: Lanzar algo más específico
		}

		/// Lee un valor dada su clave con un valor por defecto
		virtual std::string readValue(const std::string& key, 
			const std::string& defaultValue) const
		{
			if (primaryConfig_.hasKey(key))
				return primaryConfig_.readValue(key);
			else if (baseConfig_.hasKey(key))
				return baseConfig_.readValue(key);
			else
				return defaultValue;
		}

		/// Se fija si tiene una clave
		virtual bool hasKey(const std::string& key) const
		{
			return primaryConfig_.hasKey(key) || baseConfig_.hasKey(key);
		}

		/// Obtiene las claves
		virtual std::vector<std::string> getKeys() const
		{
			// FIXME: Probablemente ineficiente
			// FIXME: Puede que tenga duplicados
			std::vector<std::string> pck = primaryConfig_.getKeys();
			std::vector<std::string> bck = baseConfig_.getKeys();
			pck.insert(pck.end(), bck.begin(), bck.end());
			return pck;
		}

		/// Obtiene los pares clave-valor
		virtual std::map<std::string, std::string> getKeyValuePairs() const
		{
			std::map<std::string, std::string> ret, aux;
			ret = primaryConfig_.getKeyValuePairs();
			aux = baseConfig_.getKeyValuePairs();
			ret.insert(aux.begin(), aux.end());
			return ret;
		}
	};
}

boost::shared_ptr<Config> Config::getView(const std::string& prefix) const
{
	using boost::shared_ptr;
	
	return shared_ptr<Config>(new ConfigView(*this, prefix));
}

boost::shared_ptr<Config> 
Config::getCombinedConfig(const Config& baseConfig) const
{
	using boost::shared_ptr;

	return shared_ptr<Config>(new JoinedConfig(*this, baseConfig));
}