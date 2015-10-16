//
// Plist.cpp: Demonstrates use of CTaskManager class
//

#include "ModuleInstance.h"


int main(int argc, char* argv[])
{
	CTaskManager           taskManager;
	CExeModuleInstance     *pProcess;
	CModuleInstance        *pModule;

	// Retrieves information about processes and modules.  
	// The taskManager dynamically decides whether to use ToolHelp library or PSAPI
	taskManager.Populate();

	FILE* outfile = fopen("C:\\procs.txt","wt");
	// Enumerates all processes
	for (unsigned i = 0; i < taskManager.GetProcessCount(); i++)
	{
		pProcess = taskManager.GetProcessByIndex(i);
		fprintf(outfile,"Process %s pid=%d\n", pProcess->Get_Name(), pProcess->Get_ProcessId());

		// Enumerates all modules loaded by (pProcess) process
		for (unsigned j = 0; j < pProcess->GetModuleCount(); j++)
		{
			pModule = pProcess->GetModuleByIndex(j);
			fprintf(outfile,"\t %s Handle=%.8x\n", pModule->GetBaseName(), pModule->Get_Module());
		} // for
	} // for
	
	fclose(outfile);
	return 0;
}
