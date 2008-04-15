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
// os_dep.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Creado el 8 de abril de 2008
//=============================================================================

#include "os_dep.h"

using namespace Util;

#ifdef WIN32
///////////////////////////// PARA WIN32 //////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/// Extensión de los módulos en Windows
const std::string MODULES_FILE_EXTENSION = ".dll";

/// Tiempo específico a Win32
struct OSDep::OSSpecificTime
{
	FILETIME data_;
};

namespace
{
	/// Para bibliotecas compartidas en Windows, o sea DLLs
	class WinSharedLibrary : public OSDep::ISharedLibrary
	{
		/// Handle
		HMODULE handle_;

	public:
		/// Construye una a partir del path
		WinSharedLibrary(const std::string& path)
		{
			handle_ = ::LoadLibrary(path.c_str());
			if (!handle_)
				throw; // FIXME: Lanzar algo más descriptivo
		}

		/// Destruye una biblioteca compartida
		virtual ~WinSharedLibrary()
		{
			::FreeLibrary(handle_);
		}

		/// Obtiene la dirección de un símbolo
		virtual void* getSymbolAddress(const std::string& symbolName) const
		{
			return ::GetProcAddress(handle_, symbolName.c_str());
		}
	};

	/// Timestamps de Win32
	void timeStamp(OSDep::OSSpecificTime& osst)
	{
		// FIXME: Ignoro kernel time?
		FILETIME dummy;
		::GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, &dummy, 
			&osst.data_);
	}

	/// Obtiene la diferencia entre dos timestamps como un double en segundos
	/// en Win32
	double getTimeStampDiff(const OSDep::OSSpecificTime& ossStart, 
		const OSDep::OSSpecificTime& ossStop)
	{
		__int64 start = static_cast<__int64>(ossStart.data_.dwHighDateTime) |
			ossStart.data_.dwLowDateTime;
		__int64 stop = static_cast<__int64>(ossStop.data_.dwHighDateTime) |
			ossStop.data_.dwLowDateTime;
		return static_cast<double>(stop - start) / 1e7;
	}
}

/// Implementación para Win32 de obtener la biblioteca compartida
boost::shared_ptr<OSDep::ISharedLibrary>
OSDep::ISharedLibrary::makeFromPath(const std::string& path)
{
	using boost::shared_ptr;

	return shared_ptr<ISharedLibrary>(new WinSharedLibrary(path));
}

///////////////////////////// FIN PARA WIN32 //////////////////////////////////
#else

#error OS no soportado

#endif

const std::string& OSDep::getModulesFileExtension()
{
	return MODULES_FILE_EXTENSION;
}

OSDep::ProcessTimer::ProcessTimer(bool autoStart) :
acumTime_(0.0),
running_(false),
pOSSpecTime_(new OSSpecificTime)
{
	if (autoStart)
		start();
}

OSDep::ProcessTimer::~ProcessTimer()
{
}

double OSDep::ProcessTimer::readAcumTime() const
{
	if (!running_)
		return acumTime_;
	else
		return -1.0;
}

void OSDep::ProcessTimer::start()
{
	// Si está corriendo, no hace nada
	if (running_)
		return;

	// Corriendo...
	running_ = true;

	// Marco un timestamp de cuando empecé
	timeStamp(*pOSSpecTime_);
}

void OSDep::ProcessTimer::stop()
{
	// Obtengo un timestamp de este momento
	OSSpecificTime now;
	timeStamp(now);
	
	// Si no está corriendo, no hace nada
	if (!running_)
		return;

	// Acumulo la diferencia como un double en segundos
	acumTime_ += getTimeStampDiff(*pOSSpecTime_, now);
}

void OSDep::ProcessTimer::reset()
{
	running_ = false;
	acumTime_ = 0.0;
}