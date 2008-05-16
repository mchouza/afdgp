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
// afd_job.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de abril de 2008
//=============================================================================

#include "afd_job.h"
#include "bin_rw.h"
#include "config.h"
#include "config_file.h"
#include "evolver.h"
#include "mm2_wrapper.h"
#include "stats_collector.h"
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

using namespace Core;

namespace
{
	/// Devuelve el nombre del archivo binario donde se almacena la poblaci�n
	std::string getBinFileName(const std::string& id, const Core::Config& c)
	{
		return c.readValue("JobsInProgressPath") + "/" +
			id + ".bin";
	}
}

AFDJob::AFDJob(const ModuleLibrary& lib,
			   const Config& baseConfig, 
			   const std::string& filename,
			   std::ostream& statsStream) :
config_(baseConfig),
filename_(filename),
numberOfRuns_(0),
numberOfGens_(0),
runNum_(0),
genNum_(0),
statsStream_(statsStream)
{
	using boost::lexical_cast;

	// Obtengo la configuraci�n para este trabajo
	Core::ConfigFile specConfig(filename);

	// Creo el evolver con la configuraci�n base y la espec�fica a este trabajo
	pEvolver_.reset(new GP::Evolver(lib, config_, specConfig));

	// Muestro las generalidades
	pEvolver_->getStatsCollector().printGeneralParameters(statsStream_);

	// Muestro la fila de descripci�n de las estad�sticas por generaci�n
	pEvolver_->getStatsCollector().printStatsByGenHeader(statsStream_);

	// Obtengo el ID
	id_ = specConfig.readValue("ID");

	// Obtengo el n�mero de ejecuciones
	numberOfRuns_ = 
		lexical_cast<unsigned>(specConfig.readValue("NumberOfRuns"));

	// Obtengo el n�mero de generaciones
	numberOfGens_ = 
		lexical_cast<unsigned>(specConfig.readValue("NumberOfGens"));


	// Carga los datos de la ejecuci�n anterior, si es necesario
	// FIXME: Habilitar!!
	//resumeIfPossible();
}

AFDJob::~AFDJob()
{
}

void AFDJob::makeCheckPoint() const
{
	// Por Secci�n 27.8.1.1 P�rrafo 3 del standard, fstream::seekg() y 
	// fstream::seekp() son efectivamente lo mismo

	using boost::uint64_t;
	using std::fstream;
	using Util::mmHash2;
	using Util::writeToBinStream;
	
	// Abro el archivo de salida
	fstream binOut(getBinFileName(id_, config_).c_str(),
		std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

	// Escribo un hash nulo
	writeToBinStream<uint64_t>(binOut, 0);

	// Guardo la posici�n desde donde leer
	fstream::pos_type readPos = binOut.tellp();

	// Serializo la poblaci�n y dem�s datos del evolver
	pEvolver_->serialize(binOut);

	// Hago flush antes de leer
	binOut.flush();

	// Calculo el hash de la poblaci�n
	binOut.seekg(readPos);
	uint64_t hash2Save = mmHash2(binOut);

	// Escribo el hash
	binOut.clear();
	binOut.seekp(0);
	writeToBinStream(binOut, hash2Save);
}

bool AFDJob::step()
{
	// Si se pas� de generaci�n...
	if (genNum_ >= numberOfGens_)
	{
		// Actualizo los contadores
		genNum_ = 0;
		runNum_++;

		// Si se pas� de run, listo
		if (runNum_ >= numberOfRuns_)
			return false;

		// Reinicializo el evolver
		pEvolver_->newRun();
	}

	// Realiza el paso evolutivo
	pEvolver_->step();

	// Muestro las estad�sitcas pertinentes
	pEvolver_->getStatsCollector().printStatsByGenRow(statsStream_);

	// Paso a la generaci�n siguiente
	genNum_++;

	// Sigue trabajando
	return true;
}

/// Contin�a la ejecuci�n anterior, si es posible
void AFDJob::resumeIfPossible()
{
	// FIXME: No carga estad�sticas

	using boost::filesystem::exists;
	using boost::uint64_t;
	using std::ifstream;
	using Util::mmHash2;
	using Util::readFromBinStream;

	// Nombre del archivo
	std::string binFileName = getBinFileName(id_, config_);

	// Se fija si est�n los archivos con los datos
	if (!exists(binFileName))
		// Si no est�n, sale
		return;

	// Si est�n, conti�o cargando...
	ifstream binFile(binFileName.c_str(), std::ios::binary);
	if (!binFile.is_open())
		throw; // FIXME: Lanzar algo m�s espec�fico

	// Leo el hash
	uint64_t savedHash = readFromBinStream<uint64_t>(binFile);

	// Guardo la posici�n
	ifstream::pos_type readPos = binFile.tellg();

	// Calculo el hash del resto del archivo
	uint64_t calcHash = mmHash2(binFile);
	
	// Si no coinciden es un error
	if (savedHash != calcHash)
		throw; // FIXME: Lanzar algo m�s espec�fico

	// Pongo en posici�n al stream
	binFile.clear();
	binFile.seekg(readPos);

	// Deserializo los datos del evolver
	pEvolver_->deserialize(binFile);
}

void AFDJob::printResults()
{
	pEvolver_->getStatsCollector().printGlobalStats(statsStream_);
}