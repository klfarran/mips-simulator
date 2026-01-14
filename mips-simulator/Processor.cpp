#include "Processor.h"
#include "PipReg.h"
#include <string>
#include <vector>

	using namespace std;


	Processor::Processor(){
		// PC starts at address 0x00000000
		PC = 0x00000000;
		//main 004000240 half between sp and gp 
		//look at spim and load in a program to look at this stuff 
		
		//this is RAM memory 
		Memory.resize(3000);
		
		Register_File.resize(32);
		//$zero = 0, $at = 1, $v0-v1 = 2-3, $a0-a3 = 4-7, $t0-t7 = 8-15, $s0-s7 = 16-23. $t8-t9 = 24-25, $k0-k1 = 26-27, $gp = 28, $sp = 29, $fp = 30, $ra = 31
		
		//$gp starts at address 4000
		Register_File[28] = 4000;
		//$sp starts at address 12000
		Register_File[29] = 12000;
		
		//initialized to be empty. will be loaded in by the 'OS' when the user provides an input file
		Instruction_Mem = {};
	}
	
	//set instruction memory upon loading of a new program 
	void Processor:Set_Instr_Mem(const vector<string> instr_mem) {
		Instruction_Mem = instr_mem;
	}
	
	//cycle through instructions and shoot them at the pipeline
	void Processor::Go() {
		//loop through instruction mem and shoot each one at IF 
		//make sure memeory is ASCIIz'd and all that - give you space for evetrything you allocate in the beginning
		//make PC whatever address the first instruction is at 
		//$gp points to those things like stack 
	}
	
	/*pipeline stage functions*/
	
	void Processor::IF() {
		//get instruction from instruction mem at PC
		string instruction = Instruction_Mem[PC];
		/*CHANGE THIS PC IS NOW A STRING */PC = PC + 4;
		Id(PipReg(instruction));
		
	}
	
	void Processor::ID(PipReg IF_ID) {
		//get instruction out of the pipeline register
		string instruction = IF_ID.get_instruction();
		
		
		//break open the instruction, get operand values 
		
		//is this instruction a syscall? see if it looks like 0000000c: 
		if (instruction.substr(26, 6) == "001100") 
				PipReg.set_opcode("syscall");  
		//else, determine the type of instruction based on the opcode: 
		else if (instruction.substr(0, 6) == "000000"){ // R-type
			if(get_reg(instruction.substr(16, 5)) != 0) //check if $rd is $zero, if so silently ignore attempts to change the $zero register
				determine_r_type(instruction);
		}
		else if (instruction.substr(0, 6) == "000010" || instruction.substr(0, 6) == "000011")  // J-type
			determine_j_type(instruction);	
		else  // I-type
			determine_i_type(instruction);
			
			//NEED TO DO CONTROL WIRES because they tell if youyre writing, reading, etc, for later stages 
	}
	
	void Processor::EX(PipReg ID_EX) {
		
	}
	
	void Processor::MEM(PipReg EX_MEM) {
		
	} 
	
	void Processor::WB(PipReg MEM_WB) {
		
	}


	/*helper functions*/
	
	//used for syscall when $v0 == 0 (register dump) 
	void Processor::Register_Dump(const vector<int>& v){
	  for(int i = 0; i < v.size(); i++)
		cout << v[i] << " ";
	  cout << endl;
	}
	
	
	