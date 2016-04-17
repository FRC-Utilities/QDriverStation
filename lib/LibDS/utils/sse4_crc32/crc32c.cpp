/**
 * @file crc32c.cpp
 * @brief Node.js bindings for CRC-32C calculation using hardware-acceleration, when available.
 *
 * The code below provides the bindings for the node-addon allowing for interfacing of C/C++ code with
 * JavaScript. It chooses between two versions of the CRC-32C calculator:
 * - The hardware-accelerated version that uses Intel's SSE 4.2 instructions, implemented in crc32c_sse42.cpp
 * - A table-lookup based CRC calculated implemented in software for non-Nehalam-based architectures
 *
 * NOTES:
 * - This code, though originally designed for little-endian hardware, should work for all platforms.
 * - Table-based CRC-32C implementation based on code by Mark Adler at http://stackoverflow.com/a/17646775.
 *
 * @author Anand Suresh <anandsuresh@gmail.com>
 */

#include "crc32c.h"

// Bit-mask for the SSE 4.2 flag in the CPU ID
#define SSE4_2_FLAG         0x100000

// The CRC-32C polynomial in reversed bit order
#define CRC32C_POLYNOMIAL   0x82f63b78

// Stores the CRC-32 lookup table for the software-fallback implementation
static uint32_t crc32cTable[8][256];

/**
 * Cross-platform CPU feature set detection to check for availability of hardware-based CRC-32C
 */
void cpuid (uint32_t op, uint32_t reg[4]) {
#if defined(_WIN64) || defined(_WIN32)
#include <intrin.h>
    __cpuid ((int*)reg, 1);
#elif defined(__x86_64__)
    __asm__ volatile (
        "pushq %%rbx       \n\t"
        "cpuid             \n\t"
        "movl  %%ebx, %1   \n\t"
        "popq  %%rbx       \n\t"
        : "=a" (reg[0]), "=r" (reg[1]), "=c" (reg[2]), "=d" (reg[3])
        : "a" (op)
        : "cc");
#elif defined(__i386__)
    __asm__ volatile (
        "pushl %%ebx       \n\t"
        "cpuid             \n\t"
        "movl  %%ebx, %1   \n\t"
        "popl  %%ebx       \n\t"
        : "=a" (reg[0]), "=r" (reg[1]), "=c" (reg[2]), "=d" (reg[3])
        : "a" (op)
        : "cc");
#else
    reg[0] = reg[1] = reg[2] = reg[3] = 0;
#endif
}


/**
 * Returns whether or not Intel's Streaming SIMD Extensions 4.2 is available on the hardware
 *
 * @return true if Intel's Streaming SIMD Extensions 4.2 are present; otherwise false
 */
bool isSSE42Available() {
    uint32_t reg[4];

    cpuid (1, reg);
    return ((reg[2] >> 20) & 1) == 1;
}


/**
 * Initializes the CRC-32C lookup table for software-based CRC calculation
 */
void initCrcTable() {
    uint32_t i, j, crc;

    for (i = 0; i < 256; i++) {
        crc = i;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc = crc & 1 ? (crc >> 1) ^ CRC32C_POLYNOMIAL : crc >> 1;
        crc32cTable[0][i] = crc;
    }

    for (i = 0; i < 256; i++) {
        crc = crc32cTable[0][i];
        for (j = 1; j < 8; j++) {
            crc = crc32cTable[0][crc & 0xff] ^ (crc >> 8);
            crc32cTable[j][i] = crc;
        }
    }
}


/**
 * Calculates CRC-32C using the lookup table
 *
 * @param initialCrc The initial CRC to use for the operation
 * @param buf The buffer that stores the data whose CRC is to be calculated
 * @param len The size of the buffer
 * @return The CRC-32C of the data in the buffer
 */
uint32_t swCrc32c (uint32_t initialCrc, const char* buf, size_t len) {
    const char* next = buf;
    uint64_t crc = initialCrc;


    // If the string is empty, return 0
    if (len == 0) return (uint32_t)crc;

    // XOR the initial CRC with INT_MAX
    crc ^= 0xFFFFFFFF;

    // Process byte-by-byte until aligned to 8-byte boundary
    while (len && ((uintptr_t) next & 7) != 0) {
        crc = crc32cTable[0][ (crc ^ *next++) & 0xff] ^ (crc >> 8);
        len--;
    }

    // Process 8 bytes at a time
    while (len >= 8) {
        crc ^= * (uint64_t*) next;
        crc = crc32cTable[7][ (crc >>  0) & 0xff] ^ crc32cTable[6][ (crc >>  8) & 0xff]
              ^ crc32cTable[5][ (crc >> 16) & 0xff] ^ crc32cTable[4][ (crc >> 24) & 0xff]
              ^ crc32cTable[3][ (crc >> 32) & 0xff] ^ crc32cTable[2][ (crc >> 40) & 0xff]
              ^ crc32cTable[1][ (crc >> 48) & 0xff] ^ crc32cTable[0][ (crc >> 56)];
        next += 8;
        len -= 8;
    }

    // Process any remaining bytes
    while (len) {
        crc = crc32cTable[0][ (crc ^ *next++) & 0xff] ^ (crc >> 8);
        len--;
    }

    // XOR again with INT_MAX
    return (uint32_t) (crc ^= 0xFFFFFFFF);
}
