/**
 * @file crc32c.h
 * @brief Function prototypes for hardware-based CRC-32C calculation
 *
 * @author Anand Suresh <anandsuresh@gmail.com>
 */

#include <stdint.h>
#include <stdlib.h>

uint32_t hwCrc32c (uint32_t initialCrc, const char* buf, size_t len);
