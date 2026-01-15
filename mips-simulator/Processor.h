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
			void Tick_Debug();
			void IF();
			void ID();
			void EX();
			void MEM();
			void WB();
			
			//helper functions
			string convert_binary(string instruction);
			int get_reg(string reg);
			int get_immediate(string imm);
			int get_dec(string address);
			int twos_comp(string val);
			string get_bin(int num);
			void determine_r_type(string instruction);
			void determine_j_type(string instruction);
			void determine_i_type(string instruction);
			void Register_Dump(const vector<int>& v);	
			
			//instruction functions
			//note- some begin with capitals letters. this is to avoid use of predefined c++ keywords (or, and, etc.) 
			void add(string instruction);
			void And(string instruction);
			void jr(string instruction);
			void Nor(string instruction);
			void Or(string instruction);
			void slt(string instruction);
			void sll(string instruction);
			void srl(string instruction);
			void Sub(string instruction);
			void syscall(string instruction);
			void jal(string instruction);
			void j(string instruction);
			void addi(string instruction);
			void andi(string instruction);
			void beq(string instruction);
			void bne(string instruction);
			void lui(string instruction);
			void lw(string instruction);
			void ori(string instruction);
			void slti(string instruction);
			void sw(string instruction);
				
		
		private:
			string PC;
			vector<int> Memory;
			vector<int> Register_File;
			vector<int> Instruction_Mem;	
			PipReg IF_ID;
			PipReg ID_EX;
			PipReg EX_MEM;
			PipReg MEM_WB;
	};
	
	
#endif