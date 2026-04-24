#ifndef UTIL_IO_VFS_H
#define UTIL_IO_VFS_H

#include "file.h"

extern vfs_node_t *vfs_root;

void vfs_init();
vfs_node_t* vfs_find(vfs_node_t *start, const char *name);
vfs_node_t* vfs_get_path(const char *path);
void vfs_mount(const char *path, vfs_node_t *local_root);
void vfs_add_node(vfs_node_t *parent, vfs_node_t *child);

#endif

