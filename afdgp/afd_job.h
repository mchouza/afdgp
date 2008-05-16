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
// afd_job.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 11 de abril de 2008
//=============================================================================

#ifndef JOB_H
#define JOB_H

#include "core_fwd.h"
#include "gp_fwd.h"
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace Core
{
	/// Representa un trabajo de diseño de un filtro analógico
	class AFDJob
	{
		/// Configuración del trabajo
		const Config& config_;

		/// Evolver para realizar el trabajo
		boost::scoped_ptr<GP::Evolver> pEvolver_;

		/// Nombre del trabajo
		std::string filename_;

		/// ID del trabajo
		std::string id_;

		/// Número de ejecuciones
		unsigned numberOfRuns_;

		/// Número de generaciones
		unsigned numberOfGens_;

		/// Número de ejecución
		unsigned runNum_;

		/// Número de generación
		unsigned genNum_;

		/// Stream para mostrar información estadística
		std::ostream& statsStream_;

		/// Carga los datos de una ejecución anterior si es posible
		void resumeIfPossible();

	public:
		/// Lo construye en base a una configuración base y un nombre de 
		/// archivo
		AFDJob(const ModuleLibrary& lib, const Config& baseConfig, 
			const std::string& filename, std::ostream& statsStream);

		/// Destructor
		virtual ~AFDJob();

		/// Guarda el estado de avance del trabajo
		void makeCheckPoint() const;

		/// Avanza un paso que debe ejecutarse "atómicamente" en el trabajo.
		/// Devuelve 'false' para indicar que terminó de trabajar
		bool step();

		/// Imprime resultados parciales o totales
		void printResults();
	};
}

#endif
