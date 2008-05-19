# 
# Copyright (c) 2008, Mariano M. Chouza
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#    * Redistributions of source code must retain the above copyright notice, 
#      this list of conditions and the following disclaimer.
# 
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
#    * The names of the contributors may not be used to endorse or promote
#      products derived from this software without specific prior written
#      permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 

################################################################################
# filters.py
#-------------------------------------------------------------------------------
# Creado por Mariano M. Chouza | Creado el 3 de mayo de 2008
################################################################################

class FatalError(Exception):
    """Clase de los errores que termiann la ejecución del programa."""

    def __init__(self, message):
        """Construye este error en base a un mensaje."""
        Exception.__init__(self, message)

class Error(Exception):
    """Clase base de los errores que detectan estos filtros."""
    
    def __init__(self, message):
        """Contruye un error en base a un mensaje."""
        self.message = message

    def __unicode__(self):
        """Muestra el mensaje."""
        return self.message

class Filter(object):
    """Clase base de los filtros. No hace nada..."""
    
    def __call__(self, line):
        """Alimenta el filtro con una línea."""
        pass

    def reset(self):
        """Vuelve el filtro a su estado original."""
        pass

class JoinedFilter(Filter):
    """Se ocupa de evaluar múltiples filtros sobre una línea."""

    def __init__(self, lineFilters = (), fileFilters = ()):
        """Inicializa un filtro que combina varios otros filtros.

        Atributos:
            lineFilters -- filtros aplicados línea a línea
            fileFilters -- filtros aplicados al archivo como un conjunto
        """
        self.lineFilters = list(lineFilters)
        self.fileFilters = list(fileFilters)
        for fileFilter in self.fileFilters:
            fileFilter.reset()

    def __call__(self, line):
        """Aplica todos los filtros disponible a la línea 'line'."""

        # Aplica todos los filtros de línea
        for lineFilter in self.lineFilters:
            # Aplica el filtro de línea
            lineFilter(line)

        # Aplica todos los filtros de archivo
        for fileFilter in self.fileFilters:
            # Aplica el filtro de archivo
            fileFilter(line)

    def reset(self):
        """Reinicializa el filtro combinado, es decir todos los filtros
        de archivo."""
        for fileFilter in self.fileFilters:
            fileFilter.reset()
           
class EightyColsFilter(Filter):
    """Se ocupa de verificar que los archivos no pasen de 80 columnas."""

    TAB_SIZE = 4 # Toma cada TAB como 4 espacios

    def __call__(self, line):
        """Verifica que la línea no pase de las 80 columnas."""
        line = line.replace('\t', ' ' * self.TAB_SIZE).rstrip('\r\n')
        if len(line) > 80:
            raise Error(u'Tiene más de 80 columnas.')

class CheckLicenseFilter(Filter):
    """Se ocupa de verificar que los archivos tengan la licencia
    correspondiente."""

    def __init__(self, license, commenter):
        """Inicializa este filtro a partir de una licencia y una función capaz
        de comentar a la licencia al lenguaje apropiado."""
        self.disabled = False
        self.license = commenter(license).split('\n')
        self.lineNumber = 0

    def __call__(self, line):
        """Compara esta línea con la línea adecuada de la licencia."""
        # Si está deshabilitado, no sigo
        if self.disabled:
            return

        # Si terminó con la licencia, deshabilito el filtro
        if self.lineNumber >= len(self.license):
            self.disable()
            return

        # Compara la línea del archivo con la línea de la licencia
        if line.rstrip() != self.license[self.lineNumber].rstrip():
            self.disable()
            raise Error(u'El archivo no tiene una licencia correcta.')

        # Siguiente línea
        self.lineNumber += 1

    def disable(self):
        """Deshabilita el filtro."""
        self.disabled = True

    def reset(self):
        """Vuelve a cero el filtro."""
        self.lineNumber = 0
        self.disabled = False

class CFilter(JoinedFilter):
    """Se ocupa de verificar un archivo C."""

    def __init__(self, licenseTxt = ''):
        """Construye el filtro de verificación de un archivo C."""

        # Comenta una serie de líneas en C
        def commenter(txt):
            lines = map(lambda s: '** ' + s, txt.split('\n'))
            lines.insert(0, '/* ')
            lines.append('*/ ')
            return '\n'.join(lines) + '\n'

        # Inicializa la clase padre
        JoinedFilter.__init__(self,
            (
                EightyColsFilter(),),
            (
                CheckLicenseFilter(licenseTxt, commenter),))            

class CppFilter(JoinedFilter):
    """Se ocupa de verificar un archivo C++."""
    
    def __init__(self, licenseTxt = ''):
        """Construye el filtro de verificación de un archivo C++."""

        # Comenta una serie de líneas en C++
        def commenter(txt):
            return '\n'.join(map(lambda s: '// ' + s, txt.split('\n'))) + '\n'

        # Inicializa la clase padre
        JoinedFilter.__init__(self,
            (
                EightyColsFilter(),),
            (
                CheckLicenseFilter(licenseTxt, commenter),))   

class PyFilter(JoinedFilter):
    """Se ocupa de verificar un archivo de Python."""
    
    def __init__(self, licenseTxt = ''):
        """Construye el filtro de verificación de un archivo Python."""
        
        # Comenta una serie de líneas en Python
        def commenter(txt):
            return '\n'.join(map(lambda s: '# ' + s, txt.split('\n'))) + '\n'

        # Inicializa la clase padre
        JoinedFilter.__init__(self,
            (
                EightyColsFilter(),),
            (
                CheckLicenseFilter(licenseTxt, commenter),)) 
