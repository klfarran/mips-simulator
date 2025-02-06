#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include<bits/stdc++.h> 

using namespace std;

int PC;
vector<int> registers(32);
//$zero = 0, $at = 1, $v0-v1 = 2-3
//$a0-a3 = 4-7, $t0-t7 = 8-15
//$s0-s7 = 16-23. $t8-t9 = 24-25
//$k0-k1 = 26-27, $gp = 28, $sp = 29
//$fp = 30, $ra = 31
vector<int> memory(3000);
vector<string>instructions;

//Print_Vector takes an integer vector v and prints each element
//to the screen with a space in between 
//used for syscall when $v0 == 0 (register dump) 
void Print_Vector(const vector<int>& v){
  for(int i = 0; i < v.size(); i++)
    cout << v[i] << " ";
  cout << endl;
}

//convert_binary takes a string which is a size 8 string in hex
//and returns a string of size 32 of the equivalent hex number in binary
//used to convert hex machine code instructions into 32 bit binary instructions 
string convert_binary(string instruction) {
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

// get_reg takes a string which is either the rs, rt, or rd part of a (binary) instruction and returns the integer position in the registers vector that corresponds
int get_reg(string reg) {
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
int get_immediate(string imm) {
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
int get_dec(string address) {
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
int twos_comp(string val) {
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
string get_bin(int num) {
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
//and adds the value of registers[rs] + registers[rt] to registers[rd] 
//NOTE- no r-type functions need to check to see if we're trying to modify $zero, because we check for that in main before we call determine_r_type() 
void Add(string instruction) {
  //cout << "add"; 
  registers[get_reg(instruction.substr(16, 5))] =
      registers[get_reg(instruction.substr(6, 5))] +
      registers[get_reg(instruction.substr(11, 5))]; 
}

// R[rd] = R[rs] & R[rt]
//takes a string instruction which is an instruction of size 32 in binary
//stores registers[rs] && registers[rt] into registers[rd]
void And(string instruction) {
  //cout << "And";
  registers[get_reg(instruction.substr(16, 5))] =
      registers[get_reg(instruction.substr(6, 5))] &&
      registers[get_reg(instruction.substr(11, 5))];
}

// PC = R[rs]
//takes a string instruction which is an instruction of size 32 in binary
//and sets the program counter to the value at registers[rs] (which is a valid address for the PC, stored when jal instruction was executed)
void jr(string instruction) {
  //cout << "jr";
  PC = registers[get_reg(instruction.substr(6, 5))];
}

// R[rd] = ~(R[rs] | R[rt])
//takes a string instruction which is an instruction of size 32 in binary
//stores the opposite of registers[rs] || registers[rt] into registers[rd]
void Nor(string instruction) {
  //cout << "nor";
  registers[get_reg(instruction.substr(16, 5))] =
      !((registers[get_reg(instruction.substr(6, 5))]) ||
        (registers[get_reg(instruction.substr(11, 5))]));
}

// R[rd] = R[rs] | R[rt]
//takes a string instruction which is an instruction of size 32 in binary
//stores the value of registers[rs] || registers[rt] into registers[rd]
void Or(string instruction) {
  //cout << "or";
  registers[get_reg(instruction.substr(16, 5))] =
      (registers[get_reg(instruction.substr(6, 5))]) ||
      (registers[get_reg(instruction.substr(11, 5))]);
}

// R[rd] = (R[rs] < R[rt]) ? 1 : 0
//takes a string instruction which is an instruction of size 32 in binary
//if registers[rs] < registers[rt], stores a 1 in registers[rd], otherwise stores a 0 in registers[rd] 
void slt(string instruction) {
  //cout << "slt";
  if (registers[get_reg(instruction.substr(6, 5))] <
      registers[get_reg(instruction.substr(11, 5))])
    registers[get_reg(instruction.substr(16, 5))] = 1;
  else
    registers[get_reg(instruction.substr(16, 5))] = 0;
}

// R[rd] = R[rt] << shamt
//takes a string instruction which is an instruction of size 32 in binary
//stores in registers[rd] the value in registers[rt] shifted left by shamt amt determined in the binary instruction 
void sll(string instruction) {
  //cout << "sll";
  int shamt_amt = get_reg(instruction.substr(21, 5));
  registers[get_reg(instruction.substr(16, 5))] =
      registers[get_reg(instruction.substr(11, 5))] << shamt_amt;
}

// R[rd] = R[rt] >> shamt
//takes a string instruction which is an instruction of size 32 in binary
//stores in registers[rd] the value in registers[rt] shifted right by shamt amt determined in the binary instruction 
void srl(string instruction) {
 // cout << "srl";
  int shamt_amt = get_reg(instruction.substr(21, 5));
  registers[get_reg(instruction.substr(16, 5))] =
      registers[get_reg(instruction.substr(11, 5))] >> shamt_amt;
}

// R[rd] = R[rs] - R[rt]
//takes a string instruction which is an instruction of size 32 in binary
//stores in registers[rd] the value in registers[rs] - value in registers[rt]
void Sub(string instruction) {
 // cout << "sub";
  registers[get_reg(instruction.substr(16, 5))] =
      registers[get_reg(instruction.substr(6, 5))] -
      registers[get_reg(instruction.substr(11, 5))];
}

//takes a string instruction which is an instruction of size 32 in binary
//does an opperation according to the value stored in registers[2], which is $v0
void syscall(string instruction) {
  cout << "syscall ";
  // if $v0 == 5, read in an int from the keyboard and store in v0
  if (registers[2] == 5) {
    int input;
    cin >> input;
    registers[2] = input;
  }

  // if$v0 == 1, print the value in $a0 to the screen
  else if (registers[2] == 1) {
    cout << registers[4];
  }
  
  // if$v0 == 0, the values of all 32 registers are printed to screen (use print vector) 
  else if (registers[2] == 0) {
      Print_Vector(registers); 
       cout << endl;
    }
      
// if$v0 == 10, end the program, and do a final register dump (print
  // vector on registers again) 
    else if(registers[2] == 10) {
      Print_Vector(registers);
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
void determine_r_type(string instruction) {
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
//sets registers[31], or $ra, to PC + 8, sets PC to the jump address determiend by the instruction
//compute a 32 bit address which from the leftmost 4 bits of the PC + the jump address determined by the instruction + two 00 bits (because all jump addresses are divided by 4 to save bits)
//subtract this address from the PC address (and divide by 4 because of how memory is implemented in this program)
void jal(string instruction) { 
  //cout << "jal"; 
  registers[31] = PC + 4; //because we already do PC+4 in main 
string new_pc = get_bin(PC).substr(0, 4) + instruction.substr(6, 26) + "00";
  PC = PC -(get_dec(new_pc))/4;
  }

//PC=JumpAddr
//takes a string instruction which is an instruction of size 32 in binary
//compute a 32 bit address which from the leftmost 4 bits of the PC + the jump address determined by the instruction + two 00 bits (because all jump addresses are divided by 4 to save bits)
//subtract this address from the PC address (and divide by 4 because of how memory is implemented in this program)
void j(string instruction) { 
  //cout << "j";  
 string new_pc = get_bin(PC).substr(0, 4) + instruction.substr(6, 26) + "00";
   // cout << get_dec(new_pc) << endl;
 // cout <<  PC - (get_dec(new_pc))/4;
  PC = PC -(get_dec(new_pc))/4;  
  }

//takes a string instruction which is an instruction of size 32 in binary
//calls a function determined by the opcode code of the instruction, which is the first 6 digits of the instruction
void determine_j_type(string instruction) {
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
//stores in registers[rt] the value in registers[rs] + the immediate value determined by the instruction 
//if the leftmost bit of the immediate == 1, immediate is negative, so 
//do two's compliment function to produce the equivalent negative number (call twos_comp helper function) (same goes for all i-type instructions with immediates)
void addi(string instruction) { 
  //cout << "addi";
  int immediate;
  if(instruction.substr(16, 1) == "1" )  //immediate is negative
    immediate = twos_comp(instruction.substr(16, 16));
    else 
    immediate = get_immediate(instruction.substr(16, 16));
  
  if(get_reg(instruction.substr(11, 5)) != 0) {
  registers[get_reg(instruction.substr(11, 5))] = registers[get_reg(instruction.substr(6, 5))] + immediate;
    }
  }

//R[rt] = R[rs] & ZeroExtImm
//takes a string instruction which is an instruction of size 32 in binary
//stores in registers[rt] the value at registers[rs] && the immediate value determined by the lower 16 bits of the instruction 
void andi(string instruction) { 
 //cout << "andi";
  int immediate;
  if(instruction.substr(16, 1) == "1" )  //immediate is negative
    immediate = twos_comp(instruction.substr(16, 16));
    else 
    immediate = get_immediate(instruction.substr(16, 16));
    
  if(get_reg(instruction.substr(11, 5)) != 0) {
  registers[get_reg(instruction.substr(11, 5))] = registers[get_reg(instruction.substr(6, 5))] && immediate;
    }
  }

//if(R[rs]==R[rt])
//PC=PC+4+BranchAddr
//if the value in registers[rs] == registers[rt], change the PC to be PC + the branch address determined by the instruction multiplied by 4, because valid addresses in memory[] are at interavls of 4
//subtract 4 from PC because we do PC = PC + 4 in main still after this instruction is pulled out of the instructions[]
void beq(string instruction) {
  //cout << "beq";   
  if(registers[get_reg(instruction.substr(6, 5))] == registers[get_reg(instruction.substr(11, 5))])
    PC = PC - 4 + (4 * get_immediate(instruction.substr(16,16)));
  }

//if(R[rs]!=R[rt])
//PC=PC+4+BranchAddr
//if the value in registers[rs] != registers[rt], change the PC to be PC + the branch address determined by the instruction multiplied by 4, because valid addresses in memory[] are at interavls of 4
//subtract 4 from PC because we do PC = PC + 4 in main still after this instruction is pulled out of the instructions[]
void bne(string instruction) { 
  //cout << "bne";
  if(registers[get_reg(instruction.substr(6, 5))] != registers[get_reg(instruction.substr(11, 5))])
    PC = PC - 4 + (4 * get_immediate(instruction.substr(16,16)));
  }

//R[rt] = {imm, 16’b0}
//load into upper 16 bits = immediate
//takes a string instruction which is an instruction of size 32 in binary
//sets registers[rt] to the immediate value shifted left by 16
void lui(string instruction) { 
  //cout << "lui";
 unsigned int shifted_imm = get_immediate(instruction.substr(16, 16)) << 16;
  if(get_reg(instruction.substr(11, 5)) != 0) { 
  registers[get_reg(instruction.substr(11, 5))] = shifted_imm;
    }
  }

//R[rt] = M[R[rs]+SignExtImm]
//really, M[(R[rs]+SignExtImm)/4] = R[rt], because memory here is stored assming all enttries are 4 byte long words 
//takes a string instruction which is an instruction of size 32 in binary
//sets registers[rt] to be memory at registers[rs + immediate], which is the immediate value determined by the lower 16 bits of the instruction
void lw(string instruction) { 
  //cout << "lw";  
  if(get_reg(instruction.substr(11, 5)) != 0) {
 registers[get_reg(instruction.substr(11, 5))] = memory[(registers[get_reg(instruction.substr(6, 5))] + get_immediate(instruction.substr(16,16)))/4];
    }
  }

//R[rt] = R[rs] | ZeroExtImm
//takes a string instruction which is an instruction of size 32 in binary
//stores in registers[rt] the value of registers[rs] || immediate, where the immediate is determined by the lower 16 bits of the instruction 
void ori(string instruction) { 
  //cout << "ori";
  int immediate;
  if(instruction.substr(16, 1) == "1" )  //immediate is negative
    immediate = twos_comp(instruction.substr(16, 16));
    else 
    immediate = get_immediate(instruction.substr(16, 16));
  
  if(get_reg(instruction.substr(11, 5)) != 0) {
  registers[get_reg(instruction.substr(11, 5))] = (registers[get_reg(instruction.substr(6, 5))] || immediate );
    }
  }

//R[rt] = (R[rs] < SignExtImm)? 1 : 0
//takes a string instruction which is an instruction of size 32 in binary
//if the value at registers[rt] < immediate, which is determined by the lower 16 bits of the instruction, set registers[rt] to be 1, otherwise set 
//registers[rt] to be 0
void slti(string instruction) { 
  //cout << "slti";
  int immediate;
  if(instruction.substr(16, 1) == "1" )  //immediate is negative
    immediate = twos_comp(instruction.substr(16, 16));
    else 
    immediate = get_immediate(instruction.substr(16, 16));
  
  if(get_reg(instruction.substr(11, 5)) != 0) {
   if(registers[get_reg(instruction.substr(6, 5))] < immediate)
  registers[get_reg(instruction.substr(11, 5))] = 1;
     else 
     registers[get_reg(instruction.substr(11, 5))] = 0;
    }
  }

//M[R[rs]+SignExtImm] = R[rt]
//really, M[(R[rs]+SignExtImm)/4] = R[rt], because memory here is stored assming all enttries are 4 byte long words 
//takes a string instruction which is an instruction of size 32 in binary
//stores registers[rt] into memory at registers[rs] + immediate, where immediate is determined by lower 16 bits of the instruction 
void sw(string instruction) { 
  //cout << "sw"; 
  if(get_reg(instruction.substr(11, 5)) != 0) {
memory[(registers[get_reg(instruction.substr(6, 5))] + get_immediate(instruction.substr(16,16)))/4] = registers[get_reg(instruction.substr(11, 5))];
    }
  }

//takes a string instruction which is an instruction of size 32 in binary
//looks at the opcode of the instruction, the first 6 bits, and calls a corresponding i type function determined by the opcode 
void determine_i_type(string instruction) {
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


int main() {
  //$gp starts at address 4000
  registers[28] = 4000;
  //$sp starts at address 12000
  registers[29] = 12000;

  // PC starts at address 0
  PC = 0;

  string filename;
  cout << "Enter the filename: ";
  cin >> filename;

  ifstream fin;
  fin.open(filename.data());

  while (!fin) {
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> filename;

    // remember to clear the error state before reopening
    fin.clear();
    fin.open(filename.data());
  }

  string instruction;

  //want instructions to be at every address that is a multiple of 4
  while (fin >> instruction) {
    instructions.push_back(instruction);
    instructions.push_back("00000000");
    instructions.push_back("00000000");
    instructions.push_back("00000000");
    }


 //while the PC is a valid address, run program  
   while(PC >= 0 && PC < instructions.size()) { 
		//convert the instruction from machine language to binary 
		string bin_instruction = convert_binary(instructions[PC]); 
		cout << "bin instruction: " << bin_instruction << endl;
		
		//seperate check for syscalls because rs field is 0, and other functions that try to have rs or rt of 0 (modify $zero) are supposed to silently do nothing 
		if (bin_instruction.substr(26, 6) == "001100") 
			syscall(instruction);  //else, determine the type of instruction based on the opcode: 
		else if (bin_instruction.substr(0, 6) == "000000"){ // R-type
			if(get_reg(bin_instruction.substr(16, 5)) != 0) 
				determine_r_type(bin_instruction);
		}
		else if (bin_instruction.substr(0, 6) == "000010" || bin_instruction.substr(0, 6) == "000011")  // J-type
			determine_j_type(bin_instruction);	
		else  // I-type
			determine_i_type(bin_instruction);
			
		//update to next address of PC
		PC = PC + 4;
	}
   
   
  
}