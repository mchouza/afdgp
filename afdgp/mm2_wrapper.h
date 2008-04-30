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
// mm2_wrapper.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 19 de abril de 2008
//=============================================================================

#ifndef MM2_WRAPPER_H
#define MM2_WRAPPER_H

// Para avisarme de los problemas en otras plataformas
#ifndef WIN32
#error Plataforma no soportada
#endif

#include "murmurhash2.h"
#include "plat_dep.h"
#include <string>
#include <vector>

namespace Util
{
	/// Seed para las funciones hash (parte baja)
	const uint32_t HASH_SEED_LO = 0x37b05be6;

	/// Seed para las funciones hash (parte alta)
	const uint32_t HASH_SEED_HI = 0x71dd1e57;
	
	/// Obtiene el hash de un buffer de bytes
	uint64_t mmHash2(const void* buffer, size_t len);

	/// Obtiene el hash de un vector de la STL. Utiliza MurmurHash2.
	template <typename T>
	uint64_t mmHash2(const std::vector<T>& v)
	{
		if (v.empty())
			return mmHash2(0, 0);
		return mmHash2(&v[0], v.size());
	}

	/// Obtiene el hash de una string de la STL. Utiliza MurmurHash2.
	uint64_t mmHash2(const std::string& s);
}

#endif

