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
// memory_mgr.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 10 de abril de 2008
//=============================================================================

#include <algorithm>
#include <boost/unordered_set.hpp>

namespace
{
	/// Guarda los pedidos de memoria
	boost::unordered_set<void*> allocatedMemory;
}

extern "C" void* my_tmalloc(size_t cant)
{
	void* ret = calloc(cant, 1);
	return ret;
}

extern "C" void* my_trealloc(void* p, size_t new_size)
{
	void* ret = realloc(p, new_size);
	return ret;
}

extern "C" void my_txfree(void* p)
{
	if (p)
		free(p);
}

extern "C" void record_alloc(void * p)
{
	allocatedMemory.insert(p);
}

extern "C" void unrecord_alloc(void* p)
{
	allocatedMemory.erase(p);
}

extern "C" void free_allocated_memory()
{
	std::for_each(allocatedMemory.begin(),
		allocatedMemory.end(), my_txfree);
	allocatedMemory.clear();
}