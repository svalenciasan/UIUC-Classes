#pragma once

#include <string>
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;

// adapted from CS 225 Final Project AMA

/** 
 * Reads a file into a string
 * @param filename The file to read from 
 * @return a string containing filename's contents
 */
std::string file_to_string(const std::string & filename);

/** 
 * Reads a file into a vector
 * @param filename The file to read from
 * @return a vector containing filename's contents
 */
std::vector<std::string> file_to_vector(const std::string & filename);