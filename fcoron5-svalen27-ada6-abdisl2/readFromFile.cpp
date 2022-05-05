#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

#include "readFromFile.h"

using std::cout;
using std::endl;
using std::cin;
using std::string;
using std::remove;
using std::ifstream;
using std::vector;

string file_to_string(const string & filename) {
	ifstream file(filename);
	string str;
	string file_contents;
	while (getline(file, str)) {
		str.erase(str.find_last_not_of(" \n\r\t")+1);
		file_contents += str;
		file_contents.push_back('\n');
	}
	file_contents.pop_back();
	file.close();
	return file_contents;
}

vector<string> file_to_vector(const string & filename) {
	ifstream file(filename);
	string str;
	vector<string> file_contents;
	while (getline(file, str)) {
		str.erase(str.find_last_not_of(" \n\r\t")+1);
		file_contents.push_back(str);
	}
	file.close();
	return file_contents;
} 