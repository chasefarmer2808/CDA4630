#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

map<string, int> registers;

class InstructionNode {
public:
	string opcode;
	string destReg;
	string opOne;
	string opTwo;
	int opOneVal;
	int opTwoVal;
	InstructionNode* next;
};

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
	ifstream infile(fileName);
	string line;
	
	while (getline(infile, line)) {
		vector<string> currLine = getLineTokens(infile, line);
		registers[currLine[0]] = atoi(currLine[1].c_str());
		cout << "here";
	}
}



int main()
{
	/*
	ifstream infile("instructions.txt");
	string line;

	while (getline(infile, line)) {
		cout << line << endl;
		line.erase(0, 1);
		line.erase(line.length()-1, 1);
		vector<string> temp = split(line, ',');
		cout << "here" << endl;
	}
	*/
	initRegisters("registers.txt");



	return 0;
}


