
__declspec(dllexport) int Export(void)
{
	ProcessNetList("circ.cir", "rawfile2.txt");
	return ProcessNetList("circ.cir", "rawfile.txt");
}
