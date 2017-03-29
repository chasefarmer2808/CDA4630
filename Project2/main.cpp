#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <math.h>

#define RLELIMIT 9.0
#define INSSIZE 32
#define OPTCOUNT 7

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

class CompEntry {
public:
	CompEntry(const string ins) {
		instruction = ins;
		option = "";
		format = "";
	}

	string instruction;
	string option;
	string format;
};

class DecompEntry {
public:
	DecompEntry(const string opt, const string form) {
		option = opt;
		format = form;
	}

	string option;
	string format;
};

class OptionResult {
public:
	OptionResult(const string opt, const int i) {
		option = opt;
		index = i;
	}

	string result;
	string option;
	int index;
};

map<string, InsEntry*> instructions;
map<string, int> formatBitCounts;
vector<DictEntry*> dictionary;
vector<CompEntry*> compInstructions;
vector<DecompEntry*> decompInstructions;
vector<string> origionalInstructions;
string optionsByPriority[] = { "111", "011", "100", "101", "010", "110", "000" };
int optionFormatCounts[] = { 4, 9, 9, 9, 13, 14, 32 };

int getFileLineCount(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	int count = 0;

	while (getline(infile, line)) {
		count++;
	}

	return count;
}

int getDictStartNum(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	int count = 0;

	while (getline(infile, line)) {
		count++;

		if (line == "xxxx") {
			return count;  //returns the line number of the first dictionary entry
		}
	}
}

//C++98 helper function for casting an int to a string.  Useful for printing in for loop, and hash map lookups
string intToString(int n) {
	string res;
	ostringstream convert;
	convert << n;
	res = convert.str();

	return res;
}

int getFirstSetFromLeft(string bits) {
	for (int i = 0; i < bits.length(); i++) {
		if (bits[i] == '1') {
			return i;
		}
	}
}

int getSecondSetFromLeft(string bits) {
	bool foundFirst = false;

	for (int i = 0; i < bits.length(); i++) {
		if (bits[i] == '1' && foundFirst == false) {
			foundFirst = true;
			continue;
		}

		if (bits[i] == '1') {
			return i;
		}
	}
}

string doBitmask(string ins) {
	bitset<INSSIZE> instruction(ins);
	int startLoc = getFirstSetFromLeft(ins);
	bitset<5> startBits(startLoc);
	string format = "";
	string bitmask = "1";
	string start = startBits.to_string();

	startLoc++;

	for (int i = startLoc; i < (startLoc + 3); i++) {
		bitmask += ins[i];
	}

	format = start + bitmask;
	 
	return format;
}

string doNBitMismatch(string ins) {
	int startLoc = getFirstSetFromLeft(ins);
	bitset<5> startBits(startLoc);
	return startBits.to_string();
}

string doTwoMismatchAnywhere(string ins) {
	int first = getFirstSetFromLeft(ins);
	int second = getSecondSetFromLeft(ins);
	bitset<5> firstBits(first);
	bitset<5> secondBits(second);

	string format = firstBits.to_string() + secondBits.to_string();

	return format;
}

string getCompFormat(string opt, string result, int dictIndex) {
	string format = "";
	bitset<4> dIndex(dictIndex);

	if (opt == "010") {
		format = doBitmask(result);
	}
	else if (opt == "011" || opt == "100" || opt == "101") {
		format = doNBitMismatch(result);
	}
	else if (opt == "110") {
		format = doTwoMismatchAnywhere(result);
	}

	format += dIndex.to_string();

	return format;
}

string intToBitString(int num, const int numOfBits) {
	string bitString = "";
	int MSB = 1 << (numOfBits - 1);
	int bit;

	for (int i = 0; i < numOfBits; i++) {
		bit = num & MSB;

		if (bit > 0) {
			bitString += "1";
		}
		else {
			bitString += "0";
		}
		
		num = num << 1;
	}

	return bitString;
}

void countInstructions(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	int i = 0;

	while (getline(infile, line)) {
		compInstructions.push_back(new CompEntry(line));
		if (instructions.count(line) > 0) {  //not a new instruction
			instructions[line]->frequency++;
		}
		else {
			instructions[line] = new InsEntry(1, i);
		}
		i++;
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
	}

	return key;
}

void initFormatBitCounts() {
	formatBitCounts["001"] = 3;
	
	for (int i = 0; i < 7; i++) {
		formatBitCounts[optionsByPriority[i]] = optionFormatCounts[i];
	}
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

OptionResult* getBestOption(vector<OptionResult*> ops) {
	for (int i = 0; i < OPTCOUNT; i++) {
		for (int j = 0; j < ops.size(); j++) {
			if (optionsByPriority[i] == ops[j]->option) { //found the best option
				return ops[j];
			}
		}
	}
}

string getAllCompBits(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	string result = "";

	while (getline(infile, line)) {
		if (line == "xxxx") {
			return result;
		}
		result += line;
	}
}

void getDict(string fileName) {
	ifstream infile(fileName.c_str());
	string line;
	int index = 0;
	int currLineNum = 0;
	int lineCount = getFileLineCount(fileName);
	int start = getDictStartNum(fileName);

	while (getline(infile, line)) {
		if (currLineNum >= start) {
			DictEntry* entry = new DictEntry(line, 0);
			dictionary.push_back(entry);
		}
		currLineNum++;
	}
}

void getDecompFormat(string fileName) {
	string allBits = getAllCompBits(fileName);
	string option;
	string format;

	for (int i = 0; i < allBits.size(); i++) {
		option = allBits.substr(i, 3);
		i += 3;
		format = allBits.substr(i, formatBitCounts[option]);
		DecompEntry* entry = new DecompEntry(option, format);
		decompInstructions.push_back(entry);
		i += (formatBitCounts[option] - 1);
	}

	if (decompInstructions[decompInstructions.size() - 1]->option == "000" && decompInstructions[decompInstructions.size() - 1]->format.length() != 32) {
		decompInstructions.erase(decompInstructions.end() - 1);
	}
}

int getMismatchDistance(bitset<INSSIZE> bits) {
	int first = 0;
	int last = 0;

	for (int i = (INSSIZE - 1); i >= 0; i--) {
		if (bits[i] == 1 && first == 0) {
			first = i;
		}
		else if (bits[i] == 1) {
			last = i;
		}
	}

	return (first - last - 1);  //return number of bits between mismatches
}

bool isConsecutive(bitset<INSSIZE> bits, int mismatches) {
	bitset<INSSIZE> temp(bits << 1);

	for (int i = 0; i < mismatches; i++) {
		if ((bits & temp) == 0) {
			return false;
		}
		temp = temp << 1;
	}
	return true;
}

int getRLERange(string ins, int start) {
	int end = 0;

	while (compInstructions[start]->instruction == ins) {  //count up the repeated instruction

		end = start;
		start++;


		if (start == (compInstructions.size())) {  //this is true if the last instruciton is part of an RLE
			break;
		}
	}

	return end;
}

void doCompression(string ins, int insIndex) {
	vector<OptionResult*> availOptions;
	OptionResult* bestOpt;

	int mismatches = 0;
	int distance = -1;
	bool consecutive = false;

	bitset<INSSIZE> insBits(ins);

	for (int i = 0; i < dictionary.size(); i++) { //for each entry in the dictionsry
		OptionResult* optRes = new OptionResult("000", i);
		bitset<INSSIZE> dictBits(dictionary[i]->ins);
		bitset<INSSIZE> result(insBits ^ dictBits);
		mismatches = result.count();
		optRes->result = result.to_string();

		if (mismatches > 1) {
			distance = getMismatchDistance(result);
			consecutive = isConsecutive(result, mismatches);
		}

		if (mismatches == 0) {
			optRes->option = "111";
		}
		else if (mismatches == 1) {
			optRes->option = "011";
		}
		else if (mismatches == 2 && consecutive) {
			optRes->option = "100";
		}
		else if (mismatches == 4 && consecutive) {
			optRes->option = "101";
		}
		else if (mismatches > 1 && distance <= 2) {
			optRes->option = "010";
		}
		else if (mismatches == 2 && distance > 2) {
			optRes->option = "110";
		}
		else {
			optRes->option = "000";
		}
		availOptions.push_back(optRes);
	}

	bestOpt = getBestOption(availOptions);
	compInstructions[insIndex]->option = bestOpt->option;

	if (bestOpt->option == "000") {
		compInstructions[insIndex]->format = compInstructions[insIndex]->instruction;
	}
	else {
		compInstructions[insIndex]->format = getCompFormat(bestOpt->option, bestOpt->result, bestOpt->index);
	}
}

void doRLE(string ins, int start, int end) {
	int count = end - start + 1;
	int groups = ceil((float)count / RLELIMIT);
	int firstInGroup = 0;
	int lastInGroup = 0;

	if (groups == 1) {
		firstInGroup = start;
		lastInGroup = end;

		doCompression(compInstructions[firstInGroup]->instruction, firstInGroup);

		compInstructions[lastInGroup]->option = "001";
		compInstructions[lastInGroup]->format = intToBitString((count - 2), 3);
		return;
	}

	for (int i = 0; i < groups; i++) {  //for each group
		firstInGroup = start + (i * 9);
		
		if (groups - i == 1) {
			//on the last group
			lastInGroup = end;
		}
		else {
			lastInGroup = firstInGroup + 8;
		}

		doCompression(compInstructions[firstInGroup]->instruction, firstInGroup);

		if (firstInGroup != lastInGroup) {  //group has more than one ins
			int currCount = lastInGroup - firstInGroup + 1;

			compInstructions[lastInGroup]->option = "001";
			compInstructions[lastInGroup]->format = intToBitString((currCount - 2), 3);
		}
	}

	return;
}

void compress() {
	string prevIns;
	int RLEstart = 0;
	int RLEend = 0;

	for (int i = 0; i < compInstructions.size(); i++) { //for all instructions except the last

		if (i > 0) {  //only start checking the last instruction after we have looped once
			prevIns = compInstructions[i-1]->instruction;
			RLEstart = i - 1;
		}

		if (compInstructions[i]->instruction == prevIns) {  //start of new RLE application
			RLEend = getRLERange(compInstructions[i]->instruction, i);  //get the index of the last repeated instruction
			doRLE(compInstructions[i]->instruction, RLEstart, RLEend);  //perform RLE on the repeated instructions
			i = RLEend;  //go the the next instruction after the repeated instructions
			continue;
		}
		
		doCompression(compInstructions[i]->instruction, i);
	}

	//should be done compressing everything at this point

	return;
}

void printCompressed(ofstream &file) {
	int lineCount = 0;
	string compString;

	for (int i = 0; i < compInstructions.size(); i++) {
		if (compInstructions[i]->format.size()) {
			compString = compInstructions[i]->option + compInstructions[i]->format;

			for (int j = 0; j < compString.size(); j++) {
				file << compString[j];
				lineCount++;

				if (lineCount == 32) {
					lineCount = 0;
					file << endl;
				}

			}
		}
	}

	int pad = 32 - lineCount;

	for (int i = 0; i < pad; i++) {
		file << '0';
	}

	file << endl;

	file << "xxxx" << endl;

	for (int i = 0; i < dictionary.size(); i++) {
		file << dictionary[i]->ins << endl;
	}
}

int main(int argc, char* argv[]) {
	/*
	countInstructions("origional.txt");
	initDict(16);
	compress();

	ofstream outputFile("./compressed.txt");
	printCompressed(outputFile);
	outputFile.close();
	*/

	initFormatBitCounts();
	getDict("compressedtest.txt");
	getDecompFormat("compressedtest.txt");

	return 0;
}