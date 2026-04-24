#include "disk.h"
#include "io.h"
#include "../str/str.h"
#include "../mem/mem.h"
#include "vfs.h"
#include "operation.h"

static uint32_t next_free_lba = 2048; 

int disk_wait_ready() {
    uint8_t status;
    while ((status = inb(0x1F7)) & 0x80);
    if (status & 0x01) return -1;
    return 0;
}

int disk_read_sector(uint32_t lba, uint8_t *buf) {
    disk_wait_ready();
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F7, 0x20);

    for (int i = 0; i < 256; i++) {
        disk_wait_ready();
        uint16_t data = inw(0x1F0);
        buf[i * 2] = (uint8_t)data;
        buf[i * 2 + 1] = (uint8_t)(data >> 8);
    }
    return 0;
}

int disk_write_sector(uint32_t lba, uint8_t *buf) {
    if (disk_wait_ready() < 0) return -1;
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    for(int i=0; i<4; i++) inb(0x1F7);
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x30);
    if (disk_wait_ready() < 0) return -1;
    while (!(inb(0x1F7) & 0x08));
    for (int i = 0; i < 256; i++) {
        uint16_t data = buf[i * 2] | (buf[i * 2 + 1] << 8);
        outw(0x1F0, data);
    }
    outb(0x1F7, 0xE7);
    return disk_wait_ready();
}

uint32_t hdd_read_internal(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    uint32_t res = 0;
    uint8_t temp[512];
    uint32_t lba = node->lba_start + (offset / 512);
    uint32_t off = offset % 512;
    while (res < size) {
        if (disk_read_sector(lba, temp) < 0) break;
        uint32_t diff = 512 - off;
        if (diff > size - res) diff = size - res;
        memcpy(buffer + res, temp + off, diff);
        res += diff;
        off = 0;
        lba++;
    }
    return res;
}

uint32_t hdd_write_internal(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    uint32_t res = 0;
    uint8_t temp[512];
    uint32_t lba = node->lba_start + (offset / 512);
    uint32_t off = offset % 512;
    while (res < size) {
        disk_read_sector(lba, temp);
        uint32_t diff = 512 - off;
        if (diff > size - res) diff = size - res;
        memcpy(temp + off, buffer + res, diff);
        if (disk_write_sector(lba, temp) < 0) break;
        res += diff;
        off = 0;
        lba++;
    }
    if (offset + res > node->length) node->length = offset + res;
    return res;
}

uint32_t read_file(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node && node->read) return node->read(node, offset, size, buffer);
    return 0;
}

uint32_t write_file(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node && node->write) return node->write(node, offset, size, buffer);
    return 0;
}

int copy_file(const char *src, const char *dest) {
    vfs_node_t *s = vfs_get_path(src);
    if (!s) return -1;
    create_file(dest, 0);
    vfs_node_t *d = vfs_get_path(dest);
    if (!d) return -1;
    uint32_t chunk_size = 4096;
    uint8_t *buf = (uint8_t*)malloc(chunk_size);
    if (!buf) return -1;
    uint32_t progress = 0;
    while (progress < s->length) {
        uint32_t cur = (s->length - progress > chunk_size) ? chunk_size : (s->length - progress);
        read_file(s, progress, cur, buf);
        write_file(d, progress, cur, buf);
        progress += cur;
    }
    free(buf);
    return 0;
}

void create_file(const char *path, uint32_t flags) {
    if (!vfs_root) vfs_init();
    vfs_node_t *node = (vfs_node_t*)malloc(sizeof(vfs_node_t));
    if (!node) return;
    memset(node, 0, sizeof(vfs_node_t));
    const char *name = (path[0] == '/') ? path + 1 : path;
    strcpy(node->name, name);
    node->flags = FS_FILE | flags;
    node->read = hdd_read_internal;
    node->write = hdd_write_internal;
    node->lba_start = next_free_lba;
    next_free_lba += 512; 
    vfs_add_node(vfs_root, node);
}

void create_dir(const char *path) {
    if (!vfs_root) vfs_init();
    vfs_node_t *node = (vfs_node_t*)malloc(sizeof(vfs_node_t));
    if (!node) return;
    memset(node, 0, sizeof(vfs_node_t));
    const char *name = (path[0] == '/') ? path + 1 : path;
    strcpy(node->name, name);
    node->flags = FS_DIRECTORY;
    vfs_add_node(vfs_root, node);
}

int ren(const char *oldpath, const char *newpath) {
    vfs_node_t *n = vfs_get_path(oldpath);
    if (!n) return -1;
    char *name = strrchr(newpath, '/');
    strcpy(n->name, name ? name + 1 : newpath);
    return 0;
}

int del(const char *path) {
    vfs_node_t *node = vfs_get_path(path);
    if (!node) return -1;
    vfs_node_t *parent = vfs_root;
    vfs_node_t *curr = parent->ptr;
    vfs_node_t *prev = 0;
    while (curr) {
        if (curr == node) {
            if (prev) prev->next = curr->next;
            else parent->ptr = curr->next;
            free(curr);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

int move_file(const char *src, const char *dest) {
    if (copy_file(src, dest) == 0) return del(src);
    return -1;
}

int move_dir(const char *src, const char *dest) {
    vfs_node_t *s = vfs_get_path(src);
    if (!s || !(s->flags & FS_DIRECTORY)) return -1;
    create_dir(dest);
    vfs_node_t *d = vfs_get_path(dest);
    if (!d) return -1;
    d->ptr = s->ptr;
    s->ptr = 0; 
    return del(src);
}

int copy_dir(const char *src, const char *dest) {
    vfs_node_t *s = vfs_get_path(src);
    if (!s || !(s->flags & FS_DIRECTORY)) return -1;
    create_dir(dest);
    vfs_node_t *curr = s->ptr;
    while (curr) {
        char new_src[256];
        char new_dest[256];
        strcpy(new_src, src);
        strcat(new_src, "/");
        strcat(new_src, curr->name);
        strcpy(new_dest, dest);
        strcat(new_dest, "/");
        strcat(new_dest, curr->name);
        if (curr->flags & FS_DIRECTORY) copy_dir(new_src, new_dest);
        else copy_file(new_src, new_dest);
        curr = curr->next;
    }
    return 0;
}

struct dirent* readdir_file(vfs_node_t *node, uint32_t index) {
    if (!(node->flags & FS_DIRECTORY) || !node->ptr) return 0;
    vfs_node_t *curr = node->ptr;
    uint32_t i = 0;
    while (curr && i < index) {
        curr = curr->next;
        i++;
    }
    if (!curr) return 0;
    static struct dirent de;
    strcpy(de.name, curr->name);
    de.ino = curr->inode;
    return &de;
}

vfs_node_t* finddir_file(vfs_node_t *node, char *name) {
    if (!(node->flags & FS_DIRECTORY)) return 0;
    return vfs_find(node, name);
}

void open_file(vfs_node_t *node) {
    if (node && node->open) node->open(node);
}

void close_file(vfs_node_t *node) {
    if (node && node->close) node->close(node);
}

int edit_file(vfs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    return (int)write_file(node, offset, size, buffer);
}

