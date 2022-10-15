/**
 * @file
 * @brief Build configuration of MaxMindDB library.
 */

#ifndef MAXMINDDB_CONFIG_H
#define MAXMINDDB_CONFIG_H

#define PACKAGE_VERSION "1.3.2"

#if __SIZEOF_INT128__ == 16
#define MMDB_UINT128_IS_BYTE_ARRAY 0  // use __int128
#else
#define MMDB_UINT128_IS_BYTE_ARRAY 1
#endif

// well, Crysis runs only on x86 processors - always little-endian :)
#define MMDB_LITTLE_ENDIAN 1

#endif  // MAXMINDDB_CONFIG_H
