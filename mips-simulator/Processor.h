#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "PipReg.h"
#include <string>
#include <vector> 

	using namepsace std; 
	
	class Processor {
		
		public: 
			Processor();
			
			void Set_Instr_Mem(const vector<string> instr_mem);
			void Tick();
			void IF();
			void ID(PipReg IF_ID);
			void EX(PipReg ID_EX);
			void MEM(PipReg EX_MEM);
			void WB(PipReg MEM_WB);
			void Register_Dump(const vector<int>& v);
		
		private:
			string PC;
			vector<int> Memory;
			vector<int> Register_File;
			vector<int> Instruction_Mem;		
	};
	
	
#endif