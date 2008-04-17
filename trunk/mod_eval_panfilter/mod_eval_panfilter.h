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
// mod_eval_panfilter.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a AFDGP el 8 de abril de 2008
//=============================================================================

#ifndef MOD_EVAL_PANFILTER_H
#define MOD_EVAL_PANFILTER_H

#include "component_namer.h"
#include "component_value_transl.h"
#include "eval_module.h"
#include "individual.h"
#include "population.h"
#include "spice_ac_out.h"
#include "transfer_spec.h"
#include <i_mod_spice.h>
#include <boost/scoped_ptr.hpp>

/// Clase encargada de evaluar a cada individuo de la población de filtros
/// analógicos pasivos
class MODULE_API ModEvalPAnFilter : public EvalModule
{
public:
	// Requeridos por la interfaz
	virtual const std::string& getName() const {return name_;};
	virtual const std::vector<Req>& getReqMods() const {return reqs_;}
	virtual unsigned int getVersion() const {return version_;}
	virtual bool giveConfigData(const std::map<std::string, std::string>& configData);
	virtual bool giveReqMods(const std::vector<boost::shared_ptr<Module> >& reqMods);
	virtual double evaluateGenome(const TGenome& genome) const;
	virtual void showIndiv(std::ostream& os, const TGenome& genome) const;

	// C & D
	ModEvalPAnFilter() {}
	virtual ~ModEvalPAnFilter();

protected:
	/// Obtiene una netlist a partir de un genoma
	void getNetlist(const TGenome& genome, std::string& netlist,
		unsigned& probedNode) const;

	/// Realiza el análisis de la netlist con SPICE y devuelve el puntaje
	double spiceAnalysis(const std::string& netlist,
		unsigned probedNode) const;

	/// Compara los resultados (interpretados) del análisis SPICE con los
	/// buscados
	double compareResults(const SpiceACOut& interpretedResults,
		const std::string& probedNodeName) const;

	/// Agrega un epílogo al final de la netlist
	void addAnalysisEpilog(std::stringstream& ssNetlist) const;

private:
	/// Objeto que encapsula la transferencia deseada
	boost::shared_ptr<TransferSpec> pDesiredTransfer_;

	/// Frecuencia de inicio del barrido
	double startFreq_;

	/// Frecuencia de finalización del barrido
	double endFreq_;

	/// Puntos por década
	unsigned pointsPerDec_;

	/// Cero para la penalidad exponencial por tamaño
	double expPenaltyZero_;

	/// Multiplicador para determinarla penalidad
	double expPenaltyMult_;
	
	/// Embrión que sirve como base para desarrollar el individuo
	boost::scoped_ptr<Individual> pEmbryo_;

	/// Objecto encargado de nombrar los componentes
	ComponentNamer embryoCompNamer_;

	/// Traductor de los valores de los componentes
	boost::scoped_ptr<ComponentValueTransl> pTransl_;

	/// Módulo del SPICE
	boost::shared_ptr<IModSpice> pSpiceModule_;

	/// Nombre del módulo
	static const std::string name_;

	/// Versión del módulo
	static const unsigned int version_;

	/// IDs de los módulos requeridos
	static const std::vector<Req> reqs_;
};

#endif
