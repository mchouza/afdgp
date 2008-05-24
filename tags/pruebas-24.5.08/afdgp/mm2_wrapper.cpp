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
// mm2_wrapper.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 26 de abril de 2008
//=============================================================================

#include "mm2_wrapper.h"
#include <algorithm>
#include <iterator>

using boost::uint64_t;

uint64_t Util::mmHash2(const void* buffer, size_t len)
{
	using Util::Aux::MurmurHash2;
	
	uint64_t hash = MurmurHash2(buffer, static_cast<int>(len), HASH_SEED_HI);
	hash <<= 32;
	hash |= MurmurHash2(buffer, static_cast<int>(len), HASH_SEED_LO);
	return hash;
}

uint64_t Util::mmHash2(const std::string& s)
{
	return mmHash2(s.data(), s.size());
}

uint64_t Util::mmHash2(std::istream& is)
{
	// FIXME: No muy eficiente... pero mmHash2 no es incremental.
	
	using std::back_insert_iterator;
	using std::copy;
	using std::istream_iterator;
	using std::vector;
	
	istream_iterator<unsigned char> isIt(is), isItEnd;
	vector<unsigned char> buffer;
	back_insert_iterator<vector<unsigned char> > bufferInserter(buffer);

	copy(isIt, isItEnd, bufferInserter);

	return mmHash2(buffer);
}
