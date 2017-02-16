#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>

using namespace std;

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

class RegNode {
public:
	RegNode(const string reg, const int val) {
		regName = reg;
		data = val;
	}

	string regName;
	int data;
};

map<string, int> registers;
vector<int> dataMem;
queue<InstructionNode*> instructions;
queue<InstructionNode*> INB;
queue<InstructionNode*> AIB;
queue<InstructionNode> LIB;
queue<RegNode*> ADB;
queue<RegNode*> REB;



vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;

	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}

	return tokens;
}

vector<string> getLineTokens(ifstream& file, string line) {
	line.erase(0, 1);
	line.erase(line.length() - 1, 1);
	vector<string> ret = split(line, ',');

	return ret;
}

void initRegisters(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	
	while (getline(infile, line)) {
		vector<string> currLine = getLineTokens(infile, line);
		registers[currLine[0]] = atoi(currLine[1].c_str());
	}

	return;
}

void initDataMem(string fileName) {
	ifstream infile(fileName.c_str());
	string line;

	while (getline(infile, line)) {
		vector<string> currLine = getLineTokens(infile, line);
		dataMem.push_back(atoi(currLine[1].c_str()));
	}

	return;
}

void initInstructions(string fileName) {
	ifstream infile(fileName.c_str());
	string line;

	while (getline(infile, line)) {
		vector<string> currLine = getLineTokens(infile, line);
		int oneVal = registers[currLine[2]];
		int twoVal = registers[currLine[3]];
		InstructionNode* temp = new InstructionNode(currLine[0], currLine[1], currLine[2], currLine[3], oneVal, twoVal);
		instructions.push(temp);
	}

	return;
}



int main()
{
	initRegisters("registers.txt");
	initDataMem("datamemory.txt");
	initInstructions("instructions.txt");

	return 0;
}


