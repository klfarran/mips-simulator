#ifndef PIPREG_H
#define PIPREG_H
#include <string>
#include <cstdint>

	using namespace std;

	class PipReg {
		
		public: 
			PipReg();
			void moveto(PipReg dest);

			string get_instruction();
			uint32_t get_PC();
			int64_t get_ALU_Result();
			int64_t get_Mem_Op_Value();
			int get_memtoreg();
			int get_memwrite();
			int get_branch();
			int get_alusrc();
			int get_regdst();
			int get_regwrite();
			
			void set_instruction(string instruction);
			void set_PC(uint32_t PC);
			void set_ALU_Result(int64_t ALU_Result);
			void set_Mem_Op_Value(int64_t Mem_Op_Value);
			void set_memtoreg(int memtoreg);
			void set_memwrite(int memwrite);
			void set_branch(int branch);
			void set_alusrc(int alusrc);
			void set_regdst(int regdst);
			void set_regwrite(int regwrite);
			
		
		private:
			string instruction; 
			uint32_t PC;
			int64_t ALU_Result;
			int64_t Mem_Op_Value;	
			
			//control lines: integers as all are either off (0) or on (1)
			int memtoreg;
			int memwrite;
			int branch;
			int alusrc;
			int regdst; 
			int regwrite;
			
			
		
	};
	
#endif