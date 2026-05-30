#ifndef UTIL_IO_OPERATION_H
#define UTIL_IO_OPERATION_H

#include "file.h"

uint32_t read_file(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_file(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_file(vfs_node_t *node);
void close_file(vfs_node_t *node);
struct dirent* readdir_file(vfs_node_t *node, uint32_t index);
vfs_node_t* finddir_file(vfs_node_t *node, char *name);

void create_file(const char *path, uint32_t flags);
void create_dir(const char *path);
int del(const char *path);
int ren(const char *oldpath, const char *newpath);
int move_file(const char *src, const char *dest);
int copy_file(const char *src, const char *dest);
int move_dir(const char *src, const char *dest);
int copy_dir(const char *src, const char *dest);
int edit_file(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

#endif

