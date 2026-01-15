#include "Processor.h"
#include "PipReg.h"
#include <string>
#include <vector>
#include <stdexcept> 

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
	void Processor::Set_Instr_Mem(const vector<string> instr_mem) {
		Instruction_Mem = instr_mem;
	}
	
	
	//convert_binary takes a string which is a size 8 string in hex
	//and returns a string of size 32 of the equivalent hex number in binary
	//used to convert hex machine code instructions into 32 bit binary instructions 
	string Processor::convert_binary(string instruction) {
	  // substring instrucion (0,1) = 4 binary digits
	  string bin = "";
	  for (int i = 0; i < instruction.size(); i++) {
		char cur = instruction[i];
		if (cur == '0')
		  bin += "0000";
		if (cur == '1')
		  bin += "0001";
		if (cur == '2')
		  bin += "0010";
		if (cur == '3')
		  bin += "0011";
		if (cur == '4')
		  bin += "0100";
		if (cur == '5')
		  bin += "0101";
		if (cur == '6')
		  bin += "0110";
		if (cur == '7')
		  bin += "0111";
		if (cur == '8')
		  bin += "1000";
		if (cur == '9')
		  bin += "1001";
		if (cur == 'A' || cur == 'a')
		  bin += "1010";
		if (cur == 'B' || cur == 'b')
		  bin += "1011";
		if (cur == 'C' || cur == 'c')
		  bin += "1100";
		if (cur == 'D' || cur == 'd')
		  bin += "1101";
		if (cur == 'E' || cur == 'e')
		  bin += "1110";
		if (cur == 'F' || cur == 'f')
		  bin += "1111";
	  }
	  return bin;
	}

	// get_reg takes a string of length 5 which is either the rs, rt, or rd part of a (binary) instruction and returns the integer position in the Register_File vector that corresponds
	int Processor::get_reg(string reg) {
	  int two_multiple = 16; // 2^4
	  int decimal_number = 0;
	  for (int i = 0; i < reg.size(); i++) {
		if (reg.substr(i, 1) == "1")
		  decimal_number += two_multiple;

		two_multiple /= 2;
	  }
	  return decimal_number;
	}

	// get_immediate takes a string which is a size 16 string and returns the 
	//equivalent number in decimal 
	int Processor::get_immediate(string imm) {
	  int two_multiple = 32768; // 2^15
	  int decimal_number = 0;
	  for (int i = 0; i < imm.size(); i++) {
		if (imm.substr(i, 1) == "1")
		  decimal_number += two_multiple;

		two_multiple /= 2;
	  }
	  return decimal_number;
	}

	//get_dec takes a string which is a binary address of size 32
	//and returns the equivalent number in decimal 
	int Processor::get_dec(string address) {
	  unsigned int two_multiple = 2147483648; // 2^31
	  int decimal_number = 0;
	  for (int i = 0; i < address.size(); i++) {
		if (address.substr(i, 1) == "1")
		  decimal_number += two_multiple;

		two_multiple /= 2;
	  }
	  return decimal_number;
	}

	//twos_comp takes a string val, which is a binary two's compliment number and returns the equivalent positive value in decimal 
	int Processor::twos_comp(string val) {
	  string new_imm = "";
	  for(int i = 0; i < val.length(); i++) {
		if(val.substr(i, 1) == "1")
		  new_imm += "0";
		else
		  new_imm += "1";
	  }
	  
	  int twoscomp = get_immediate(new_imm);
	  twoscomp += 1;
	  twoscomp *= -1;
	  //cout << "newimmm: " << twoscomp << endl;
	  return twoscomp;
	}

	//takes a number in decimal and returns 32 bit binary equivalent as a string
	//builds the string by repeatedly taking the mod of our number, neeeds to reverse
	//string at end because in this way, we build the binary number backwords 
	string Processor::get_bin(int num) {
	  string bin_num = "";
	  while(num > 0) {
		if(num %2 == 1)
		bin_num += "1";
		else
		  bin_num += "0";
		
		num = num/2; 
	  }
	  //loop to fill remaining bits of the 32 with zeros, because we dont 
	  //specify a size for our decimal number, and it will likely not use all
	  //32 bits when converted to binary 
	  string temp = "";
	  for(int i = 0; i < 32 - bin_num.length(); i++){
		temp += "0";
	  }
	  reverse(bin_num.begin(), bin_num.end()); //c++ built-in function for strings
	  return temp + bin_num; //extra zeros become the rightmost bits after the reverse 
	}

	// R[rd] = R[rs] + R[rt]
	//takes a string instruction which is an instruction of size 32 in binary 
	//and adds the value of Register_File[rs] + Register_File[rt] to Register_File[rd] 
	//NOTE- no r-type functions need to check to see if we're trying to modify $zero, because we check for that in main before we call determine_r_type() 
	void Processor::add(string instruction) {
	  //cout << "add"; 
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  Register_File[get_reg(instruction.substr(6, 5))] +
		  Register_File[get_reg(instruction.substr(11, 5))]; 
	}

	// R[rd] = R[rs] & R[rt]
	//takes a string instruction which is an instruction of size 32 in binary
	//stores Register_File[rs] && Register_File[rt] into Register_File[rd]
	void Processor::And(string instruction) {
	  //cout << "And";
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  Register_File[get_reg(instruction.substr(6, 5))] &&
		  Register_File[get_reg(instruction.substr(11, 5))];
	}

	// PC = R[rs]
	//takes a string instruction which is an instruction of size 32 in binary
	//and sets the program counter to the value at Register_File[rs] (which is a valid address for the PC, stored when jal instruction was executed)
	void Processor::jr(string instruction) {
	  //cout << "jr";
	  PC = Register_File[get_reg(instruction.substr(6, 5))];
	}

	// R[rd] = ~(R[rs] | R[rt])
	//takes a string instruction which is an instruction of size 32 in binary
	//stores the opposite of Register_File[rs] || Register_File[rt] into Register_File[rd]
	void Processor::Nor(string instruction) {
	  //cout << "nor";
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  !((Register_File[get_reg(instruction.substr(6, 5))]) ||
			(Register_File[get_reg(instruction.substr(11, 5))]));
	}

	// R[rd] = R[rs] | R[rt]
	//takes a string instruction which is an instruction of size 32 in binary
	//stores the value of Register_File[rs] || Register_File[rt] into Register_File[rd]
	void Processor::Or(string instruction) {
	  //cout << "or";
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  (Register_File[get_reg(instruction.substr(6, 5))]) ||
		  (Register_File[get_reg(instruction.substr(11, 5))]);
	}

	// R[rd] = (R[rs] < R[rt]) ? 1 : 0
	//takes a string instruction which is an instruction of size 32 in binary
	//if Register_File[rs] < Register_File[rt], stores a 1 in Register_File[rd], otherwise stores a 0 in Register_File[rd] 
	void Processor::slt(string instruction) {
	  //cout << "slt";
	  if (Register_File[get_reg(instruction.substr(6, 5))] <
		  Register_File[get_reg(instruction.substr(11, 5))])
		Register_File[get_reg(instruction.substr(16, 5))] = 1;
	  else
		Register_File[get_reg(instruction.substr(16, 5))] = 0;
	}

	// R[rd] = R[rt] << shamt
	//takes a string instruction which is an instruction of size 32 in binary
	//stores in Register_File[rd] the value in Register_File[rt] shifted left by shamt amt determined in the binary instruction 
	void Processor::sll(string instruction) {
	  //cout << "sll";
	  int shamt_amt = get_reg(instruction.substr(21, 5));
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  Register_File[get_reg(instruction.substr(11, 5))] << shamt_amt;
	}

	// R[rd] = R[rt] >> shamt
	//takes a string instruction which is an instruction of size 32 in binary
	//stores in Register_File[rd] the value in Register_File[rt] shifted right by shamt amt determined in the binary instruction 
	void Processor::srl(string instruction) {
	 // cout << "srl";
	  int shamt_amt = get_reg(instruction.substr(21, 5));
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  Register_File[get_reg(instruction.substr(11, 5))] >> shamt_amt;
	}

	// R[rd] = R[rs] - R[rt]
	//takes a string instruction which is an instruction of size 32 in binary
	//stores in Register_File[rd] the value in Register_File[rs] - value in Register_File[rt]
	void Processor::Sub(string instruction) {
	 // cout << "sub";
	  Register_File[get_reg(instruction.substr(16, 5))] =
		  Register_File[get_reg(instruction.substr(6, 5))] -
		  Register_File[get_reg(instruction.substr(11, 5))];
	}

	//takes a string instruction which is an instruction of size 32 in binary
	//does an opperation according to the value stored in Register_File[2], which is $v0
	void Processor::syscall(string instruction) {
	  cout << "syscall ";
	  // if $v0 == 5, read in an int from the keyboard and store in v0
	  if (Register_File[2] == 5) {
		int input;
		cin >> input;
		Register_File[2] = input;
	  }

	  // if$v0 == 1, print the value in $a0 to the screen
	  else if (Register_File[2] == 1) {
		cout << Register_File[4];
	  }
	  
	  // if$v0 == 0, the values of all 32 Register_File are printed to screen (use print vector) 
	  else if (Register_File[2] == 0) {
		  Register_Dump(Register_File); 
		   cout << endl;
		}
		  
	// if$v0 == 10, end the program, and do a final register dump (print
	  // vector on Register_File again) 
		else if(Register_File[2] == 10) {
		  Register_Dump(Register_File);
		 return;
		} 
		  
	  //if $v0 == anything else, give a run-time error
	  else {
		cout << "Error: invalid system call value in $v0";
		return; 
	  }
	}

	//takes a string instruction which is an instruction of size 32 in binary
	//calls a function determined by the function code of the instruction, which is the last 6 digits of the instruction
	void Processor::determine_r_type(string instruction) {
	 // cout << instruction << endl;
	  if (instruction.substr(26, 6) == "100000")
		Add(instruction);
	  else if (instruction.substr(26, 6) == "100100")
		And(instruction);
	  else if (instruction.substr(26, 6) == "001000")
		jr(instruction);
	  else if (instruction.substr(26, 6) == "100111")
		Nor(instruction);
	  else if (instruction.substr(26, 6) == "100101")
		Or(instruction);
	  else if (instruction.substr(26, 6) == "101010")
		slt(instruction);
	  else if (instruction.substr(26, 6) == "000000")
		sll(instruction);
	  else if (instruction.substr(26, 6) == "000010")
		srl(instruction);
	  else if (instruction.substr(26, 6) == "100010")
		Sub(instruction);
	  else if (instruction.substr(26, 6) == "001100")
		syscall(instruction);
	  else {
		cout << "Error: no such operation";
		return;
	  }
	} 

	//R[31]=PC+8;PC=JumpAddr
	//takes a string instruction which is an instruction of size 32 in binary
	//sets Register_File[31], or $ra, to PC + 8, sets PC to the jump address determiend by the instruction
	//compute a 32 bit address which from the leftmost 4 bits of the PC + the jump address determined by the instruction + two 00 bits (because all jump addresses are divided by 4 to save bits)
	//subtract this address from the PC address (and divide by 4 because of how memory is implemented in this program)
	void Processor::jal(string instruction) { 
	  //cout << "jal"; 
	  Register_File[31] = PC + 4; //because we already do PC+4 in main 
	string new_pc = get_bin(PC).substr(0, 4) + instruction.substr(6, 26) + "00";
	  PC = PC -(get_dec(new_pc))/4;
	  }

	//PC=JumpAddr
	//takes a string instruction which is an instruction of size 32 in binary
	//compute a 32 bit address which from the leftmost 4 bits of the PC + the jump address determined by the instruction + two 00 bits (because all jump addresses are divided by 4 to save bits)
	//subtract this address from the PC address (and divide by 4 because of how memory is implemented in this program)
	void Processor::j(string instruction) { 
	  //cout << "j";  
	 string new_pc = get_bin(PC).substr(0, 4) + instruction.substr(6, 26) + "00";
	   // cout << get_dec(new_pc) << endl;
	 // cout <<  PC - (get_dec(new_pc))/4;
	  PC = PC -(get_dec(new_pc))/4;  
	  }

	//takes a string instruction which is an instruction of size 32 in binary
	//calls a function determined by the opcode code of the instruction, which is the first 6 digits of the instruction
	void Processor::determine_j_type(string instruction) {
	  //cout << instruction << endl;
	  if (instruction.substr(0, 6) == "000011")
		jal(instruction);
	  else if (instruction.substr(0, 6) == "000010")
		j(instruction);
	  else {
		cout << "Error: no such operation";
		return;
	  }
	}

	//R[rt] = R[rs] + SignExtImm
	//takes a string instruction which is an instruction of size 32 in binary
	//stores in Register_File[rt] the value in Register_File[rs] + the immediate value determined by the instruction 
	//if the leftmost bit of the immediate == 1, immediate is negative, so 
	//do two's compliment function to produce the equivalent negative number (call twos_comp helper function) (same goes for all i-type instructions with immediates)
	void Processor::addi(string instruction) { 
	  //cout << "addi";
	  int immediate;
	  if(instruction.substr(16, 1) == "1" )  //immediate is negative
		immediate = twos_comp(instruction.substr(16, 16));
		else 
		immediate = get_immediate(instruction.substr(16, 16));
	  
	  if(get_reg(instruction.substr(11, 5)) != 0) {
	  Register_File[get_reg(instruction.substr(11, 5))] = Register_File[get_reg(instruction.substr(6, 5))] + immediate;
		}
	  }

	//R[rt] = R[rs] & ZeroExtImm
	//takes a string instruction which is an instruction of size 32 in binary
	//stores in Register_File[rt] the value at Register_File[rs] && the immediate value determined by the lower 16 bits of the instruction 
	void Processor::andi(string instruction) { 
	 //cout << "andi";
	  int immediate;
	  if(instruction.substr(16, 1) == "1" )  //immediate is negative
		immediate = twos_comp(instruction.substr(16, 16));
		else 
		immediate = get_immediate(instruction.substr(16, 16));
		
	  if(get_reg(instruction.substr(11, 5)) != 0) {
	  Register_File[get_reg(instruction.substr(11, 5))] = Register_File[get_reg(instruction.substr(6, 5))] && immediate;
		}
	  }

	//if(R[rs]==R[rt])
	//PC=PC+4+BranchAddr
	//if the value in Register_File[rs] == Register_File[rt], change the PC to be PC + the branch address determined by the instruction multiplied by 4, because valid addresses in memory[] are at interavls of 4
	//subtract 4 from PC because we do PC = PC + 4 in main still after this instruction is pulled out of the instructions[]
	void Processor::beq(string instruction) {
	  //cout << "beq";   
	  if(Register_File[get_reg(instruction.substr(6, 5))] == Register_File[get_reg(instruction.substr(11, 5))])
		PC = PC - 4 + (4 * get_immediate(instruction.substr(16,16)));
	  }

	//if(R[rs]!=R[rt])
	//PC=PC+4+BranchAddr
	//if the value in Register_File[rs] != Register_File[rt], change the PC to be PC + the branch address determined by the instruction multiplied by 4, because valid addresses in memory[] are at interavls of 4
	//subtract 4 from PC because we do PC = PC + 4 in main still after this instruction is pulled out of the instructions[]
	void Processor::bne(string instruction) { 
	  //cout << "bne";
	  if(Register_File[get_reg(instruction.substr(6, 5))] != Register_File[get_reg(instruction.substr(11, 5))])
		PC = PC - 4 + (4 * get_immediate(instruction.substr(16,16)));
	  }

	//R[rt] = {imm, 16’b0}
	//load into upper 16 bits = immediate
	//takes a string instruction which is an instruction of size 32 in binary
	//sets Register_File[rt] to the immediate value shifted left by 16
	void Processor::lui(string instruction) { 
	  //cout << "lui";
	 unsigned int shifted_imm = get_immediate(instruction.substr(16, 16)) << 16;
	  if(get_reg(instruction.substr(11, 5)) != 0) { 
	  Register_File[get_reg(instruction.substr(11, 5))] = shifted_imm;
		}
	  }

	//R[rt] = M[R[rs]+SignExtImm]
	//really, M[(R[rs]+SignExtImm)/4] = R[rt], because memory here is stored assming all enttries are 4 byte long words 
	//takes a string instruction which is an instruction of size 32 in binary
	//sets Register_File[rt] to be memory at Register_File[rs + immediate], which is the immediate value determined by the lower 16 bits of the instruction
	void Processor::lw(string instruction) { 
	  //cout << "lw";  
	  if(get_reg(instruction.substr(11, 5)) != 0) {
	 Register_File[get_reg(instruction.substr(11, 5))] = memory[(Register_File[get_reg(instruction.substr(6, 5))] + get_immediate(instruction.substr(16,16)))/4];
		}
	  }

	//R[rt] = R[rs] | ZeroExtImm
	//takes a string instruction which is an instruction of size 32 in binary
	//stores in Register_File[rt] the value of Register_File[rs] || immediate, where the immediate is determined by the lower 16 bits of the instruction 
	void Processor::ori(string instruction) { 
	  //cout << "ori";
	  int immediate;
	  if(instruction.substr(16, 1) == "1" )  //immediate is negative
		immediate = twos_comp(instruction.substr(16, 16));
		else 
		immediate = get_immediate(instruction.substr(16, 16));
	  
	  if(get_reg(instruction.substr(11, 5)) != 0) {
	  Register_File[get_reg(instruction.substr(11, 5))] = (Register_File[get_reg(instruction.substr(6, 5))] || immediate );
		}
	  }

	//R[rt] = (R[rs] < SignExtImm)? 1 : 0
	//takes a string instruction which is an instruction of size 32 in binary
	//if the value at Register_File[rt] < immediate, which is determined by the lower 16 bits of the instruction, set Register_File[rt] to be 1, otherwise set 
	//Register_File[rt] to be 0
	void Processor::slti(string instruction) { 
	  //cout << "slti";
	  int immediate;
	  if(instruction.substr(16, 1) == "1" )  //immediate is negative
		immediate = twos_comp(instruction.substr(16, 16));
		else 
		immediate = get_immediate(instruction.substr(16, 16));
	  
	  if(get_reg(instruction.substr(11, 5)) != 0) {
	   if(Register_File[get_reg(instruction.substr(6, 5))] < immediate)
	  Register_File[get_reg(instruction.substr(11, 5))] = 1;
		 else 
		 Register_File[get_reg(instruction.substr(11, 5))] = 0;
		}
	  }

	//M[R[rs]+SignExtImm] = R[rt]
	//really, M[(R[rs]+SignExtImm)/4] = R[rt], because memory here is stored assming all enttries are 4 byte long words 
	//takes a string instruction which is an instruction of size 32 in binary
	//stores Register_File[rt] into memory at Register_File[rs] + immediate, where immediate is determined by lower 16 bits of the instruction 
	void Processor::sw(string instruction) { 
	  //cout << "sw"; 
	  if(get_reg(instruction.substr(11, 5)) != 0) {
	memory[(Register_File[get_reg(instruction.substr(6, 5))] + get_immediate(instruction.substr(16,16)))/4] = Register_File[get_reg(instruction.substr(11, 5))];
		}
	  }

	//takes a string instruction which is an instruction of size 32 in binary
	//looks at the opcode of the instruction, the first 6 bits, and calls a corresponding i type function determined by the opcode 
	void Processor::determine_i_type(string instruction) {
	  //cout << instruction << endl;
	  if (instruction.substr(0, 6) == "001000")
		addi(instruction);
	  else if (instruction.substr(0, 6) == "001100")
		andi(instruction);
	  else if (instruction.substr(0, 6) == "000100")
		beq(instruction);
	  else if (instruction.substr(0, 6) == "000101")
		bne(instruction);
	  else if (instruction.substr(0, 6) == "001111")
		lui(instruction);
	  else if (instruction.substr(0, 6) == "100011")
		lw(instruction);
	  else if (instruction.substr(0, 6) == "001101")
		ori(instruction);
	  else if (instruction.substr(0, 6) == "001010")
		slti(instruction);
	  else if (instruction.substr(0, 6) == "101011")
		sw(instruction);
	  else {
		cout << "Error: no such instruction";
		return;
	  }
	}
	
	//used for syscall when $v0 == 0 (register dump) 		
	void Processor::Register_Dump(){
		vector<string> regs = {"$zero", "$at", "$v0", "v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", 
			"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
				
		for(int i = 0; i < Register_File.size(); i++)
		    cout << regs[i] << ": " << Register_File[i] << endl;
		
		    cout << endl;
	}
	
	
	
	//cycle through instructions and shoot them at the pipeline
	void Processor::Tick() {
		//loop through instruction mem and shoot each one at IF 
		//make sure memeory is ASCIIz'd and all that - give you space for evetrything you allocate in the beginning
		//make PC whatever address the first instruction is at 
		//$gp points to those things like stack 
		
		/* 
		Each tick, these things need to happen: 
			
			1. Make 5 calls: IF(); ID(IF_ID); EX(ID_EX); .... note that if a pip reg is empty, that stage should just return back 
				*note that IF "fetches" the next instruction at the current PC, so we don't need a seperate step for that*
			
			2. Previously fetched instructions move forward one stage (pipeline regs "swap") 
				IF → ID
				ID → EX
				EX → MEM
				MEM → WB
			*need to be careful here, only non-empty pipregs should be moved to the next pip reg* (just an if statement) 
			
			3. PC = PC+4; (or the branch target?) 
		*/
	}
	
	//a version of Tick() which interupts and prints out processor state info at the end of each tick
	void Processor::Tick_Debug() {
		
		
		cout << "PC: " << PC << endl; 
		cout << "IF: " << endl;
		cout << "ID: " << endl;
		cout << "EX: " << endl;
		cout << "MEM: " << endl;
		cout << "WB: " << endl;
		
		//optional: register dump
		//Register_Dump();
		
		cout << "Hit enter to continue" << endl;
		char cont;
		cont = cin.get();
		
		PC = PC+4; 		
	}
	
	/*pipeline stage functions*/
	
	void Processor::IF() {
		//get instruction from instruction mem at PC
		string instruction = Instruction_Mem[PC];
		/*CHANGE THIS PC IS NOW A STRING */PC = PC + 4;
		Id(PipReg(instruction));
		
	}
	
	void Processor::ID() {
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
	
	void Processor::EX() {
		
	}
	
	void Processor::MEM() {
		
	} 
	
	void Processor::WB() {
		
	}


	
	
	