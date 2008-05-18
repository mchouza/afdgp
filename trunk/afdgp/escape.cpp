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
// escape.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 16 de mayo de 2008
//=============================================================================

#include "escape.h"
#include <cctype>
#include <climits>
#include <cstdlib>

namespace
{
	/// Tabla base
	const char baseTable[][2] =
	{
		{'\\', '\\'},
		{'n', '\n'},
		{'t', '\t'}
	};

	/// Obtiene la tabla de 'escaping'
	const char* getEscTable()
	{
		static char table[1 << CHAR_BIT];
		static bool tableInitialized = false;

		if (!tableInitialized)
		{
			memset(table, 0, sizeof(table));
			for (size_t i = 0;
				i < sizeof(baseTable) / sizeof(baseTable[0]); 
				i++)
			{
				table[baseTable[i][1]] = baseTable[i][0];
			}
			tableInitialized = true;
		}

		return table;
	}

	/// Obtiene la tabla de 'unescaping'
	const char* getUnEscTable()
	{
		static char table[1 << CHAR_BIT];
		static bool tableInitialized = false;

		if (!tableInitialized)
		{
			memset(table, 0, sizeof(table));
			for (size_t i = 0;
				i < sizeof(baseTable) / sizeof(baseTable[0]); 
				i++)
			{
				table[baseTable[i][0]] = baseTable[i][1];
			}
			tableInitialized = true;
		}

		return table;
	}
}

namespace Util
{
	std::string unescapeString(const std::string& input)
	{
		using std::string;

		string::const_iterator it(input.begin()), itEnd(input.end());
		string ret(itEnd - it, ' ');
		string::iterator outIt(ret.begin());

		const char* unEscTable = getUnEscTable();
		
		for (; it != itEnd; ++it, ++outIt)
		{
			if (*it == '\\')
			{
				*outIt = unEscTable[*++it];
				if (!*outIt)
					throw 0; // FIXME: Lanzar algo más específico
			}
			else
			{
				*outIt = *it;
				if (!isprint(*outIt))
					throw 0; // FIXME: Lanzar algo más específico
			}
		}

		ret.resize(outIt - ret.begin());
		return ret;
	}

	std::string escapeString(const std::string& input)
	{
		using std::string;
		
		string ret;

		string::const_iterator it(input.begin()), itEnd(input.end());

		const char* escTable = getEscTable();

		for (; it != itEnd; ++it)
		{
			if (isprint(*it))
			{
				ret.push_back(*it);
			}
			else
			{
				ret.push_back('\\');
				char c = escTable[*it];
				if (!c)
					throw 0; // FIXME: Lanzar algo más específico
				ret.push_back(c);
			}
		}

		return ret;
	}
}