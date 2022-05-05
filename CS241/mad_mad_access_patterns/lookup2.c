/**
 * mad_mad_access_patterns
 * CS 241 - Spring 2022
 */
#include "tree.h"
#include "utils.h"

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

// static int file_fd = NULL;
static void* mmap_addr = NULL;

// Helper Functions
bool findWord(char* word, long offset);

int main(int argc, char **argv) {
  //  Check correct input
  if (argc < 3) {
    printArgumentUsage();
    exit(1);
  }
  // Open file
  int file_fd = open(argv[1], O_RDONLY);
  if (file_fd == -1) {
    openFail(argv[1]);
    exit(2);
  }

  // Get size
  struct stat file_stat;
  // memset(&file_stat, 0, sizeof(file_stat));
  if (fstat(file_fd, &file_stat) == -1) {exit(EXIT_FAILURE);}

  // MMap
  mmap_addr = mmap(0, file_stat.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
  if (mmap_addr == MAP_FAILED) {
    mmapFail(argv[1]);
    exit(3);
  }

  // Check if file starts with BTRE
  if (strncmp(mmap_addr, BINTREE_HEADER_STRING, 4) != 0) {
    formatFail(argv[1]);
    exit(2);
  }

  // Look through the words
  for (size_t i = 2; i < (size_t)argc; ++i) {
    if (!findWord(argv[i], 4)) {printNotFound(argv[i]);}
  }

  // Cleanup
  munmap(mmap_addr, file_stat.st_size);
  close(file_fd);

  return 0;
}

bool findWord(char* word, long offset) {
  // No child
  if (offset == 0) {return false;}

  // Reads first node
  BinaryTreeNode* node = mmap_addr + offset;

  // printf("word:%s.\n", node_word);

  int cmp = strcmp(node->word, word);
  if (cmp == 0) {
    printFound(word, node->count, node->price);
    return true;
  } else if (cmp > 0) {
    // printf("wordLeft:%s.\n", node_word);
    if (findWord(word, node->left_child)) {return true;}
  } else {
    // printf("wordRight:%s.\n", node_word);
    if (findWord(word, node->right_child)) {return true;}
  }
  // printf("wordFailed:%s.\n", node_word);
  return false;
}