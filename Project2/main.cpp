#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

map<string, int> instructions;

class DictEntry {
public:
	DictEntry(const uint32_t instruction, const int freq) {
		ins = instruction;
		frequency = freq;
	}

	uint32_t ins;
	int frequency;
};

void countInstructions(string fileName) {
	ifstream infile(fileName.c_str());
	string line;

	while (getline(infile, line)) {
		instructions[line]++;
	}
}

int main() {
	countInstructions("origional.txt");

	return 0;
}