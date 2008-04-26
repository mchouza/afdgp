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
// test_hash.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 19 de abril de 2008
//=============================================================================

#include "common.h"
#include "mm2_wrapper.h"
#include <algorithm>
#include <ctime>
#include <iostream>

template <typename U, typename V>
inline bool compare_only_second_members(const std::pair<U, V>& a,
										const std::pair<U, V>& b)
{
	return a.second < b.second;
}

void test_hash_time()
{
	std::vector<int> v(50000000);

	for (size_t i = 0; i < v.size(); i++)
		v[i] = rand();

	clock_t st = clock();
	unsigned __int64 hash = Util::mmHash2(v);
	clock_t end = clock();

	std::cout << "Hashing " << v.size() * sizeof(int) / (1 << 20) << " MB\n";
	std::cout << "Hash: " << std::hex << hash << "\n";
	std::cout << "Tiempo: " << static_cast<double>(end - st) / CLOCKS_PER_SEC
		<< "\n";
}

void test_hash_collisions()
{
	std::cout << "Inicializando datos de prueba...\n";
	std::vector<std::vector<char> > v(1 << 20);
	for (size_t i = 0; i < v.size(); i++)
	{
		v[i].resize(rand() % 150);
		for (size_t j = 0; j < v[i].size(); j++)
			v[i][j] = rand();
	}
	std::vector<unsigned __int64> hashes(1 << 20);
	std::cout << "Datos de prueba inicializados.\n";
	
	std::cout << "Calculando hashes...\n";
	for (size_t i = 0; i < v.size(); i++)
		hashes[i] = Util::mmHash2(v[i]);
	std::cout << "Hashes calculados.\n";

	std::cout << "Detectando duplicados...\n";
	std::sort(hashes.begin(), hashes.end());
	std::vector<unsigned __int64>::iterator itHashesNewEnd =
		std::unique(hashes.begin(), hashes.end());
	std::sort(v.begin(), v.end());
	std::vector<std::vector<char> >::iterator itDataNewEnd =
		std::unique(v.begin(), v.end());
	std::cout << "Se han detectado " 
		<< static_cast<int>(hashes.end() - itHashesNewEnd)
		<< " duplicados en los hashes.\n";
	std::cout << "Se han detectado " 
		<< static_cast<int>(v.end() - itDataNewEnd)
		<< " duplicados en los datos.\n";
}

void find_hash_collisions()
{
	typedef std::vector<std::pair<std::vector<char>, unsigned __int64> >
		TDataVec;

	unsigned seed = static_cast<unsigned int>(time(0));
	srand(seed);

	std::cout << "Inicializando datos de prueba...\n";
	TDataVec v(1 << 20);
	for (size_t i = 0; i < v.size(); i++)
	{
		v[i].first.resize(rand() % 75 + 75);
		for (size_t j = 0; j < v[i].first.size(); j++)
			v[i].first[j] = rand();
	}
	std::cout << "Datos de prueba inicializados.\n";
	
	std::cout << "Calculando hashes...\n";
	for (size_t i = 0; i < v.size(); i++)
		v[i].second = Util::mmHash2(v[i].first);
	std::cout << "Hashes calculados.\n";

	std::cout << "Eliminando duplicados...\n";
	std::sort(v.begin(), v.end());
	TDataVec::iterator itNewEnd = std::unique(v.begin(), v.end());
	std::cout << "Se encontraron " << static_cast<int>(v.end() - itNewEnd)
		<< " valores duplicados.\n";
	v.resize(itNewEnd - v.begin());
	
	std::cout << "Reordenando para buscar hashes duplicados...\n";
	std::sort(v.begin(), v.end(),
		compare_only_second_members<std::vector<char>, __int64>);
	std::cout << "Reordenamiento completado.\n";

	std::cout << "Buscando hashes duplicados...\n";
	for (size_t i = 0; i < v.size() - 1; i++)
	{
		if (v[i].second == v[i+1].second)
		{
			std::cout << "Se ha encontrado una colisión!\n";
			std::cout << "Seed: " << seed << "\n";
			std::cout << "Hash: " << std::hex << v[i].second << "\n";
			std::cout << "Vector 1 (hexa): ";
			for (size_t j = 0; j < v[i].first.size(); j++)
				std::cout << std::hex
				<< static_cast<int>(v[i].first[j]) << " ";
			std::cout << "\n";
			std::cout << "Vector 2 (hexa): ";
			for (size_t j = 0; j < v[i+1].first.size(); j++)
				std::cout << std::hex
				<< static_cast<int>(v[i+1].first[j]) << " ";
			std::cout << "\n";
			return;
		}
	}
	std::cout << "No se encontraron hashes duplicados.\n";
}

#ifdef TEST_HASH

int main()
{
	//test_hash_time();
	//test_hash_collisions();
	find_hash_collisions();
}

#endif
