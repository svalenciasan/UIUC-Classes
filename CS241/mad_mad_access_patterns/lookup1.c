/**
 * mad_mad_access_patterns
 * CS 241 - Spring 2022
 */
#include "tree.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/
static FILE* file = NULL;

// Helper Functions
bool findWord(char* word, long offset);


int main(int argc, char **argv) {
  //  Check correct input
  if (argc < 3) {
    printArgumentUsage();
    exit(1);
  }

  file = fopen(argv[1], "r");
  if (!file) {
    openFail(argv[1]);
    exit(2);
  }

  // Check if file starts with BTRE
  char valid_check[4];
  if (fread(valid_check, 1, 4, file) < 4) {exit(EXIT_FAILURE);}
  if (strcmp(valid_check, BINTREE_HEADER_STRING) != 0) {
    formatFail(argv[1]);
    exit(2);
  }

  // Look through the words
  for (size_t i = 2; i < (size_t)argc; ++i) {
    if (!findWord(argv[i], 4)) {printNotFound(argv[i]);}
  }

  // Cleanup
  fclose(file);

  return 0;
}

bool findWord(char* word, long offset) {
  // No child
  if (offset == 0) {return false;}

  // Move file pointer to current node
  fseek(file, offset, SEEK_SET);

  // Reads first node
  BinaryTreeNode node;
  fread(&node, sizeof(BinaryTreeNode), 1, file);

  // Gets word
  ssize_t word_size = 0;
  while (fgetc(file) != 0) {
    ++word_size;
  }
  // Counts 0
  ++word_size;

  // Move file pointer back
  fseek(file, -1 * word_size, SEEK_CUR);

  // Read word
  char node_word[word_size];
  fread(node_word, 1, word_size, file);
  // printf("word:%s.\n", node_word);

  int cmp = strcmp(node_word, word);
  if (cmp == 0) {
    printFound(word, node.count, node.price);
    return true;
  } else if (cmp > 0) {
    // printf("wordLeft:%s.\n", node_word);
    if (findWord(word, node.left_child)) {return true;}
  } else {
    // printf("wordRight:%s.\n", node_word);
    if (findWord(word, node.right_child)) {return true;}
  }
  // printf("wordFailed:%s.\n", node_word);
  return false;
}