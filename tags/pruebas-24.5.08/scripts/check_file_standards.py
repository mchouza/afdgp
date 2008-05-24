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
# check_file_standards.py
#-------------------------------------------------------------------------------
# Creado por Mariano M. Chouza | Creado el 9 de abril de 2008
################################################################################

import codecs
import os
import sys
from filters import CFilter, CppFilter, PyFilter, Error

# Licencia
licenseTxt = """
Copyright (c) 2008, Mariano M. Chouza
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.

   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

   * The names of the contributors may not be used to endorse or promote
     products derived from this software without specific prior written
     permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
"""

# Sintaxis
syntaxTxt = """
NO CORRESPONDE A LA SINTAXIS ACTUAL
check_file_standards <dir1> ... <dirN> -<sdir1> ... -<sdirM>'

Revisa todos los archivos cuyos tipos tenga registrados para verificar
el cumplimiento de un cierto número de "standards". La exploración en
busca de estos archivos se realiza en forma recursiva a partir de los
directorios pasados como parámetros (<dir1>, <dir2>, ..., <dirN> en el
ejemplo. Si se le antepone un guión ('-'), el directorio será "suprimido"
de la exploración recursiva.
"""

# Despacho
fileTypeDispatch =\
{
    'cpp': CppFilter(licenseTxt),
    'c': CFilter(licenseTxt),
    'h': CppFilter(licenseTxt),
    'py': PyFilter(licenseTxt)
}

# Tipo de encoding
encoding =\
{
    'cpp': 'windows-1252',
    'c': 'windows-1252',
    'h': 'windows-1252',
    'py': 'utf_8_sig'
}

# Imprime todo por defecto
silent = False

class InvalidOption(Exception):
    """Indica una opción no válida."""

    def __init__(self, optionName):
        """Inicializa la excepción con el nombre de la opción."""
        self.optionName = optionName

    def __str__(self):
        """Muestra el error como una cadena de texto."""
        return 'La opción ' + self.optionName + ' no pudo ser reconocida.'

# Se fija si un directorio está suprimido
def isSuppressed(suppressions, dirName):

    # Si está en la lista, está suprimido
    if dirName in suppressions:
        return True

    # Si tiene como prefijo uno de la lista también
    for supDir in suppressions:
        if dirName.startswith(supDir + os.sep):
            return True

    return False

# Procesa un archivo
def processFile(filePath, extension):

    # Si es silencioso, tiene que imprimri el warning
    printWarning = silent

    # Abre el archivo
    file2Proc = codecs.open(filePath, 'r', encoding[extension])

    # Obtengo el filtro
    myFilter = fileTypeDispatch[extension]

    # Lo reseteo
    myFilter.reset()

    # Lo aplico a todas las líneas
    lineNumber = 0
    for line in file2Proc:
        lineNumber += 1
        try:
            myFilter(line)
        except Error, e:
            if printWarning:
                printWarning = False
                print u'En archivo %s:' % filePath
            print u'\tLínea %d: ' % lineNumber, unicode(e)

    # Cierro el archivo
    file2Proc.close()

# Revisa un directorio
def checkIndivDir(supressions, dirName, files):

    # FIXME: EXTREMADAMENTE INEFICIENTE, HAGO RECURSIÓN SABIENDO QUE TENGO
    # QUE CORTAR!!
    # Si está suprimido, abandono
    if isSuppressed(supressions, dirName):
        return
    
    # Muestro donde estoy
    if not silent:
        print 'Revisando directorio: %s' % dirName

    # Recorro los archivos
    for fileName in files:

        # Obtengo la extensión
        ext = fileName.split('.')[-1]

        # Si no está en la lista, sigo con otro
        if ext not in fileTypeDispatch:
            continue

        # Muestro el nombre de archivo
        if not silent:
            print '\t%s' % fileName

        # Lo proceso
        processFile(os.path.join(dirName, fileName), ext)

# Revisa un directorio en forma recursiva hasta el fondo
def recCheckDir(baseDir, suppressions):
    
    # Muestro la base
    if not silent:
        print 'Directorio base: %s' % baseDir

    # Hago un walk del árbol de directorios
    os.path.walk(baseDir, checkIndivDir, suppressions)

# Muestra la sintaxis
def printSyntax():

    # Imprimo la sintaxis
    print syntaxTxt

# Procesa el cuerpo de los argumentos
def processArgBody(argBody):

    # Divido por ';'
    return argBody.split(';')

# Main
def main(argv = []):
    
    # Directorios a revisar
    dirs2Check = []

    # Suppressions (lugares en donde no reviso)
    suppressions = []

    # Si no tengo parámetros, indico la sintaxis y salgo con error
    if len(argv) < 2:
        printSyntax()
        return 1

    # Reviso los parámetros
    for arg in argv[1:]:
        
        # Me fijo si es una opción
        if arg[0] == '/' or arg[0] == '-':
            # Es una opción, me fijo cuál es
            if arg[1:].find('=') != -1:
                argName, argBody = arg[1:].split('=', 1)
            else:
                argName, argBody =  arg[1:], ''
            if argName == 'silent':
                global silent
                silent = True
            elif argName == 'inspect':
                dirs2Check.extend(processArgBody(argBody))
            elif argName == 'dontInspect':
                suppressions.extend(processArgBody(argBody))
            else:
                raise InvalidOption(arg)
        else:
            # No es una opción, así que es inválida
            raise InvalidOption(arg)

    # Pongo a todos los paths en una forma canónica, así puedo identificar a
    # las suppressions
    dirs2Check = map(os.path.abspath, dirs2Check)
    suppressions = map(os.path.abspath, suppressions)

    # Reviso a cada directorio en forma recursiva, indicando las supressions
    for d2c in dirs2Check:
        recCheckDir(d2c, suppressions)  

    # OK
    return 0

# Entry point
if __name__ == '__main__':
    try:
        sys.exit(main(sys.argv))
    except Exception, e:
        print e
        
