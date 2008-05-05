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

	cout << "Módulo de operaciones: " << pOM->getName() << "\n";

	cout << "Generando población...\n";
	TPop pop = generatePopulation(*pOM);
	cout << "Población generada.\n";

	cout << "Serializando la población...\n";
	ofstream out("pop.bin", std::ofstream::binary);
	PopSerializer::serialize(out, pop);
	out.close();
	cout << "Población serializada.\n";

	cout << "Deserializando la población...\n";
	TPop pop2;
	std::ifstream in("pop.bin", std::ifstream::binary);
	PopSerializer::deserialize(pop2, in);
	in.close();
	cout << "Población deserializada.\n";

	cout << "Comparando poblaciones...\n";
	if (pop == pop2)
		cout << "Las poblaciones son idénticas!\n";
	else
		cout << "Las poblaciones no son iguales!!\n";

	return 0;
}

#endif
