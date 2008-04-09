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
// panf_genome_ops_wrapper.cpp
//-----------------------------------------------------------------------------
// Creado por Mariano M. Chouza | Agregado a NGPD el 6 de abril de 2008
//=============================================================================

#include "panf_genome_iterator.h"
#include "panf_genome_ops_wrapper.h"
#include <panf_genome.h>
#include <utility>
#include <vector>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;
using std::endl;
using std::max;
using std::ostream;
using std::pair;
using std::string;
using std::vector;

/// Namespace anónimo para funciones de utilidad
namespace
{
	// FIXME: HARDCODED

	/// Profundidad máxima
	const size_t MAX_DEPTH = 5;

	/// Profundidad mínima
	const size_t MIN_DEPTH = 2;

	/// Cantidad de cabezas de escritura
	const size_t NUM_WRITING_HEADS = 1;

	/// Máscara que permite extraer el byte bajo
	const int LOBYTE_MASK = (1 << CHAR_BIT) - 1;

	/// Escribe un valor en base al genoma y una posición
	void writeVal(TGenome& genome, unsigned pos, unsigned val)
	{
		for (unsigned i = 0; i < 4; i++)
			genome[pos + i] = (val >> 8 * (3 - i)) & 0xff;
	}

	/// Lee un valor en base al genoma y una posición
	unsigned readVal(const TGenome& genome, unsigned pos)
	{
		unsigned acum = 0;
		for (unsigned i = 0; i < 4; i++)
		{
			acum <<= 8;
			acum |= genome[pos+i];
		}
		return acum;
	}

	/// Imprime un valor en os en base al genoma y una posición
	void printVal(ostream& os, const TGenome& genome, unsigned& pos)
	{
		os << readVal(genome, pos);
		pos += 3; // El otro ciclo ya lo avanza uno
	}

	// Forward
	void continueConstruction(TGenome& genome, EOpCode opCode,
		size_t minDepth, size_t maxDepth);

	/// Elije un operador CT
	EOpCode pickRandomCTOp()
	{
		// Obtengo un entero al azar en el rango [0, 3)
		unsigned randomInt = rand() % 3;

		// De acuerdo a cual es, elijo que devolver
		switch (randomInt)
		{
		case 0:
			return R;
		case 1:
			return L;
		case 2:
			return C;
		default:
			// No es posible
			assert(0);
			return EOC_INVALID;
		}
	}

	/// Elije un operador CC en base a la profundidades mínima y máxima
	/// permitidas
	EOpCode pickRandomCCOp(size_t minDepth, size_t maxDepth)
	{
		// Operadores de los cuales elegir
		// FIXME: No están todos
		static const EOpCode termOps[] = {END, SAFE_CUT};
		static const EOpCode deepOps[] = {TWO_LEAD, FLIP, SERIES, PARALLEL,
											TWO_GROUND};
		static const size_t termOpsSz = sizeof(termOps) / sizeof(EOpCode);
		static const size_t deepOpsSz = sizeof(deepOps) / sizeof(EOpCode);

		// Hay tres posibilidades para [minDepth, maxDepth]:
		// 1) Puede ser [1, 1], en ese caso solo puedo elegir END o SAFE_CUT.
		// 2) Puede ser [1, X] con X > 1, en ese caso puedo elegir
		//    cualquiera.
		// 3) Puede ser [X, Y] con X > 1, en ese caso puedo elgir cualquiera
		//    _menos_ END o SAFE_CUT.

		assert(maxDepth >= minDepth);
		assert(minDepth > 0);

		// Me fijo si puedo elegir a los que no son END ni SAFE_CUT
		bool canChooseDeep = (maxDepth > 1) ? true : false;

		// Me fijo si puedo elegir a END o SAFE_CUT
		bool canChooseTerm = (minDepth == 1) ? true : false;

		assert(canChooseDeep || canChooseTerm);

		// Elijo un entero al azar
		unsigned randomInt = rand();

		// De acuerdo a lo que puede elegir...
		if (canChooseDeep && canChooseTerm)
		{
			randomInt %= (termOpsSz + deepOpsSz);
			if (randomInt >= termOpsSz)
				return deepOps[randomInt - termOpsSz];
			else
				return termOps[randomInt];
		}
		else if (canChooseDeep && !canChooseTerm)
		{
			randomInt %= deepOpsSz;
			return deepOps[randomInt];
		}
		else if (!canChooseDeep && canChooseTerm)
		{
			randomInt %= termOpsSz;
			return termOps[randomInt];
		}
		else
		{
			// No es posible
			assert(0);
			return EOC_INVALID;
		}
	}

	// FIXME: Debería poder configurar el rango
	/// Función que construye un subárbol de tipo valor
	void makeRandomVal(TGenome& genome, size_t minDepth, size_t maxDepth)
	{
		// De acuerdo con el uso exclusivo de enteros perturbables, la
		// profundidad mínima debe ser 1
		if (minDepth != 1)
			// FIXME: Lanzar algo más específico
			throw;

		// Obtengo un entero aleatorio
		unsigned randomInt = rand();

		// Guardo los bytes en el genoma
		for (int i = 0; i < 4; i++)
			genome.push_back(
				static_cast<unsigned char>(
					(randomInt >> (3-i)) & LOBYTE_MASK));

		// Acá termina
	}

	/// Función que construye un subárbol CT (tipo de componente)
	void makeRandomCT(TGenome& genome, size_t minDepth, size_t maxDepth)
	{
		// Si la profundidad mínima es distinta de 1, estamos en un error
		if (minDepth != 1)
			// FIXME: Lanzar algo más específico
			throw;

		// Elijo un operador CT
		EOpCode opCode = pickRandomCTOp();

		// Agrego el operador al genoma
		genome.push_back(opCode);

		// La construcción termina acá
	}
	
	// FIXME: EL manejo de la profundidad es pcoo riguroso
	/// Función recursiva que construye en forma aleatoria un árbol de CC al
	/// final de un genoma
	void makeRandomCC(TGenome& genome, size_t minDepth, size_t maxDepth)
	{
		// Elijo un operador de continuar la construcción en forma aleatoria
		// de acuerdo a la profundidad permitida
		EOpCode opCode = pickRandomCCOp(minDepth, maxDepth);

		// Agrego el operador al genoma
		genome.push_back(opCode);

		// Continúo la contrucción
		continueConstruction(genome, opCode, max<size_t>(minDepth - 1, 1),
			maxDepth - 1);
	}

	/// Continúa la construcción después de haber elegido el operador CC
	/// opCode
	void continueConstruction(TGenome& genome, EOpCode opCode,
		unsigned minDepth, unsigned maxDepth)
	{
		// Decido en base a cual fue la última opción
		switch (opCode)
		{
		case TWO_LEAD:
			// En ese caso, debe seguir el tipo
			makeRandomCT(genome, 1, 1);

			// Luego, el valor
			makeRandomVal(genome, 1, 1);

			// Por último, un árbol CC
			makeRandomCC(genome, minDepth, maxDepth);
			break;

		case FLIP:
			// En este caso sigue un CC
			makeRandomCC(genome, minDepth, maxDepth);
			break;

		case SERIES:
		case TWO_GROUND:
			// En este caso, siguen dos CCs
			makeRandomCC(genome, minDepth, maxDepth);
			makeRandomCC(genome, minDepth, maxDepth);
			break;

		case PARALLEL:
			// En este caso, siguen cuatro CCs
			makeRandomCC(genome, minDepth, maxDepth);
			makeRandomCC(genome, minDepth, maxDepth);
			makeRandomCC(genome, minDepth, maxDepth);
			makeRandomCC(genome, minDepth, maxDepth);
			break;

		case END:
		case SAFE_CUT:
			// En este caso no sigue nada
			break;

		default:
			// Nunca debería llegar
			// FIXME: Manejar NODE, VIA, etc...
			assert(0);
		}
	}

	/// Arma una sección de genoma del tipo pedido
	void makeRandomGenomeSection(TGenome& genome, EOpKind kind,
		size_t minDepth, size_t maxDepth)
	{
		assert(minDepth <= maxDepth && minDepth > 0);
		
		switch (kind)
		{
		case EOK_CC:
			makeRandomCC(genome, minDepth, maxDepth);
			break;
		case EOK_CT:
			makeRandomCT(genome, 1, 1);
			break;
		case EOK_VAL:
			makeRandomVal(genome, 1, 1);
			break;
		default:
			// FIXME: Lanzar algo más específico
			throw;
		}
	}

	/// Reemplaza una sección de un genoma por otra sección del otro
	void replaceGenomeSection(TGenome& genome1, size_t genome1StartPos,
		size_t genome1EndPos, TGenome& genome2, size_t genome2StartPos,
		size_t genome2EndPos)
	{
		// FIXME Tal vez no sea muy óptimo...
		// Elimino el rango original
		genome1.erase(genome1.begin() + genome1StartPos,
			genome1.begin() + genome1EndPos);

		// Inserto el nuevo rango
		genome1.insert(genome1.begin() + genome1StartPos,
			genome2.begin() + genome2StartPos,
			genome2.begin() + genome2EndPos);
	}

	/// Avanza el iterador hasta llegar a otra posición con la misma
	/// profundidad o menos
	void advanceUntilSameOrLessDepth(PANFGenomeIterator& it)
	{
		// Profundidad inicial
		size_t initialDepth = it.getDepth();

		// Avanzo hasta que llegue a la misma o menos
		for (++it; it.getDepth() > initialDepth; ++it);
	}

	/// Intercambia una sección de un genoma por otra sección del otro
	void swapGenomeSections(TGenome& genome1, size_t genome1StartPos,
		size_t genome1EndPos, TGenome& genome2, size_t genome2StartPos,
		size_t genome2EndPos)
	{
		// FIXME: Tal vez no sea muy óptimo...

		// Copio los nuevos rangos
		TGenome genome1Section(genome1.begin() + genome1StartPos,
			genome1.begin() + genome1EndPos);
		TGenome genome2Section(genome2.begin() + genome2StartPos,
			genome2.begin() + genome2EndPos);

		// Elimino los rangos originales
		genome1.erase(genome1.begin() + genome1StartPos,
			genome1.begin() + genome1EndPos);
		genome2.erase(genome2.begin() + genome2StartPos,
			genome2.begin() + genome2EndPos);

		// Inserto los nuevos rangos
		genome1.insert(genome1.begin() + genome1StartPos,
			genome2Section.begin(), genome2Section.end());
		genome2.insert(genome2.begin() + genome2StartPos,
			genome1Section.begin(), genome1Section.end());
	}

}

void PANFGenomeOpsWrapper::load(std::istream &is, bool textualFormat)
{
	// Tengo que cargar, empiezo borrando
	genome_.clear();
	
	// Si es binario, lo cargo como binario
	if (!textualFormat)
	{
		// Obtengo los bytes uno a uno y los inserto en el vector
		char ch;
		while (is.get(ch))
			genome_.push_back(ch);

		// Listo
		return;
	}
	
	// No, lo proceso como texto

	// Cadena que representa una línea
	string line;
	
	// Leo línea a línea
	while (std::getline(is, line))
	{
		// Me fijo que de que se trata
		if (line == "TWO_LEAD")
			genome_.push_back(TWO_LEAD);
		else if (line == "R")
			genome_.push_back(R);
		else if (line == "C")
			genome_.push_back(C);
		else if (line == "L")
			genome_.push_back(L);
		else if (line == "SERIES")
			genome_.push_back(SERIES);
		else if (line == "PARALLEL")
			genome_.push_back(PARALLEL);
		else if (line == "NODE")
			genome_.push_back(NODE);
		else if (line == "VIA")
			genome_.push_back(VIA);
		else if (line == "TWO_GROUND")
			genome_.push_back(TWO_GROUND);
		else if (line == "END")
			genome_.push_back(END);
		else if (line == "SAFE_CUT")
			genome_.push_back(SAFE_CUT);
		else if (line == "FLIP")
			genome_.push_back(FLIP);
		else if (isdigit(line[0]))
		{
			// Si hay un entero es un entero perturbable, que se interpreta
			// por su posición
			unsigned val = lexical_cast<unsigned>(line);
			for (int i = 0; i < 4; i++)
				genome_.push_back(
					static_cast<unsigned char>(val >> (3-i) & LOBYTE_MASK));
		}
		else
			// FIXME: Lanzar algo más descriptivo
			throw;
	}
}

void PANFGenomeOpsWrapper::save(ostream &os, bool textualFormat) const
{
	// Si es binario
	if (!textualFormat)
	{
		// Recorro el genoma guardandolo en la stream
		for (unsigned i = 0; i < genome_.size(); ++i)
			os.put(genome_[i]);

		// Acá termino
		return;
	}

	// FIXME: Puedo pasarme
	// No, lo guardo como texto
	for (unsigned i = 0; i < genome_.size(); ++i)
	{
		switch (genome_[i])
		{
		case TWO_LEAD:
			os << "TWO_LEAD" << endl;
			break;
		case R:
			os << "R" << endl;
			// Después de R viene el valor
			printVal(os, genome_, ++i);
			os << endl;
			break;
		case C:
			os << "C" << endl;
			// Después de C viene el valor
			printVal(os, genome_, ++i);
			os << endl;
			break;
		case L:
			os << "L" << endl;
			// Después de L viene el valor
			printVal(os, genome_, ++i);
			os << endl;
			break;
		case SERIES:
			os << "SERIES" << endl;
			break;
		case PARALLEL:
			os << "PARALLEL" << endl;
			break;
		case NODE:
			os << "NODE" << endl;
			break;
		case VIA:
			os << "VIA" << endl;
			// Después de VIA viene que VIA es...
			os << static_cast<int>(genome_[++i]);
			break;
		case TWO_GROUND:
			os << "TWO_GROUND" << endl;
			break;
		case END:
			os << "END" << endl;
			break;
		case SAFE_CUT:
			os << "SAFE_CUT" << endl;
			break;
		case FLIP:
			os << "FLIP" << endl;
			break;
		default:
			// FIXME: Lanzar algo más específico
			throw;
			break;
		}
	}
}

void PANFGenomeOpsWrapper::makeRandom()
{
	// Para cada cabeza
	for (unsigned i = 0; i < NUM_WRITING_HEADS; i++)
	{
		// Realiza el árbol en forma recursiva
		makeRandomCC(genome_, MIN_DEPTH, MAX_DEPTH);
	}
}

void PANFGenomeOpsWrapper::cross(TGenome& otherGenome)
{
	// Obtengo una posición aleatoria
	size_t pos = rand() % genome_.size();

	// Recorro hasta ahí (para tener la profundidad)
	PANFGenomeIterator it(genome_);
	while (it.getPos() < pos)
		++it;

	// Obtengo la profundidad
	size_t depth = it.getDepth();

	// Recorro con otro iterador hasta llegar a la misma profundidad
	PANFGenomeIterator itEnd(it);
	advanceUntilSameOrLessDepth(itEnd);

	// El rango de iteradores hallado me determina la sección a intercambiar
	// Busco ahora las posiciones de la misma clase en el otro genoma
	std::vector<size_t> posSameKindOG;
	PANFGenomeIterator itOGTmp(otherGenome);
	for (; itOGTmp.getPos() < otherGenome.size(); ++itOGTmp)
	{
		if (itOGTmp.getKind() == it.getKind())
			posSameKindOG.push_back(itOGTmp.getPos());
	}

	// Si no encontré ninguna de la misma clase, salgo...
	if (posSameKindOG.size() == 0)
		return;

	// Dentro de esas, elijo una posición al azar
	size_t posOG = posSameKindOG[rand() % posSameKindOG.size()];

	// Obtengo el correspondiente par de iteradores
	PANFGenomeIterator itOG(otherGenome, posOG);
	PANFGenomeIterator itOGEnd(itOG);
	advanceUntilSameOrLessDepth(itOGEnd);

	// Realizo el intercambio
	swapGenomeSections(genome_, it.getPos(), itEnd.getPos(), otherGenome,
		itOG.getPos(), itOGEnd.getPos());
}

void PANFGenomeOpsWrapper::cross(PANFGenomeOpsWrapper &otherGenomeWrapper)
{
	// Utiliza al otro método
	cross(otherGenomeWrapper.genome_);
}

void PANFGenomeOpsWrapper::mutate()
{
	// Obtengo una posición aleatoria
	size_t pos = rand() % genome_.size();

	// Recorro hasta ahí (para tener la profundidad)
	PANFGenomeIterator it(genome_);
	while (it.getPos() < pos)
		++it;

	// Obtengo la profundidad
	size_t depth = it.getDepth();

	// Recorro con otro iterador hasta llegar a la misma profundidad
	PANFGenomeIterator itEnd(it);
	advanceUntilSameOrLessDepth(itEnd);

	// Obtengo las profundidades que correspondan
	size_t minDepth = (depth > MIN_DEPTH) ? 1 : MIN_DEPTH - depth + 1;
	size_t maxDepth = (depth > MAX_DEPTH) ? 1 : MAX_DEPTH - depth + 1; 
	assert(minDepth <= maxDepth && minDepth > 0);

	// Armo un nuevo genoma aleatorio con la profundidad adecuada
	TGenome randomGenome;
	makeRandomGenomeSection(randomGenome, it.getKind(), minDepth, maxDepth);

	// Realizo el reemplazo
	replaceGenomeSection(genome_, it.getPos(), itEnd.getPos(), randomGenome,
		0, randomGenome.size());
}

void PANFGenomeOpsWrapper::altOp()
{
	// Busco todos los lugares con valores
	vector<size_t> valuePos;
	PANFGenomeIterator it(genome_);
	for (; it.getPos() < genome_.size(); ++it)
	{
		if (it.getKind() == EOK_VAL)
			valuePos.push_back(it.getPos());
	}

	// Si no hay posiciones, salgo
	if (valuePos.empty())
		return;

	// Elijo una posición en forma aleatoria
	size_t alteredValPos = valuePos[rand() % valuePos.size()];

	// FIXME: Hacer configurable los parámetros de la distribución y, tal vez,
	// dar la posibilidad de elegir otras funciones de densidad de
	// probabilidad
	// FIXME: Aunque sea hacer normal!!
	double oldVal = readVal(genome_, static_cast<unsigned>(alteredValPos));

	// Obtengo el nuevo valor
	double newVal = oldVal +
		(2.0 - 4.0 * (static_cast<double>(rand()) / RAND_MAX));

	// Guardo el nuevo valor
	writeVal(genome_, alteredValPos, static_cast<unsigned>(newVal));
}