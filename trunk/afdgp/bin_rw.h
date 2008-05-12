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
// bin_rw.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 12 de mayo de 2008
//=============================================================================

#ifndef BIN_RW_H
#define BIN_RW_H

namespace Util
{
	/// Lee una variable desde una stream binaria
	template <typename T> 
	T readFromBinStream(std::istream& is)
	{
		T ret;
		is.read(reinterpret_cast<char*>(&ret), sizeof(T));
		return ret;
	}

	/// Lee un conjunto de variables desde una stream binaria
	template <typename T>
	void readFromBinStream(std::istream& is, T* buffer, size_t numElems)
	{
		is.read(reinterpret_cast<char*>(buffer), 
			static_cast<std::streamsize>(numElems * sizeof(T)));
	}

	/// Escribe una variable en una stream binaria
	template <typename T> 
	void writeToBinStream(std::ostream& os, const T& a)
	{
		os.write(reinterpret_cast<const char*>(&a), sizeof(T));
	}

	/// Escribe un conjunto de variables a una stream binaria
	template <typename T>
	void writeToBinStream(std::ostream& os, const T* buffer, size_t numElems)
	{
		os.write(reinterpret_cast<const char*>(buffer), 
			static_cast<std::streamsize>(numElems * sizeof(T)));
	}
}

#endif
