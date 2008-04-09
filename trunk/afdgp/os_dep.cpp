#include "os_dep.h"

using namespace Util;

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/// Extensi�n de los m�dulos en Windows
const std::string MODULES_FILE_EXTENSION = ".dll";

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
				throw; // FIXME: Lanzar algo m�s descriptivo
		}

		/// Destruye una biblioteca compartida
		virtual ~WinSharedLibrary()
		{
			::FreeLibrary(handle_);
		}

		/// Obtiene la direcci�n de un s�mbolo
		virtual void* getSymbolAddress(const std::string& symbolName) const
		{
			return ::GetProcAddress(handle_, symbolName.c_str());
		}
	};
}

/// Implementaci�n para Win32 de obtener la biblioteca compartida
boost::shared_ptr<OSDep::ISharedLibrary>
OSDep::ISharedLibrary::makeFromPath(const std::string& path)
{
	using boost::shared_ptr;

	return shared_ptr<ISharedLibrary>(new WinSharedLibrary(path));
}

#else

#error OS no soportado

#endif

const std::string& OSDep::getModulesFileExtension()
{
	return MODULES_FILE_EXTENSION;
}