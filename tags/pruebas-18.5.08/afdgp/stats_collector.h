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
// stats_collector.h
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 15 de mayo de 2008
//=============================================================================

#ifndef STATS_COLLECTOR_H
#define STATS_COLLECTOR_H

namespace GP
{
	/// Clase base de los recolectores de estadísticas
	class StatsCollector
	{
	public:
		/// Imprime generalidades
		virtual void printGeneralParameters(std::ostream& os) const = 0;

		/// Imprime el encabezado de las estadísticas por generación
		virtual void printStatsByGenHeader(std::ostream& os) const = 0;

		/// Imprime las últimas estadísticas por generación recolectadas
		virtual void printStatsByGenRow(std::ostream& os) const = 0;

		/// Imprime las estadísticas globales del proceso
		virtual void printGlobalStats(std::ostream& os) const = 0;
	};
}

#endif
