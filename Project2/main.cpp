#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class DictEntry {
public:
	DictEntry(const string instruction, const int freq) {
		ins = instruction;
		frequency = freq;
	}

	string ins;
	int frequency;
};

class InsEntry {
public:
	InsEntry(const int freq, const int index) {
		frequency = freq;
		firstOccurance = index;
	}

	int frequency;
	int firstOccurance;
};

map<string, int> allInstructions;
//map<string, int> instructions;
map<string, InsEntry*> instructions;
vector<DictEntry*> dictionary;

void countInstructions(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	int i = 0;

	while (getline(infile, line)) {
		if (instructions.count(line) > 0) {  //not a new instruction
			instructions[line]->frequency++;
		}
		else {
			instructions[line] = new InsEntry(1, i);
		}
		i++;
		//instructions[line]++;
		//allInstructions[line] = i;
		//i++;
		//instructions[line]->frequency++;
		//instructions[line]->instruction.push_back(line);
	}
}

string findMostFrequent() {
	int max = 0;
	int occurance = 0;
	string key;

	for (map<string, InsEntry*>::iterator it = instructions.begin(); it != instructions.end(); ++it) {
		if (it->second->frequency == max) {  //if we get the same frequency
			//need to compare first occurances
			if (occurance > it->second->firstOccurance) {  //if the last thing we put in occured after the current thing
				key = it->first;  //set the current thing as the chosen max key
				occurance = it->second->firstOccurance;  //set the new earliest occurance
			}
		}
		else if (it->second->frequency > max) {  //found new max
			max = it->second->frequency;  //set the new max
			occurance = it->second->firstOccurance;
			key = it->first;
		}
		/*
		if (it->second >= max) {
			max = it->second;
			key = it->first;
		}
		*/
	}

	return key;
}

void initDict(int size) {
	string currEntry;

	if (size > instructions.size()) {
		size = instructions.size(); //accounts for when all instructions can fit in dictionary
	}

	for (int i = 0; i < size; i++) {
		currEntry = findMostFrequent();
		DictEntry* entry = new DictEntry(currEntry, instructions[currEntry]->frequency);
		instructions.erase(currEntry);
		dictionary.push_back(entry);
	}
}

int main() {
	countInstructions("origional.txt");
	initDict(16);

	return 0;
}