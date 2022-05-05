/**
 * finding_filesystems
 * CS 241 - Spring 2022
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <stdbool.h> 
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string,
             "Free blocks: %zd\n"
             "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // puts("chmod time");
    // Thar she blows!
    inode* i_node = get_inode(fs, path);

    // Check if inode exists
    if (!i_node) {
        errno = ENOENT;
        return -1;
    }
    // Blank out the last 9 bits and then replace them
    i_node->mode = ((i_node->mode >> RWX_BITS_NUMBER) << RWX_BITS_NUMBER) | new_permissions;

    clock_gettime(CLOCK_REALTIME, &i_node->ctim);

    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    // puts("Chown time");
    inode* i_node = get_inode(fs, path);

    // Check if inode exists
    if (!i_node) {
        errno = ENOENT;
        return -1;
    }

    // Change owner
    if (((uid_t)-1) != owner) {
        i_node->uid = owner;
    }

    // Change group
    if (((gid_t)-1) != group) {
        i_node->gid = group;
    }

    clock_gettime(CLOCK_REALTIME, &i_node->ctim);

    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    // puts("Create inode time");
    // Check if inode exists
    if (get_inode(fs, path) || valid_filename(path) != 1) {
        return NULL;
    }

    inode_number i_number = first_unused_inode(fs);
    // Checks if unused inode available
    if (i_number == -1) {
        return NULL;
    }

    const char* filename;
    inode* inode_parent = parent_directory(fs, path, &filename);
    // Check if valid parent
    if (!valid_filename(filename) || !inode_parent || !is_directory(inode_parent)) {
        return NULL;
    }

    // Initialize inode
    init_inode(inode_parent, fs->inode_root + i_number);

    //DIRENT
    minixfs_dirent source;
    source.inode_num = i_number;
    source.name = strdup(filename); // Todo: FREE

    // Gets the index for a new block ex: size = 10 block size = 2 then index 5 would be the new block index in a 0 index list
    int new_block_idx = inode_parent->size / sizeof(data_block);
    size_t new_block_offset = inode_parent->size % sizeof(data_block);

    // Add indirect block if direct are full
    if (new_block_idx >= NUM_DIRECT_BLOCKS && inode_parent->indirect == UNASSIGNED_NODE) {
        if (add_single_indirect_block(fs, inode_parent) == -1) {
            return NULL;
        }
    }

    // Get the block of data
    data_block_number block_number;
    if (new_block_idx < NUM_DIRECT_BLOCKS) {
        block_number = inode_parent->direct[new_block_idx];
    } else {
        data_block_number* indirect_block = (data_block_number*)(fs->data_root + inode_parent->indirect);
        block_number =  indirect_block[new_block_idx - NUM_DIRECT_BLOCKS];
    }
    data_block* curr_block = fs->data_root + block_number;
    make_string_from_dirent(curr_block->data + new_block_offset, source);
    
    inode_parent->size += MAX_DIR_NAME_LEN;

    free(source.name);

    return fs->inode_root + i_number;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    // puts("VIRTUAL TIME");
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        superblock* super_b = fs->meta;

        // Count the number of used blocks
        char* data_map = GET_DATA_MAP(super_b);
        ssize_t num_used_blocks = 0;
        for (size_t i = 0; i < super_b->dblock_count; ++i) {
            if (data_map[i] == 1) {
                ++num_used_blocks;
            }
        }
        // Get entire info string
        char* info_str = block_info_string(num_used_blocks);//todo free?

        // Check if offset is within info_str length
        if ((long)strlen(info_str) < *off) {
            // printf("info len: %lu, off: %lu", (long)strlen(info_str), *off);
            return 0;//todo: -1?
        }

        // Get offset from info string
        char* offset_str = info_str + *off;

        // cap count
        if (count > strlen(offset_str)) {
            count = strlen(offset_str);
        }

        memcpy(buf, offset_str, count);

        return count;
    }

    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    // Requesting to write more bytes than max filesize
    size_t max_file_size = (NUM_DIRECT_BLOCKS + NUM_INDIRECT_BLOCKS) * sizeof(data_block);
    if (count + *off > max_file_size) {
        errno = ENOSPC;
        return -1;
    }
    
    // Creates file if not existing
    // puts("before create");
    minixfs_create_inode_for_path(fs, path);
    // puts("after create");
    inode* i_node = get_inode(fs, path);
    // No more space for i_nodes
    if (!i_node) {
        errno = ENOSPC;
        return -1;
    }

    // Counts the number of blocks needed
    int block_count = (count + *off) / sizeof(data_block);
    size_t remainder_count = (count + *off) % sizeof(data_block);
    // Need an extra block because of remaining data
    if (remainder_count != 0) {
        ++block_count; 
    }
    // Allocates the blocks needed to hold in the data. If failed then err. Shouldn't fail?
    if (minixfs_min_blockcount(fs, path, block_count) == -1) {
        errno = ENOSPC;
        return -1;
    }

    // Gets starting index and offset for write
    int block_index = *off / sizeof(data_block);
    int block_offset = *off % sizeof(data_block);

    // Fills data block by block
    size_t bytes_written = 0;
    while (bytes_written < count) {
        // Get the block of data
        data_block_number block_number;
        if (block_index < NUM_DIRECT_BLOCKS) {
            block_number = i_node->direct[block_index];
        } else {
            data_block_number* indirect_block = (data_block_number*)(fs->data_root + i_node->indirect);
            block_number =  indirect_block[block_index - NUM_DIRECT_BLOCKS];
        }
        data_block* curr_block = fs->data_root + block_number;

        size_t n_bytes = sizeof(data_block) - block_offset;// Write bytes until the end of the block

        // Check if n_bytes is larger than what's left to write. Cap if true.
        if (n_bytes > count - bytes_written) {//todo >=
            n_bytes = count - bytes_written;
        }

        //Write data
        memcpy(curr_block->data + block_offset, buf + bytes_written, n_bytes);

        bytes_written += n_bytes;
        block_offset = 0; // Only non-zero for first iteration?
        ++block_index;
    }

    // Update off
    *off += bytes_written;//count

    // Update size
    if ((unsigned long)*off > i_node->size) {
        i_node->size = *off;
    }

    // Update mtim atim
    clock_gettime(CLOCK_REALTIME, &i_node->mtim);
    clock_gettime(CLOCK_REALTIME, &i_node->atim);

    return count;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    // puts("READ TIME");
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);

    // 'ere be treasure!

    inode* i_node = get_inode(fs, path);
    // Check if file exists
    if (!i_node) {
        errno = ENOENT;
        return -1;
    }
    //Check if off is past file size
    if ((unsigned long)*off >= i_node->size) {
        return 0;
    }

    //Cap the count if larger than the bytes left to read
    if (count >= i_node->size - *off) {
        count = i_node->size - *off;
    }

    // Gets starting index and offset for read
    int block_index = *off / sizeof(data_block);
    int block_offset = *off % sizeof(data_block);

    // reads data block by block
    size_t bytes_read = 0;
    while (bytes_read < count) {
        // Get the block of data
        data_block_number block_number;
        if (block_index < NUM_DIRECT_BLOCKS) {
            block_number = i_node->direct[block_index];
        } else {
            data_block_number* indirect_block = (data_block_number*)(fs->data_root + i_node->indirect);
            block_number =  indirect_block[block_index - NUM_DIRECT_BLOCKS];
        }
        data_block* curr_block = fs->data_root + block_number;

        size_t n_bytes = sizeof(data_block) - block_offset;// Write bytes until the end of the block

        // Check if n_bytes is larger than what's left to write. Cap if true.
        if (n_bytes > count - bytes_read) {//todo >=
            n_bytes = count - bytes_read;
        }

        //Write data
        memcpy(buf + bytes_read, curr_block->data + block_offset, n_bytes);

        bytes_read += n_bytes;
        block_offset = 0; // Only non-zero for first iteration?
        ++block_index;
    }
    // Update off
    *off += bytes_read;

    // Update mtim atim
    clock_gettime(CLOCK_REALTIME, &i_node->atim);

    return bytes_read;
}