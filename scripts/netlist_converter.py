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
# netlist_to_bin.py
#-------------------------------------------------------------------------------
# Convierte netlists a un formato binario propio
#-------------------------------------------------------------------------------
# Empezado el 19-4-08
################################################################################

import decimal
import re
import struct
from math import floor, log10

# Número mágico para el formato binario
MAGIC_NUMBER = '\xe7\x49\xeb\x32'

# Sufijos para los valores
SUFFIXES = {'G':1e9, 'MEG':1e6, 'K':1e3, 'M':1e-3, 'U':1e-6, 'N':1e-9,
            'P':1e-12}            

class Error(Exception):
    """Clase base de los errores en este programa."""
    pass

class BinFormatError(Error):
    """Indica un error en el formato del archivo binario."""
    def __str__(self):
        return 'Error en el formato del archivo binario.'

class MySyntaxError(Error):
    """Indica un error de sintaxis.

    Atributos:
        line -- línea donde ocurrió el error
        msg -- mensaje adicional
    """
    
    def __init__(self, line, msg = None):
        self.line = line
        self.msg = msg

    def __str__(self):
        ret = 'Error de sintaxis en la línea %d' % self.line
        if self.msg == None:
            return ret + '.'
        else:
            return ret + ': %s' % self.msg

class RepeatedCompError(Error):
    """Indica un componente repetido.

    Atributos:
        compName -- nombre del componente ya agregado
    """

    def __init__(self, name):
        self.compName = name

    def __str__(self):
        return 'El componente "%s" ya fue agregado anteriormente.' %\
               self.compName

class CompNotFoundError(Error):
    """Indica un componente no encontrado.

    Atributos:
        compName -- nombre del componente que no se encontró
    """

    def __init__(self, name):
        self.compName = name

    def __str__(self):
        return 'El componente "%s" no pudo ser encontrado.' % self.compName

class OutOfRangeError(Error):
    """Indica un número con una valor fuera del rango permitido.

    Atributos:
        min -- mínimo valor permitido
        max -- máximo valor permitido
        val -- valor encontrado
    """

    def __init__(self, min, max, val):
        self.min = min
        self.max = max
        self.val = val

    def __str__(self):
        return 'El valor %e se encuentra fuera del rango permitido: '\
               '[%e, %e].' % (self.val, self.min, self.max)

def floatToSuffixedString(number):
    """Transforma un número en una string con el sufijo adecuado.

    Parámetros:
        number -- número a convertir
    """

    # Reviso que el número esté en rango
    minVal = min(SUFFIXES.values())
    maxVal = 1000.0 * max(SUFFIXES.values())
    if number > maxVal or number < minVal:
        raise OutOfRangeError(minVal, maxVal, number)

    # Para todos los sufijos...
    for suffixItem in SUFFIXES.iteritems():
        # ...me fijo si es apropiado.
        if suffixItem[1] <= number:
            # Si, uso este y salgo del loop
            suffix = suffixItem[0]
            break

    # Devuelvo el string
    return '%f%s' % (number / SUFFIXES[suffix], suffix)

class Netlist(object):
    """Representa a la netlist.

    Atributos:
        title -- nombre de la netlist
        edgeDict -- componentes en la netlist
    """

    def __init__(self, title, edgeDict = None):
        self.title = title
        if edgeDict != None:
            self.edgeDict = edgeDict
        else:
            self.edgeDict = {}

    def __iter__(self):
        for comp in self.edgeDict.iteritems():
            yield comp[0], comp[1][0], comp[1][1], comp[1][2]

    def __len__(self):
        return len(self.edgeDict)

    def __str__(self):
        acum = '%s\n' % self.title
        for comp in self:
            acum += '%s %d %d %s\n' % (comp[0], comp[1], comp[2],
                                       floatToSuffixedString(comp[3]))
        return acum

    def addComponent(self, name, start, end, value):
        """Agrega un componente.

        Atributos:
            name -- nombre del componente a agregar
            start -- nodo al que se conecta el terminal '+'
            end -- nodo al que se conecta el terminal '-'
            value -- valor del componente
        """

        # Me fijo si ya estaba agregado
        if name in self.edgeDict:
            # Lanzo un error
            raise RepeatedCompError(name)

        # Lo agrego
        self.edgeDict[name] = [start, end, value]

    def removeComponent(self, name):
        """Elimina un componente.

        Atributos:
            name -- nombre del componente a eliminar
        """

        # Me fijo si no está
        if name not in self.edgeDict:
            raise CompNotFoundError(name)

        # Lo borro
        del self.edgeDict[name]

    def check(self):
        """Revisa que la netlist sea válida."""

        # Defino la función de acumulación
        def acumAdj(d, c):
            d.setdefault(c[0], set()).add(c[1])
            d.setdefault(c[1], set()).add(c[0])
            return d

        # Obtiene el diccionario de adyacencia
        adjDict = reduce(acumAdj,
                         self.edgeDict.itervalues(),
                         {})

        # Obtiene el diccionario de nodos cubiertos
        coveredNodesDict = dict.fromkeys(adjDict, False)

        # Define la función recursiva para marcar los nodos alcanzados
        def recCheck(node):
            if coveredNodesDict[node]:
                return
            else:
                coveredNodesDict[node] = True
                for adjNode in adjDict[node]:
                    recCheck(adjNode)

        # La aplico
        recCheck(coveredNodesDict.keys()[0])

        # Me fijo si está todos cubiertos
        allCovered = reduce(lambda a, b: a and b,
                            coveredNodesDict.itervalues(),
                            True)

        # FIXME: Considerar otras causas de invalidez
        # Devuelvo si es válida
        return allCovered
            
def loadLine(line, netlist, lineNum):
    """Carga una línea de la netlist.

    Parámetros:
        line -- línea a cargar
        netlist -- netlist donde cargar los datos
        lineNum -- número de la línea a cargar
    """
    
    # Expresión regular para revisar las líneas de la netlist
    reNormalLine = re.compile(r'^(?P<comp>\w+)\s+'
                              '(?P<start>\d+)\s+'
                              '(?P<end>\d+)\s+'
                              '(?P<baseVal>\d+(\.\d+)?)'
                              '(?P<suffix>[A-Z]*)$')

    # Convierto la línea a mayúsculas
    line = line.upper()

    # Parseo la línea con la expresión regular
    match = reNormalLine.match(line)

    # Si falló, es un error
    if match == None:
        raise MySyntaxError(lineNum)

    # Obtengo el valor del componente
    compValue = float(match.group('baseVal'))
    for suffix in SUFFIXES.iterkeys():
        if match.group('suffix').startswith(suffix):
            compValue *= SUFFIXES[suffix]
            break

    # Agrego el componente
    netlist.addComponent(match.group('comp'),
                         int(match.group('start')),
                         int(match.group('end')),
                         compValue)

def loadNetlistFromTextFile(filename):
    """Crea una netlist a partir de un archivo de texto.

    Parámetros:
        filename -- nombre del archivo de texto
    """
    
    # Abro el archivo de texto
    textFile = open(filename, 'r')

    # Contador de líneas
    lineCount = 0

    # Recorro las líneas del archivo de texto
    for line in textFile:

        # Empiezo en la línea 1
        lineCount += 1

        # La primera línea es el encabezado
        if lineCount == 1:
            # Creo la netlist a grabar como binario
            netlist = Netlist(line.strip())
            continue

        # Las que siguen construyen el grafo
        loadLine(line, netlist, lineCount)

    # Ya terminé con el archivo de texto
    textFile.close()

    # Devuelvo la netlist
    return netlist

def loadNetlistFromBinaryFile(filename):
    """Crea una netlist a partir de un archivo binario.
    
    Parámetros:
        filename -- nombre del archivo de texto
    """

    # Abro el archivo binario
    binFile = open(filename, 'rt')

    # Reviso el número mágico
    if binFile.read(4) != MAGIC_NUMBER:
        raise BinFormatError()

    # Obtengo la cantidad de componentes
    cantComp, = struct.unpack('L', binFile.read(4))

    # Obtengo los componentes
    components = []
    for i in range(cantComp):
        components.append(struct.unpack('cLLf',
                                        binFile.read(struct.calcsize('cLLf'))))

    # Obtengo las strings
    strings = binFile.read().split('\x00')[:-1]

    # Compruebo que haya la cantidad correcta de strings
    if len(strings) != cantComp + 1:
        raise BinFormatError()

    # Armo la netlist
    netlist = Netlist(strings[0])
    for i in range(cantComp):
        netlist.addComponent(components[i][0] + strings[i+1],
                             components[i][1],
                             components[i][2],
                             components[i][3])

    # Cierro el archivo binario
    binFile.close()

    # Devuelvo la netlist
    return netlist

def saveNetlistToTextFile(netlist, filename):
    """Guarda la netlist en formato de texto.

    Parámetros:
        filename -- nombre del archivo de texto donde guardarla
    """

    # Abro el archivo de texto
    textFile = open(filename, 'wt')

    # Lo grabo en el archivo
    print >>textFile, netlist,

    # Cierro el archivo de texto
    textFile.close()

def saveNetlistToBinaryFile(netlist, filename):
    """Guarda la netlist en formato binario.

    Parámetros:
        filename -- nombre del archivo binario donde guardarla
    """
    
    # Abro el archivo binario
    binFile = open(filename, 'w')

    # Escribo el número mágico
    binFile.write(MAGIC_NUMBER)

    # Escribola cantidad de componentes
    binFile.write(struct.pack('L', len(netlist)))

    # Para cada componente...
    for comp in netlist:
        # Lo escribo
        binFile.write(struct.pack('cLLf',
                                  comp[0][0],
                                  comp[1],
                                  comp[2],
                                  comp[3]))

    # Escribo la tabla de strings:
    
    # Primero el nombre de la netlist, ...
    binFile.write(netlist.title + '\x00')

    # ...luego el resto de los nombres
    for comp in netlist:
        binFile.write(comp[0][1:] + '\x00')
    
    # Cierro el archivo binario
    binFile.close()
    
def main():
    """Entry point.

    Parámetros:
        no tiene
    """

    # Creo la netlist desde el archivo de texto 'data.txt'
    netlist = loadNetlistFromTextFile('data.txt')

    # Guardo la netlist en el archivo binario 'test.bin'
    saveNetlistToBinaryFile(netlist, 'test.bin')

    # Compruebo la netlist
    if netlist.check():
        print 'Es válida.'
    else:
        print 'No es válida.'

    # Cargo una netlist desde el archivo binario
    netlist2 = loadNetlistFromBinaryFile('test.bin')

    # Guardo la netlist en un archivo nuevo de texto
    saveNetlistToTextFile(netlist2, 'test.txt')

# Si arranco este módulo
if __name__ == '__main__':
    # Atrapo lo que pueda venir...
    try:
        # Voy al "entry point"
        main()
        
    except Error, e:
        # Imprimo el mensaje y salgo con error
        print e
        exit(1)

    # Salgo OK
    exit(0)
