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
// mod_eval_panfilter.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#include "mod_eval_panfilter.h"
#include "panf_genome_exec_wrapper.h"
#include "profiling.h"
#include "spice_ac_out.h"
#include <boost/lexical_cast.hpp>
#include <complex>
#include <limits>
#include <sstream>

using boost::lexical_cast;
using boost::shared_ptr;
using std::abs;
using std::complex;
using std::istringstream;
using std::map;
using std::numeric_limits;
using std::string;
using std::stringstream;
using std::vector;

namespace
{
	/// Para elevar al cuadrado
	template <typename T>
	inline T square(T x)
	{
		return x * x;
	}
}

// Exporto al módulo
EXPORT_MODULE(ModEvalPAnFilter)

const string ModEvalPAnFilter::name_ = "EvalPassiveAnalogFilter";	
const unsigned int ModEvalPAnFilter::version_ = 0;

// Solo requiere al módulo del SPICE
const vector<Module::Req> ModEvalPAnFilter::reqs_ =
	std::vector<Module::Req>(
		1,
		// SPICE
		Module::Req("Spice", 0));

// FIXME: Solo para impresión con propósitos de debugging
#include <iostream>

ModEvalPAnFilter::~ModEvalPAnFilter()
{
	// Obtengo los datos estadísticos
	//MyProfiler::generate_report();
}

bool ModEvalPAnFilter::giveReqMods(const vector<shared_ptr<Module> > &reqMods)
{
	using boost::dynamic_pointer_cast;
	
	// Debe ser uno solo
	if (reqMods.size() != 1)
		return false;
	
	// Me fijo que sea el del SPICE
	if (reqMods[0]->getName() != "Spice")
		return false;

	// La versión no importa

	// Guardo el puntero
	pSpiceModule_ = dynamic_pointer_cast<IModSpice>(reqMods[0]);

	// OK
	return true;
}

// FIXME: Faltan más datos
// FIXME: Organizarlo mejor
bool ModEvalPAnFilter::giveConfigData(const map<string, string>& configData)
{
	// Iteradores
	map<string, string>::const_iterator itIE, itTol, itPN, itDT, itPenZ,
		itPenMult, itStartFreq, itEndFreq, itPointsPerDec;
	
	// Sí no está el embrión inicial, es un error
	if ((itIE = configData.find("InitialEmbryo")) == configData.end())
		return false;

	// Si no está la tolerancia es un error
	if ((itTol = configData.find("Tolerance")) == configData.end())
		return false;

	// Si no está la transferencia deseada es un error
	if ((itDT = configData.find("DesiredTransfer")) == configData.end())
		return false;

	// Si no están lso iteradores de las penalidades es un error
	if ((itPenZ = configData.find("PenaltyZero")) == configData.end() ||
		(itPenMult = configData.find("PenaltyMultiplier")) == configData.end())
		return false;

	// Si no están los parámetros de análisis
	if ((itStartFreq = configData.find("StartFreq")) == configData.end() ||
		(itEndFreq = configData.find("EndFreq")) == configData.end() ||
		(itPointsPerDec = configData.find("PointsPerDec")) == configData.end())
		return false;

	// Construyo al embrión inicial
	pEmbryo_.reset(new Individual(istringstream(itIE->second)));

	// Actualizo al "component namer" con los componentes del embrión
	embryoCompNamer_.advance(istringstream(itIE->second));

	// Creo al traductor
	pTransl_.reset(new ComponentValueTransl(lexical_cast<unsigned>(itTol->second)));

	// Incializo la transferencia buscada
	pDesiredTransfer_.reset(new TransferSpec(itDT->second));

	// Inicializo los valores de penalidad
	expPenaltyZero_ = lexical_cast<double>(itPenZ->second);
	expPenaltyMult_ = lexical_cast<double>(itPenMult->second);

	// Inicializo los valores del barrido
	startFreq_ = lexical_cast<double>(itStartFreq->second);
	endFreq_ = lexical_cast<double>(itEndFreq->second);
	pointsPerDec_ = lexical_cast<unsigned>(itPointsPerDec->second);

	// OK
	return true;
}

double ModEvalPAnFilter::evaluateGenome(const TGenome& genome) const
{
	// FIXME: Solo para profiling
	// Mido todo el tiempo que se pasa evaluando
	//MyProfiler p("Eval");
	
	// Si no le dieron lo que pide, no funciona
	if (!pSpiceModule_)
		// FIXME: Lanzar algo específico
		throw;

	// Obtiene la netlist y el nodo que nos interesa
	string netlist;
	unsigned probedNode;
	getNetlist(genome, netlist, probedNode);

	// Realiza una evaluación de la netlist utilziando SPICE
	double spiceScore =  spiceAnalysis(netlist, probedNode);

	// Devuelve el puntaje del SPICE más una penalidad por tamaño
	return spiceScore +
		std::exp(expPenaltyMult_ * (genome.size() - expPenaltyZero_));
}

void ModEvalPAnFilter::getNetlist(const TGenome& genome, string& netlist,
								  unsigned& probedNode) const
{
	// FIXME: Solo para profiling
	// Mido el tiempo que se pasa construyendo la netlist
	//MyProfiler p("GetNetlist");
	
	// Creo un individuo en base al embrión
	Individual indiv(*pEmbryo_);

	// Le aplico el proceso de construcción
	PANFGenomeExecWrapper wrapper(genome);
	wrapper.execute(indiv.getWritingHeads(), embryoCompNamer_, *pTransl_);

	// Numero los vértices en forma consecutiva
	indiv.reenumerateVertices();

	// Simplifico al individuo para que el SPICE lo acepte
	indiv.spiceCleanUp();

	// Numero los vértices en forma consecutiva
	indiv.reenumerateVertices();

	// Convierto el individuo desarrollado en una netlist
	stringstream ssNetList;
	indiv.convertToNetlist(ssNetList, false);

	// Agrego al final un epílogo de análisis
	addAnalysisEpilog(ssNetList);

	// Paso la string de la netlist al parámetro
	netlist.assign(ssNetList.str());

	// Indico cual es el nodo analizado
	probedNode = indiv.getProbedNode();
}

double ModEvalPAnFilter::spiceAnalysis(const string& netlist,
									   unsigned probedNode) const
{
	// FIXME: Solo para profiling
	// Mido el tiempo que se pasa haciendo el análisis con el SPICE
	//MyProfiler p("SPICEAnalysis");
	
	// Por si hay problemas al evaluarlo o la salida del SPICE es cualquier
	// cosa
	try
	{
		// Envío dicha netlist al SPICE
		vector<char> spiceResult;
		pSpiceModule_->procNetlist(netlist, spiceResult);

		// Interpreto los resultados del SPICE
		SpiceACOut interpretedResult(spiceResult);

		// Comparo los resultados del SPICE con los buscados para obtener el
		// score
		return compareResults(interpretedResult,
			lexical_cast<string>(probedNode));
	}
	catch (...)
	{
		// En ese caso devuelvo el peor puntaje: + infinito
		return numeric_limits<double>::infinity();
	}
}

double ModEvalPAnFilter::compareResults(const SpiceACOut&
										interpretedResults,
										const string& probedNodeName) const
{
	// FIXME: Solo para profiling
	//MyProfiler p("ModEvalPAnFilter::CompareResults");
	
	// FIXME: Demasiado simplista, solo uso diferencias al cuadrado

	// Obtengo el vector de valores de tensión para el nodo
	vector<complex<double> > voltageVec;
	interpretedResults.getNodeVoltages(probedNodeName, voltageVec);

	// Obtengo el vector de valores de frecuencia
	const vector<double>& freqVec = interpretedResults.getFreqs();

	// Obtengo la trnsferencia deseada
	const TransferSpec& desiredTransfer = *pDesiredTransfer_;

	assert(voltageVec.size() == freqVec.size());

	// Recorro ambos comparando con los datos base
	double acumDif = 0.0;
	for (size_t i = 0; i < voltageVec.size(); i++)
		acumDif += square(abs(voltageVec[i]) - desiredTransfer(freqVec[i]));
	
	// Devuelvo las diferencias acumuladas
	return acumDif;
}

void ModEvalPAnFilter::addAnalysisEpilog(std::stringstream &ssNetlist) const
{
	// Agrego el final a la netlist
	ssNetlist << "* ANALYSIS\n";
	ssNetlist << ".AC DEC " << pointsPerDec_ << ' '
		<< startFreq_ << ' ' << endFreq_ << '\n';
	ssNetlist << ".END\n";
}

void ModEvalPAnFilter::showIndiv(ostream& os, const TGenome& genome) const
{
	// Obtengo la netlist
	unsigned dummy;
	string netlist;
	getNetlist(genome, netlist, dummy);

	// Muestro la netlist
	os << netlist;
}
