#include "vfs.h"
#include "disk.h"
#include "operation.h"
#include "../str/str.h"
#include "../mem/mem.h"

uint32_t hdd_read_internal(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t hdd_write_internal(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);

#define VFS_METADATA_SECTOR 131072

vfs_node_t *vfs_root = 0;
uint32_t next_free_lba = VFS_METADATA_SECTOR;

void vfs_save_to_disk() {
    if (!vfs_root) return;
    uint8_t buffer[512];
    memset(buffer, 0, 512);

    memcpy(buffer, &next_free_lba, 4);

    vfs_node_t *curr = vfs_root->ptr;
    int offset = 4;
    while (curr && (offset + 44) <= 512) {
        memcpy(buffer + offset, curr->name, 32);
        memcpy(buffer + offset + 32, &curr->lba_start, 4);
        memcpy(buffer + offset + 36, &curr->length, 4);
        memcpy(buffer + offset + 40, &curr->flags, 4);
        offset += 44;
        curr = curr->next;
    }
    disk_write_sector(VFS_METADATA_SECTOR, buffer);
}

void vfs_load_from_disk() {
    uint8_t buffer[512];
    if (disk_read_sector(VFS_METADATA_SECTOR, buffer) < 0) return;

    uint32_t saved_lba;
    memcpy(&saved_lba, buffer, 4);
    
    next_free_lba = saved_lba <= VFS_METADATA_SECTOR ? (VFS_METADATA_SECTOR + 1) : saved_lba;

    int offset = 4;
    while (offset + 44 <= 512) {
        if (buffer[offset] == 0) break;

        vfs_node_t *node = (vfs_node_t*)malloc(sizeof(vfs_node_t));
        memset(node, 0, sizeof(vfs_node_t));
        
        memcpy(node->name, buffer + offset, 32);
        memcpy(&node->lba_start, buffer + offset + 32, 4);
        memcpy(&node->length, buffer + offset + 36, 4);
        memcpy(&node->flags, buffer + offset + 40, 4);
        
        node->read = hdd_read_internal;
        node->write = hdd_write_internal;

        node->next = vfs_root->ptr;
        vfs_root->ptr = node;

        offset += 44;
    }
}

void vfs_init() {
    vfs_root = (vfs_node_t*)malloc(sizeof(vfs_node_t));
    memset(vfs_root, 0, sizeof(vfs_node_t));
    strcpy(vfs_root->name, "/");
    vfs_root->flags = FS_DIRECTORY;
    
    vfs_load_from_disk();
}

void vfs_add_node(vfs_node_t *parent, vfs_node_t *child) {
    if (!parent || !child) return;
    
    if (parent->ptr == 0) {
        parent->ptr = child;
    } else {
        vfs_node_t *curr = parent->ptr;
        while (curr->next) {
            curr = curr->next;
        }
        curr->next = child;
    }
    
    vfs_save_to_disk();
}

vfs_node_t* vfs_find(vfs_node_t *start, const char *name) {
    if (!start || !(start->flags & FS_DIRECTORY)) return 0;
    vfs_node_t *curr = start->ptr;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return 0;
}

vfs_node_t* vfs_get_path(const char *path) {
    if (!path || path[0] != '/') return 0;
    if (strlen(path) == 1) return vfs_root;

    char buf[128];
    vfs_node_t *curr = vfs_root;
    int i = 1, bptr = 0;

    while (path[i]) {
        if (path[i] == '/') {
            buf[bptr] = 0;
            curr = vfs_find(curr, buf);
            if (!curr) return 0;
            bptr = 0;
        } else {
            buf[bptr++] = path[i];
        }
        i++;
    }
    buf[bptr] = 0;
    return vfs_find(curr, buf);
}

