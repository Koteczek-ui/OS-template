#ifndef UTIL_IO_DISK_H
#define UTIL_IO_DISK_H

#include <stdint.h>

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_FEATURES     0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_SEL    0x1F6
#define ATA_PRIMARY_COMM_STAT    0x1F7

#define ATA_CMD_READ_PIO         0x20
#define ATA_CMD_WRITE_PIO        0x30
#define ATA_CMD_CACHE_FLUSH      0xE7
#define ATA_CMD_IDENTIFY         0xEC

#define ATA_STAT_BSY             0x80
#define ATA_STAT_DRQ             0x08
#define ATA_STAT_DF              0x20
#define ATA_STAT_ERR             0x01

int disk_read_sector(uint32_t lba, uint8_t *buf);
int disk_write_sector(uint32_t lba, uint8_t *buf);

uint8_t disk_status();

#endif

