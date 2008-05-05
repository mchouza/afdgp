#include "common.h"
#include "module_library.h"
#include "ops_module.h"
#include "pop_serializer.h"
#include <fstream>
#include <iostream>

namespace
{
	const size_t NUM_INDIV = 100000;
	
	TPop generatePopulation(const OpsModule& opsMod)
	{
		TPop ret(NUM_INDIV);
		for (size_t i = 0; i < ret.size(); i++)
			opsMod.randomInit(ret[i]);
		return ret;
	}
}

#ifdef TEST_POP_SERIALIZER

int main()
{
	using boost::shared_dynamic_cast;
	using boost::shared_ptr;
	using std::cout;
	using std::map;
	using std::ofstream;
	using std::string;
	using Util::PopSerializer;

	time_t startTime, endTime;

#ifdef NDEBUG
	Core::ModuleLibrary lib("../release");
#else
	Core::ModuleLibrary lib("../debug");
#endif
	lib.dump(cout);

	shared_ptr<OpsModule> pOM = shared_dynamic_cast<OpsModule, Module>(
		lib.getModuleByName("GenOpsPassiveAnalogFilter"));

	cout << "M�dulo de operaciones: " << pOM->getName() << "\n";

	cout << "Generando poblaci�n...\n";
	TPop pop = generatePopulation(*pOM);
	cout << "Poblaci�n generada.\n";

	cout << "Serializando la poblaci�n...\n";
	ofstream out("pop.bin", std::ofstream::binary);
	PopSerializer::serialize(out, pop);
	out.close();
	cout << "Poblaci�n serializada.\n";

	cout << "Deserializando la poblaci�n...\n";
	TPop pop2;
	std::ifstream in("pop.bin", std::ifstream::binary);
	PopSerializer::deserialize(pop2, in);
	in.close();
	cout << "Poblaci�n deserializada.\n";

	cout << "Comparando poblaciones...\n";
	if (pop == pop2)
		cout << "Las poblaciones son id�nticas!\n";
	else
		cout << "Las poblaciones no son iguales!!\n";

	return 0;
}

#endif
