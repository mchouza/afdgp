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

#=============================================================================
# check_file_standards.py
#-----------------------------------------------------------------------------
# Creado por Mariano M. Chouza | Creado el 9 de abril de 2008
#=============================================================================

# FIXME: Reformular estructura para el procesamiento por línea y los warnings

import os
import sys

# La cantidad de espacios que se considera que vale un TAB
TAB_SIZE = 4

# Cantidad de advertencias antes de suprimir
MAX_WARNING_COUNT = 3

# Tamaño de línea máximo
MAX_LINE_SIZE = 80

# Da un aviso
def warn(msg, wc):
    
    # Si no se "cansó" de dar avisos, lo muestro
    if wc <= MAX_WARNING_COUNT:
        print msg

    # Si está en el límite, aviso
    if wc == MAX_WARNING_COUNT:
        print '(Posteriores advertencias serán omitidas.)'


# Reviso un archivo fuente en general
def checkSrcFile(fileName):

    # Cantidad de avisos dados
    warningCount = 0

    # Contador de líneas
    lineNumber = 1

    # Para todas las líneas
    for line in file(fileName):

        # Avanza el número de línea
        lineNumber += 1

        # Reemplazo los tabs por TAB_SIZE espacios
        line = line.replace('\t', ' ' * TAB_SIZE)

        # Veo que la longitud sea menor o igual a lo establecido
        if len(line) > MAX_LINE_SIZE:
            warn('La línea %d tiene más de %d caracteres.' %
                 (lineNumber, MAX_LINE_SIZE),
                 warningCount)
            warningCount += 1

# Reviso un archivo fuente C++
def checkCppSrcFile(fileName):

    # Es un archivo de código fuente, así que lo reviso primero como tal
    checkSrcFile(fileName)

# Reviso un archivo header de C++
def checkCppHdrFile(fileName):

    # Es un archivo de código fuente, así que lo reviso primero como tal
    checkSrcFile(fileName)

# Extensiones a procesar y con qué hacerlo
fileTypeDispatch =\
{
    'cpp': checkCppSrcFile,
    'h':   checkCppHdrFile
}

# Revisa un directorio
def checkIndivDir(args, dirName, files):
    
    # Muestro donde estoy
    print 'Revisando directorio: %s' % dirName

    # Recorro los archivos
    for fileName in files:

        # Obtengo la extensión
        ext = fileName.split('.')[-1]

        # Si no está en la lista, sigo con otro
        if ext not in fileTypeDispatch:
            continue

        # Muestro el nombre de archivo
        print '\t%s' % fileName

        # Lo proceso
        fileTypeDispatch[ext](os.path.join(dirName, fileName))

# Revisa un directorio en forma recursiva hasta el fondo
def checkBaseDir(baseDir, recLevel = 0):
    
    # Muestro la base
    print 'Directorio base: %s' % baseDir

    # Hago un walk del árbolde directorios
    os.path.walk(baseDir, checkIndivDir, None)

# Main
def main(argv = []):
    
    # Reviso la cantidad de parámetros
    if len(argv) > 2:
        # Demasiados, salgo con error
        print >>sys.stderr, 'Cantidad incorrecta de parámetros.\n'
        return 1
    elif len(argv) == 2:
        # Dir de arranque pasado como parámetro
        baseDir = argv[1]
    else:
        # Tomo el directorio actual como el de arranque
        baseDir = '.'

    # Reviso el directorio
    checkBaseDir(baseDir)

    # OK
    return 0

# Entry point
if __name__ == '__main__':
    sys.exit(main(sys.argv))
