#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm> // for std::find

using namespace std;

/*
	This class holds relevent information for an instruction.  It also stores the values of the operand registers.
	InstructionNode nodes exist in INM, INB, LIB, and AIB.
*/
class InstructionNode {
public:
	InstructionNode(const string op, const string dest, const string one, const string two, const int oneVal, const int twoVal) {
		opcode = op;
		destReg = dest;
		opOne = one;
		opTwo = two;
		opOneVal = oneVal;
		opTwoVal = twoVal;
	}

	string opcode;
	string destReg;
	string opOne;
	string opTwo;
	int opOneVal;
	int opTwoVal; 
};

/*
	This class holds the information for nodes that are held in ADB and REB.  It holds the result
	of a data memory look up, or an ALU operation.
*/
class RegNode {
public:
	RegNode(const string reg, const int val) {
		regName = reg;
		data = val;
	}

	string regName;
	int data;
};

map<string, int> registers;  //this represents the system registers.  It maps the name of the reg to its value.
vector<int> dataMem;		 //this represents data memory in the system. 
vector<string> aluOps;		 //this stores the possible ALU operations.  I store it in a vector for easy look up later.
list<InstructionNode*> instructions;  //Represents INM buffer.  This list holds InstructionNode nodes for the instructions that are loaded from instructions.txt
list<InstructionNode*> INB;	 //List that represents instruction buffer.  It recieves InstructionNodes from INM beffer
list<InstructionNode*> AIB;	 //Represents AIB buffer.  Recieves ALU instruction operations from INB
list<InstructionNode*> LIB;  //Represents LIB buffer.  Recieves load instructions from INB
list<RegNode*> ADB;			 //Represents ADB buffer.  Recieves result from LIB buffer.
list<RegNode*> REB;			 //Represents REB buffer.  Recieves result from ADB and AIB buffer


//Helper function for splitting strings by a specified char deliminer.  Returns a vector of the splits.
vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;

	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}

	return tokens;
}

//helper function for properly getting the information from each line in instruction.txt
vector<string> getLineTokens(ifstream& file, string line) {
	line.erase(0, 1);  //remove angle braket at beginning
	line.erase(line.length() - 1, 1);  //remove angle braket at end
	vector<string> ret = split(line, ',');  //get tokens

	return ret;
}

//Function for initializing RGF
void initRegisters(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	
	while (getline(infile, line)) {  //for each line in registers.txt
		vector<string> currLine = getLineTokens(infile, line);
		registers[currLine[0]] = atoi(currLine[1].c_str());
	}

	return;
}

//function for initializing DAM
void initDataMem(string fileName) {
	ifstream infile(fileName.c_str());
	string line;

	while (getline(infile, line)) {  //for each line in memory.txt
		vector<string> currLine = getLineTokens(infile, line);
		dataMem.push_back(atoi(currLine[1].c_str()));
	}

	return;
}

//function for initializing INM 
void initInstructions(string fileName) {
	ifstream infile(fileName.c_str());
	string line;

	while (getline(infile, line)) {  //for each line in instructions.txt
		vector<string> currLine = getLineTokens(infile, line);
		int oneVal = registers[currLine[2]];
		int twoVal = registers[currLine[3]];
		InstructionNode* temp = new InstructionNode(currLine[0], currLine[1], currLine[2], currLine[3], oneVal, twoVal);
		instructions.push_back(temp);
	}

	return;
}

//pushes values for string representation of ALU operations.  Would have done this in one line on aluOps definition, but no C++98 support for that
void initOps() {
	aluOps.push_back("ADD");
	aluOps.push_back("AND");
	aluOps.push_back("SUB");
	aluOps.push_back("OR");
}


//helper function for checking if there is something still to do.
bool isDone() {
	int size = 0;

	size = instructions.size() + INB.size() + AIB.size() + LIB.size() + ADB.size() + REB.size();

	if (size > 0) {  //if any of these data structures has something, we are not done
		return false;
	}

	return true;
}

//C++98 helper function for casting an int to a string.  Useful for printing in for loop, and hash map lookups
string intToString(int n) {
	string res;
	ostringstream convert;
	convert << n;
	res = convert.str();

	return res;
}

//helper function for checking if the passed in string is an ALU operation
bool isALUOP(const string &op, const vector<string> &array) {
	return std::find(array.begin(), array.end(), op) != array.end();
}

//Processes an instruction in INM if it exists
void processINM() {
	if (instructions.size() > 0) {
		InstructionNode* temp = instructions.front();
		instructions.pop_front();
		INB.push_back(temp);
	}

	return;
}

//Processes an instruction in INB if it exists by checking the type of instruction and putting it in 
//the appropriate buffer
void processINB() {
	if (INB.size() > 0) {
		InstructionNode* temp = INB.front();

		if (isALUOP(temp->opcode, aluOps)) {
			AIB.push_back(temp);
		}
		else {
			LIB.push_back(temp);
		}

		INB.pop_front();
	}

	return;
}

//processes instruction in LIB if it exists.  Adds operands and pushes new RegNode to ADB for processing
void processLIB() {
	if (LIB.size() > 0) {
		InstructionNode* temp = LIB.front();

		int sum = temp->opOneVal + temp->opTwoVal;
		RegNode* reg = new RegNode(temp->destReg, sum);
		ADB.push_back(reg);
		LIB.pop_front();
	}

	return;
}

//processes nodes in here if one exists.  Gets data from data memory and pushes new RegNode to REB for final processing
void processADB() {
	if (ADB.size() > 0) {
		RegNode* temp = ADB.front();
		int data = dataMem[temp->data];
		RegNode* reg = new RegNode(temp->regName, data);
		REB.push_back(reg);
		ADB.pop_front();
	}

	return;
}

//Performes corresponding operation on instruction in AIB and stores result in new Regnode that gets pushed to REB for final processing
void processAIB() {
	if (AIB.size() > 0) {
		InstructionNode* temp = AIB.front();
		int res;

		if (temp->opcode == "ADD") {
			res = temp->opOneVal + temp->opTwoVal;
		}
		else if (temp->opcode == "AND") {
			res = temp->opOneVal && temp->opTwoVal;
		}
		else if (temp->opcode == "OR") {
			res = temp->opOneVal || temp->opTwoVal;
		}
		else if (temp->opcode == "SUB") {
			res = temp->opOneVal - temp->opTwoVal;
		}
		else {
			res = 0;
		}

		RegNode* reg = new RegNode(temp->destReg, res);
		REB.push_back(reg);
		AIB.pop_front();
	}

	return;
}

//Processes a RegNode here if it exists in REB.  This is simply updating the register through a hash map lookup
void processREB() {
	if (REB.size() > 0) {
		RegNode* temp = REB.front();
		registers[temp->regName] = temp->data;
		REB.pop_front();
	}

	return;
}

//Helper function for printing the system at the end of each step
void printStep(int stepNo, ofstream &file) {
	string stepStr = "STEP ";
	stepStr += intToString(stepNo) + ":";

	string INMStr = "INM: ";
	string INBStr = "INB: ";
	string AIBStr = "AIB: ";
	string LIBStr = "LIB: ";
	string ADBStr = "ADB: ";
	string REBStr = "REB: ";
	string RGFStr = "RGF: ";
	string DAMStr = "DAM: ";

	for (list<InstructionNode*>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		INMStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + (*it)->opOne + "," + (*it)->opTwo + ">,";
	}

	INMStr.pop_back();

	for (list<InstructionNode*>::iterator it = INB.begin(); it != INB.end(); ++it) {
		INBStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + intToString((*it)->opOneVal) + "," + intToString((*it)->opTwoVal) + ">,";
	}

	INBStr.pop_back();

	for (list<InstructionNode*>::iterator it = AIB.begin(); it != AIB.end(); ++it) {
		AIBStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + intToString((*it)->opOneVal) + "," + intToString((*it)->opTwoVal) + ">,";
	}

	AIBStr.pop_back();

	for (list<InstructionNode*>::iterator it = LIB.begin(); it != LIB.end(); ++it) {
		LIBStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + intToString((*it)->opOneVal) + "," + intToString((*it)->opTwoVal) + ">,";
	}

	LIBStr.pop_back();

	for (list<RegNode*>::iterator it = ADB.begin(); it != ADB.end(); ++it) {
		ADBStr += "<" + (*it)->regName + "," + intToString((*it)->data) + ">,";
	}

	ADBStr.pop_back();

	for (list<RegNode*>::iterator it = REB.begin(); it != REB.end(); ++it) {
		REBStr += "<" + (*it)->regName + "," + intToString((*it)->data) + ">,";
	}

	REBStr.pop_back();

	for (int i = 0; i < registers.size(); i++) {
		string currReg = "R" + intToString(i);
		RGFStr += "<" + currReg + "," + intToString(registers[currReg]) + ">,";
	}

	RGFStr.pop_back();

	for (int i = 0; i < dataMem.size(); i++) {
		DAMStr += "<" + intToString(i) + "," + intToString(dataMem[i]) + ">,";
	}

	DAMStr.pop_back();

	file << stepStr << endl;
	file << INMStr << endl;
	file << INBStr << endl;
	file << AIBStr << endl;
	file << LIBStr << endl;
	file << ADBStr << endl;
	file << REBStr << endl;
	file << RGFStr << endl;
	file << DAMStr << endl;
	file << endl;

}


int main()
{
	//initialize data structures
	initRegisters("registers.txt");
	initDataMem("datamemory.txt");
	initInstructions("instructions.txt");
	initOps();

	int step = 0;  //starting at step 0
	ofstream outputFile("./simulation.txt");
	
	printStep(step, outputFile);  //print initial state of the system

	while (!isDone()) {  //while this is true, process items in buffers as needed
		
		processREB();
		processADB();
		processAIB();
		processLIB();
		processINB();
		processINM();

		step++;
		printStep(step, outputFile);
	}

	outputFile.close();

	return 0;
}


