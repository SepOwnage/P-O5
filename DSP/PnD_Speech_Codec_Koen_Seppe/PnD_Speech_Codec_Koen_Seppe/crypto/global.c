/* Mathijs Cuppens & Robbe Van Rompaey
 * PnD Crypto 2016
 *
 * global.c
 *
 * This file contains declaration of global variables
 */

#include "global.h"

//char are all in Little-endian!!!

#if (SEL == 0)

/// FINAL PARAMETERS STS_PROTOCOL

unsigned short MODULUS_MASTER[MODULUS_SIZE_MASTER] =
{0xdf83,0x0555,0xf186,0x5f4c,0x4eab,0x41a7,0x4665,0x555d,
0x8f09,0xa85b,0x5091,0xd41c,0xf04b,0x2251,0xb0b0,0xb4c1,
0x1563,0xf58f,0xbe3b,0x87e3,0x20f2,0x4751,0xaac9,0xcd2f,
0x3cda,0xb6a5,0x4ab2,0x91da,0x7377,0xf6b4,0x11ad,0x99b5,
0x3e02,0xc1aa,0x6ad9,0x6615,0x2491,0x00b6,0x048c,0xd25d,
0x77e1,0xaa5c,0x5883,0x0c00,0x583d,0x189f,0x9c87,0xf9b1,
0xc144,0x8674,0x9e38,0xbc61,0x3c26,0xa6e5,0x2d31,0xee53,
0x534e,0x012b,0xfc8c,0xda52,0x19a7,0xefaf,0x31cb,0x099e,
0x1b21,0x5e15,0x07c2,0x4213,0xe93c,0x03af,0x2503,0x2cb5,
0x6580,0x59a3,0x2f99,0x03e9,0x9191,0xb220};

unsigned short PUBLIC_EXPONENT_MASTER[PUBLIC_SIZE_MASTER] =
{0x0001, 0x0001};

unsigned short PRIVATE_EXPONENT_MASTER[PRIVATE_SIZE_MASTER] =
{0xd3c1,0xb5d1,0xf903,0x0655,0x5b6e,0x41bf,0x6097,0x2bbd,
0x8801,0xaab7,0xbca1,0xac02,0x015a,0x08ad,0x5ffb,0xad3f,
0x6a55,0xdcc3,0x3fc5,0xfd11,0xa9bc,0x4668,0xd902,0xc2c5,
0xbd3f,0x78c0,0xcbcb,0x9901,0x71e5,0xd451,0xbd24,0xcbc7,
0x4bda,0x22cd,0x2cab,0x9ec3,0x38ff,0xc0e7,0xf9d0,0x04eb,
0x0d5f,0x92b0,0x15cd,0x280d,0xaef7,0x4f73,0xe142,0x9e60,
0xfd36,0x73d8,0xecc0,0x9ff4,0x10a4,0x505d,0xe6c5,0xf700,
0x3030,0xc4b0,0x13d4,0xd989,0x5b81,0xdb4d,0xc551,0xadc9,
0xbecc,0xaf91,0x46d9,0xd44c,0x969c,0xc635,0xf610,0x42a2,
0xb071,0x03d2,0xdd01,0xd124,0xf004,0x3c13};

unsigned short MODULUS_SLAVE[MODULUS_SIZE_SLAVE] =
{0x86f7,0x07a4,0x2b53,0x67eb,0x5f4f,0x0c06,0xf83b,0xdb6a,
0xea72,0x09c5,0xd4c7,0x420a,0xd251,0xa9e4,0x403c,0x63b7,
0xff44,0x1d00,0x1196,0xe719,0xeccf,0x940a,0x1957,0xf609,
0x1394,0x8ffc,0x6e5a,0x4434,0x8e24,0xc0ff,0xff7a,0xe920,
0x7181,0xc33c,0xc91a,0xa3e2,0xf13a,0x5808,0x6504,0xcd42,
0x46c0,0x6012,0xe48f,0xb64e,0x76e5,0x8d4a,0xf46c,0xafc3,
0xad86,0x15ff,0xa630,0x1673,0xb96e,0x35c9,0xafe5,0x6ddd,
0x516f,0x3609,0x3035,0xa0e7,0xc30f,0x3331,0x1f54,0x7a8c,
0x636a,0xb2ba,0x6f41,0x0105,0xba8d,0x2caa,0x4878,0x6f56,
0x6631,0x1309,0x9d96,0xe408,0xf48f,0xa279};

unsigned short PUBLIC_EXPONENT_SLAVE[PUBLIC_SIZE_SLAVE] =
{0x0001, 0x0001};

unsigned short PRIVATE_EXPONENT_SLAVE[PRIVATE_SIZE_SLAVE] =
{0x5a41,0x6df6,0x83eb,0xfd2a,0xb965,0xdeb0,0xe6b1,0xf17a,
0x63ce,0x8517,0x194f,0xffc3,0xc823,0xda68,0xfcf7,0x3173,
0x7c31,0x591f,0x63d9,0xe613,0xa40c,0x6f40,0x3ff2,0x2865,
0x9e87,0xa0e3,0xdfa1,0xedad,0x6cc0,0x4ff9,0xb11b,0x34b7,
0x931a,0x58a1,0xb550,0x8aea,0x5e4b,0x49c0,0x06af,0xc4d8,
0xa2b3,0x36e6,0x1429,0x8728,0x17da,0x26bd,0x05b5,0xaf61,
0x5c3b,0x8fee,0x1c97,0x345b,0x9260,0xd767,0x69a0,0xadd5,
0xe8de,0x7612,0x8378,0x3d1d,0x34f6,0x5384,0xaefd,0x6557,
0x466b,0xb4c4,0xf173,0x3740,0xe793,0xe929,0xeda9,0xada4,
0x0ef0,0x98f8,0xa9da,0x5cb9,0x7479,0x078e};

unsigned short PRIME[PRIME_SIZE] =
{0xac43,0xc798,0x6b03,0x9c3f,0xabe3,0x89ae,0xd2ea,0xa9fc,
0x1c6b,0xa5db,0x512a,0xcb18,0x0eab,0x79a9,0xd048,0x90cc,
0xc105,0xbce8,0x3e03,0x7680,0x47b6,0xaa46,0xe9af,0xe57f,
0x42ac,0x5ce0,0x82aa,0xc551,0xc19f,0xaa16,0x7d17,0x2400,
0x0790,0xe940,0xf320,0xe1de,0x5f86,0x4427,0xe628,0x4efc,
0xb79f,0x5591,0x74cf,0x9578,0x3fcc,0x95a1,0x1fee,0xf7ed,
0x12c6,0x1290,0xfb4d,0xe42b,0x48ea,0xcc71,0x8cfa,0x20cb,
0x5256,0xf754,0x21cb,0xdbad,0xef3b,0x4351,0x46c6,0xa332,
0xf712,0x6935,0x794d,0x8a09,0x6e9b,0xdf41,0x66b8,0x24cc,
0x3492,0x10f9,0x5f3d,0x04a3,0x7380,0xd0a8};

unsigned short GENERATOR[GENERATOR_SIZE] =
{0xf08b,0xf10d,0x9acc,0xbbed,0x0587,0x799c,0x70a6,0xeae4,
0x337d,0x5ec0,0x3462,0x1238,0x7afb,0x144e,0x3d61,0x84e0,
0x5419,0x4b06,0x4d87,0x2d13,0x255f,0xd7ff,0x7bf4,0x8e34,
0x4f48,0x7c07,0x1aba,0x2f7c,0x1b64,0x751b,0xbe05,0x3c6f,
0xb7db,0x1d62,0x6aac,0x3ef6,0xd7e2,0x2536,0x636b,0xc5a1,
0x6680,0x6dd5,0x7803,0x2566,0x2184,0x9727,0xbbd2,0x91a3,
0xfa0d,0xf48d,0xf1bc,0x4846,0xeaaa,0x4b4b,0x2073,0xbebd,
0xe635,0xcf66,0x278b,0x3ac3,0x8873,0x8d84,0x5c54,0x4eaa,
0x6bc8,0x3e3c,0x21f5,0x362d,0x20b8,0xc3b9,0x4be8,0xde23,
0x67c7,0xc193,0x59aa,0xf68d,0x571c,0x82c1};


#elif (SEL == 1)


/// OTHER PARAMETERS, generated for testing STS_PROTOCOL

unsigned char MODULUS_MASTER[MODULUS_SIZE_MASTER] =
    "\xa7\xcf\x2c\x9b\xa9\xfd\x38\xf7\x5e\x39\x02\x47\xf0\x9d\xd8\xf4"
    "\x5c\x52\x0f\x0e\x3c\x40\xab\xeb\x27\x86\x78\x57\x7c\x2b\xf4\x53"
    "\x10\x81\x28\x3e\x37\xa0\xc8\x76\x12\x7d\x2e\xb8\xdc\xeb\x4c\x7c"
    "\x0a\x88\xcf\x53\xe8\xf4\x9b\x33\x27\x2d\x47\x37\xc2\x81\xd8\xd0"
    "\x16\xf7\xe7\x0d\x92\xa7\xb8\xc1\x41\x0b\x3c\x79\x4b\xc8\x49\xae"
    "\x76\x1e\x27\x0f\xed\x44\x92\x08\xde\x5c\x5d\x5c\x2a\xe5\xf0\x31"
    "\xcb\x28\x3a\x14\x38\x28\x50\x6f\x47\x4a\x89\x3b\x72\x0a\x69\x09"
    "\x95\x3f\x89\x75\x46\x17\x51\x85\x2b\x8f\x8f\x53\x84\x99\xfa\xe0"
    "\x01\x32\xbc\x56\xd7\xd5\xc3\xeb\x19\xb8\x9b\x52\xef\xb5\xeb\x9f"
    "\x6d\x22\x94\xe5\x98\x75\xb6\x3b\xb8\xf5\xc5\xa0";

unsigned char PUBLIC_EXPONENT_MASTER[PUBLIC_SIZE_MASTER] =
	"\x01\x00\x01";

unsigned char PRIVATE_EXPONENT_MASTER[PRIVATE_SIZE_MASTER] =
    "\x11\x42\x27\x94\xfe\xef\x50\xe7\x95\x0a\x5a\xa0\x11\x28\xce\xe0"
    "\x7c\xf2\x66\x0f\xc9\x1b\xbc\x16\x6c\x71\xcb\x38\x3f\x1a\x86\x7c"
    "\xbc\x66\x56\x69\x60\x99\xcc\x32\x07\xdc\xe5\xcd\xb0\xd5\xc5\xdb"
    "\xb5\x36\x19\x4b\x3f\xeb\xb7\xf7\x0b\xef\xd4\x80\x12\x06\x4e\x51"
    "\x31\xae\x9f\xde\xa6\x2c\x69\xb1\x47\x88\x1c\x46\x42\xaf\x58\x19"
    "\xef\xef\xea\xdd\x14\xc6\xd8\x37\x1b\xee\xca\x4f\x67\x66\xd5\x47"
    "\x57\xe9\xab\x00\x33\xb8\x25\xb0\xee\x1b\x28\xc8\x99\xe4\xb3\x4d"
    "\x47\x61\xe7\x0f\x01\xfe\x9a\xe0\xc2\xcc\x5c\x67\x04\xcb\xcc\x12"
    "\xf8\x0b\xb9\x09\xf6\x25\x83\xc4\xf3\x15\x08\x73\x15\x65\xa7\xc2"
    "\x1d\x4e\x49\x5f\x74\x65\xc5\x52\xf2\x99\xa0\x0d";

unsigned char MODULUS_SLAVE[MODULUS_SIZE_SLAVE] =
    "\xf9\x2c\x83\xa6\xd2\x89\xa6\x01\x82\x41\x86\xf3\x1a\xe2\xa8\x48"
    "\x8b\x40\x43\xab\xc8\x3e\x13\x1a\x87\xbd\xd2\xe9\xd4\x6f\x17\xe1"
    "\x2d\x9f\x87\x10\xf6\xf4\xdf\x7a\x5e\x26\xfe\x6d\xad\xf5\x9d\x9f"
    "\x41\x3f\xcd\x17\x73\xfc\x7f\x21\x0b\xca\xab\x81\x21\x19\xe8\x77"
    "\x8b\xbd\xac\xb4\x30\x44\xde\x84\x76\xcf\x8d\x61\xb7\x3b\x4d\x21"
    "\xda\x59\xbc\x22\x55\xa5\x07\x8b\x1e\x14\xd3\xa6\x65\x98\xa4\xdb"
    "\xe4\x05\x52\x20\xfa\xc6\x39\x79\x84\x60\x17\x69\x56\x71\x0d\x6a"
    "\x48\x90\x2a\xc0\x92\x6b\x64\x9f\x22\x8e\xba\xd7\x21\x3e\x51\xbd"
    "\xc1\x3e\xaf\xf2\x07\xd2\xea\x18\x55\x08\x65\x4a\x13\x59\x18\xb6"
    "\x98\x17\x40\x7a\x3d\x0f\xe0\x5f\xb7\x2e\xd7\xbe";

unsigned char PUBLIC_EXPONENT_SLAVE[PUBLIC_SIZE_SLAVE] =
	"\x01\x00\x01";

unsigned char PRIVATE_EXPONENT_SLAVE[PRIVATE_SIZE_SLAVE] =
    "\xe5\xad\xbd\x3e\x6a\xac\x1c\x1e\x38\xca\xc0\x05\xea\x38\xb7\xe3"
    "\x79\x30\x09\x53\xcb\x43\x3e\xc9\x9c\xd3\x60\xbf\xb0\xa6\x55\x65"
    "\x60\xb3\xd3\x47\x3c\x66\x75\xda\xec\xb2\xe6\xa8\x0c\xa9\x61\xaf"
    "\xfe\xc8\xcf\x77\x6b\xcb\x0c\xc2\xbd\xfe\xcb\xce\x3c\x99\x3b\x36"
    "\x52\x6c\x2e\xcf\xa2\xc4\x8a\xb6\x95\x51\xc0\xb7\x1e\x4e\x7f\x92"
    "\x88\x5e\xac\x85\x8e\x59\xcf\x46\x12\x9d\x19\x5e\xd1\xd8\xb5\x32"
    "\x88\xfb\x9b\x9e\x04\xa9\x69\x74\x16\xc1\x06\xa5\x2b\x77\xe6\x5c"
    "\x63\x77\x23\x5d\x42\x91\xb6\xb1\x79\x02\x73\xad\x78\x4a\x59\x10"
    "\xe0\x9b\x06\xfd\x48\xc4\xa4\x28\x43\x84\xb6\xae\x23\x70\xc1\x59"
    "\x14\x3c\xf7\x1e\x20\x76\x03\x98\x60\x65\x7b\x67";

unsigned char GENERATOR[GENERATOR_SIZE] =
    "\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

unsigned char PRIME[PRIME_SIZE] =
    "\x73\x27\x42\x63\x45\x6e\xe4\x85\xc8\x06\xac\x23\x0c\x30\xd1\xbe"
    "\xff\x86\xf1\xa5\xda\x03\xb2\x98\x40\x8c\x17\x9a\x1b\x5f\x96\x42"
    "\x19\xd2\xd8\x81\x5b\x1c\x50\xff\x14\xb3\x7b\xd7\x47\xc8\xa7\xc9"
    "\xe4\x00\x45\x17\x65\xf1\x34\x46\x78\x37\xe9\xef\x85\xbd\x63\x9e"
    "\xc0\xac\x9a\x40\xd0\x80\x25\x8d\xd1\xae\x5a\x64\x27\x93\xc5\x75"
    "\x3c\xab\xe4\x22\x0a\xc8\xbf\x2d\x70\x9a\x0c\x2d\x28\x11\x67\xb4"
    "\x25\xa9\xe4\x63\x27\xeb\xb4\x94\x97\x23\xb3\x11\x0a\x97\x3a\x0d"
    "\xa2\xdd\xe5\x45\x21\x25\x22\xb7\x97\xe2\x68\x56\x69\xc2\xaf\x1b"
    "\x09\x02\x27\x5d\xfc\x14\x0a\x4a\xb0\x90\x4d\xab\x8f\x08\x16\x14"
    "\x58\xaa\xf6\x3b\x95\x57\x53\xab\x80\x4f\xbc\xe8";


#else


unsigned char MODULUS_MASTER[MODULUS_SIZE_MASTER] =
    "\x45\x87\x91\x79\xda\xf1\x0b\x17\x3b\x8b\xf1\xbd\x7d\x2d\x77\xbe"
    "\xc3\x5b\x13\x72\xf4\x3c\x23\x79\x90\x3f\x89\x84\x34\x9e\xb8\x8d"
    "\x40\x0a\x59\x32\x58\x92\x0e\xa4\xe8\x52\x88\x4a\xe2\x91\x33\xb1"
    "\x32\x0c\x22\xa7\x30\x51\x4c\x66\x9d\xd9\x70\x9a\xaa\x74\xb0\x76"
    "\x0d\x65\x61\x50\x9f\x42\x0a\x0b\x72\x60\xd0\x91\x49\x2b\x05\xa2"
    "\x66\x58\xc9\xb9\x5f\xa6\xfa\x55\xf9\x05\x43\x51\x99\x28\xdc\x4f"
    "\xe4\x77\xd6\xe3\x64\x59\x48\x8d\x98\xdf\x13\xd9\x34\x78\xb7\x01"
    "\xa1\xfb\xae\x97\x6e\xd4\xf0\x7d\xfc\x8a\xcd\xe8\x6c\x9c\xc0\x1e"
    "\x54\x68\xf0\xd0\x34\xa4\x29\xe4\x1e\xda\xa7\x07\x1d\x53\x54\x00"
    "\x9a\xb7\xa8\x1c\x33\x7a\x1a\x0b\xf5\xd7\xe2\xb7";


unsigned char PUBLIC_EXPONENT_MASTER[PUBLIC_SIZE_MASTER] =
	"\x01\x00\x01";

unsigned char PRIVATE_EXPONENT_MASTER[PRIVATE_SIZE_MASTER] =
    "\xed\xb4\xa3\x82\x08\xeb\x07\x28\x71\xf2\x8c\xb9\xda\x4d\x62\x62"
    "\x58\x12\x46\xda\x32\x8b\xce\x45\x3c\x67\x82\xc3\x6c\x20\x89\xa4"
    "\x90\x16\xd4\xdc\xdc\x90\x1c\x4e\x4a\x78\xa2\xf4\x87\xdf\x41\x44"
    "\x09\xb1\x2e\x3b\x47\xae\xde\xcb\x53\x7e\x27\x83\x79\xab\xd4\x42"
    "\x49\xce\x79\x08\xfa\xc2\xef\xc7\xd8\x1a\xc9\xf8\x9f\xce\xfc\x45"
    "\x89\xa0\x9a\xa5\xe7\xfa\x1e\x50\x43\xed\xcf\x13\x55\x6c\x57\xd2"
    "\xce\x52\xef\x94\xd4\xe8\x14\xae\x26\x3d\x1d\xe8\xfd\xfe\x97\x88"
    "\x41\x6b\x38\x14\xb1\x23\xac\xc2\xc3\x1b\x49\xb6\xab\x3a\xbe\x5e"
    "\x58\xb3\x35\xaa\x67\x47\x00\xde\x1c\x35\xda\xeb\xf1\xf0\xbc\x82"
    "\x7c\x44\x10\x22\x9b\x45\x73\x97\x92\xfe\xb1\x21";

unsigned char MODULUS_SLAVE[MODULUS_SIZE_SLAVE] =
    "\x07\xe6\x36\xdd\xc3\xfe\x28\x6c\x8d\x3d\x73\xb2\x83\xc4\x5f\x25"
    "\x3c\x24\xe9\x69\xea\x4d\xdc\x71\xeb\x7d\x98\x24\x5d\xe2\xb3\x67"
    "\x53\xf5\xe3\xeb\x09\x88\x9e\xf2\x1b\x28\xe5\xeb\x66\x5e\xe1\x2d"
    "\x6f\xc1\x67\xbd\xee\x70\x91\x3a\xac\x4d\x80\xac\x69\x66\xf0\xad"
    "\x20\x91\x9b\x67\xae\x81\xfc\x5b\x15\xa1\xdd\x22\x85\x2e\x57\x41"
    "\xf6\xdc\x89\xd7\xba\xdd\xdb\x7a\x6b\xd8\x48\xf0\x8f\x8c\xb8\x4a"
    "\xb9\x68\x06\xd6\x1f\x13\xd5\x36\xb6\x16\x1d\x72\xf7\xfa\x01\xd0"
    "\x3e\x63\xb9\xe4\x91\x88\xe3\x75\x0a\xce\x41\xb1\xbe\xed\x59\xf6"
    "\x48\xda\xc7\x87\x2d\xe5\x58\x00\xfd\x68\x06\xfd\xde\x2c\x3b\x4c"
    "\xf8\x74\x93\x0d\x90\xb0\x20\x8e\x8d\xeb\x9e\xc1";

unsigned char PUBLIC_EXPONENT_SLAVE[PUBLIC_SIZE_SLAVE] =
	"\x01\x00\x01";

unsigned char PRIVATE_EXPONENT_SLAVE[PRIVATE_SIZE_SLAVE] =
    "\xa9\x27\xb9\x6b\xe0\x10\xbe\x6c\x63\xfc\x5b\x28\xb4\x5b\xbb\x30"
    "\x65\xd7\x52\xa5\xc7\x90\x2f\xc7\x91\x0a\x7e\x95\x3c\x17\x1a\xdc"
    "\xfb\xf9\x8f\x38\x29\x24\xd4\x2f\xb5\x7a\xb3\x4e\x9c\xae\x79\xcc"
    "\x87\x5d\x0e\x3a\x30\xd2\x0b\xa5\xc6\x75\xfc\xce\x47\xab\x46\x28"
    "\x59\x09\xe3\x69\x3b\xcc\xd3\x72\xed\x51\xc1\x06\xb7\x1a\xdb\x86"
    "\x0d\x5b\xe1\x54\x3d\x7b\xba\xa6\x35\x35\x6f\xb1\xd6\x09\xe9\x12"
    "\x2e\x61\x6a\x6b\xf6\x77\x4d\xa6\xb9\xc2\x20\xc6\xc8\xd6\x04\x92"
    "\x69\x70\x00\x3d\x66\xd5\x3d\x04\x29\x96\x54\x08\xdd\x90\x59\xba"
    "\x51\x4c\xdb\x19\xbf\x8f\x6d\xbb\x1a\x98\xb8\x35\x3a\xa1\x96\xa6"
    "\xf9\xa9\x6f\x37\x4e\xd9\x16\x55\xa8\x5c\x59\xa8";

unsigned char GENERATOR[GENERATOR_SIZE] =
    "\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";


unsigned char PRIME[PRIME_SIZE] =
    "\x43\x85\xef\xc4\x49\x88\x54\x76\x62\x5e\xbc\x10\x17\x44\x4d\x65"
    "\x23\xdd\xd4\xe5\x49\x7a\xf0\xa1\x64\xdf\xd5\xe5\x91\xc2\xc9\x44"
    "\xdf\x9f\xe5\xcd\x8b\x26\x38\xaa\x68\x00\x4d\x17\xf5\x5b\x27\x27"
    "\xcc\x5a\xf7\x54\x9e\xf2\x3e\x54\x23\xba\x21\xe3\xeb\x12\xf0\xc2"
    "\xc2\xd2\x19\x1e\xdd\xe4\x5b\xba\xaf\xf1\xb1\x4b\x96\x61\xb7\x95"
    "\x87\xd4\xf3\x6f\x8b\x49\x22\x1c\xb7\x61\x16\xf1\xa3\x28\xb5\xb1"
    "\x43\x56\x1f\xdd\x96\x0a\x13\x73\x1f\x22\xe4\x66\xc1\xc8\x51\x95"
    "\x8e\x89\x15\x00\xdb\x5e\xa4\x96\xb2\x29\xd1\x4e\xd6\x34\xf8\x2e"
    "\x17\x5e\x23\x78\x4b\xc1\x0b\xf7\x58\x0a\xa3\x9a\xd8\x57\x82\x3c"
    "\x6a\x57\x9f\x5b\xb8\x06\x70\xbd\xfe\xfa\xb7\xd0";


#endif
