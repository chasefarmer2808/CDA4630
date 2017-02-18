#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

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
vector<string> aluOps;
list<InstructionNode*> instructions;
list<InstructionNode*> INB;
list<InstructionNode*> AIB;
list<InstructionNode*> LIB;
list<RegNode*> ADB;
list<RegNode*> REB;



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
		instructions.push_back(temp);
	}

	return;
}

void initOps() {
	aluOps.push_back("ADD");
	aluOps.push_back("AND");
	aluOps.push_back("SUB");
	aluOps.push_back("OR");
}

bool isDone() {
	int size = 0;

	size = instructions.size() + INB.size() + AIB.size() + LIB.size() + ADB.size() + REB.size();

	if (size > 0) {
		return false;
	}

	return true;
}

string intToString(int n) {
	string res;
	ostringstream convert;
	convert << n;
	res = convert.str();

	return res;
}

bool isALUOP(const string &op, const vector<string> &array) {
	return std::find(array.begin(), array.end(), op) != array.end();
}

void processINM() {
	if (instructions.size() > 0) {
		InstructionNode* temp = instructions.front();
		instructions.pop_front();
		INB.push_back(temp);
	}

	return;
}

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

void processREB() {
	if (REB.size() > 0) {
		RegNode* temp = REB.front();
		registers[temp->regName] = temp->data;
		REB.pop_front();
	}

	return;
}

void printStep(int stepNo) {
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

	for (list<InstructionNode*>::iterator it = INB.begin(); it != INB.end(); ++it) {
		INBStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + intToString((*it)->opOneVal) + "," + intToString((*it)->opTwoVal) + ">,";
	}

	for (list<InstructionNode*>::iterator it = AIB.begin(); it != AIB.end(); ++it) {
		AIBStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + intToString((*it)->opOneVal) + "," + intToString((*it)->opTwoVal) + ">,";
	}

	for (list<InstructionNode*>::iterator it = LIB.begin(); it != LIB.end(); ++it) {
		LIBStr += "<" + (*it)->opcode + "," + (*it)->destReg + "," + intToString((*it)->opOneVal) + "," + intToString((*it)->opTwoVal) + ">,";
	}

	for (list<RegNode*>::iterator it = ADB.begin(); it != ADB.end(); ++it) {
		ADBStr += "<" + (*it)->regName + "," + intToString((*it)->data) + ">,";
	}

	for (list<RegNode*>::iterator it = REB.begin(); it != REB.end(); ++it) {
		REBStr += "<" + (*it)->regName + "," + intToString((*it)->data) + ">,";
	}

	for (int i = 0; i < registers.size(); i++) {
		string currReg = "R" + intToString(i);
		RGFStr += "<" + currReg + "," + intToString(registers[currReg]) + ">,";
	}

	for (int i = 0; i < dataMem.size(); i++) {
		DAMStr += "<" + intToString(i) + "," + intToString(dataMem[i]) + ">,";
	}

	cout << stepStr << endl;
	cout << INMStr << endl;
	cout << INBStr << endl;
	cout << AIBStr << endl;
	cout << LIBStr << endl;
	cout << ADBStr << endl;
	cout << REBStr << endl;
	cout << RGFStr << endl;
	cout << DAMStr << endl;
	cout << endl;

}


int main()
{
	initRegisters("registers.txt");
	initDataMem("datamemory.txt");
	initInstructions("instructions.txt");
	initOps();

	int step = 0;

	printStep(step);

	while (!isDone()) {
		//do stuff

		processREB();
		processADB();
		processAIB();
		processLIB();
		processINB();
		processINM();

		step++;
		printStep(step);
	}

	return 0;
}


