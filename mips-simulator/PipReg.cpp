#include "PipReg.h"
#include <string>
#include <cstdint>

	using namespace std;

	//explicit constructor
	//creates an empty pipeline register 
	//control wires all start at 0 (off) by default
	PipReg::PipReg(){
		instruction = NULL;
		PC = 0x00000000;
		ALU_Result = 0;
		Mem_Op_Value = 0;
		memtoreg = 0;
		memwrite = 0;
		branch = 0;
		alusrc = 0;
		regdst = 0;
		regwrite = 0;
	}
	

	string PipReg::get_instruction() {
		return instruction;
	}
	
	uint32_t PipReg::get_PC() {
		return PC;
	}

	int64_t PipReg::get_ALU_Result() {
		return ALU_Result;
	}
	
	int64_t PipReg::get_Mem_Op_Value() {
		return Mem_Op_Value;
	}
	
	int PipReg::get_memtoreg() {
		return memtoreg;
	}
	
	int PipReg::get_memwrite() {
		return memwrite;
	}
	
	int PipReg::get_branch() {
		return branch;
	}
	
	int PipReg::get_alusrc() {
		return alusrc;
	}
	
	int PipReg::get_regdst() {
		return regdst;
	}
	
	int PipReg::get_regwrite() {
		return regwrite;
	}

	

	void PipReg::set_instruction(string instruction) {
		this->instruction = instruction;
	}
	
	
	void PipReg::set_PC(uint32_t PC) {
		this->instruction = instruction;
	}

	void PipReg::set_ALU_Result(int64_t ALU_Result) {
		this->ALU_Result = ALU_Result;
	}
	
	void PipReg::set_Mem_Op_Value(int64_t Mem_Op_Value) {
		this->Mem_Op_Value = Mem_Op_Value;
	}
	
	void PipReg::set_memtoreg(int memtoreg) {
		this->memtoreg = memtoreg;
	}
	
	void PipReg::set_memwrite(int memwrite) {
		this->memwrite = memwrite;
	}
	
	void PipReg::set_branch(int branch) {
		this->branch = branch;
	}
	
	void PipReg::set_alusrc(int alusrc) {
		this->alusrc = alusrc;
	}
	
	void PipReg::set_regdst(int regdst) {
		this->regdst = regdst;
	}
	
	void PipReg::set_regwrite(int regwrite) {
		this->
	}
