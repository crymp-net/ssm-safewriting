#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* FIPS 180-2 SHA-224/256/384/512 implementation
	* Last update: 02/02/2007
	* Issue date:  04/30/2005
	*
	* Copyright (C) 2013, Con Kolivas <kernel@kolivas.org>
	* Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
	* All rights reserved.
	*
	* Redistribution and use in source and binary forms, with or without
	* modification, are permitted provided that the following conditions
	* are met:
	* 1. Redistributions of source code must retain the above copyright
	*    notice, this list of conditions and the following disclaimer.
	* 2. Redistributions in binary form must reproduce the above copyright
	*    notice, this list of conditions and the following disclaimer in the
	*    documentation and/or other materials provided with the distribution.
	* 3. Neither the name of the project nor the names of its contributors
	*    may be used to endorse or promote products derived from this software
	*    without specific prior written permission.
	*
	* THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
	* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	* ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
	* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
	* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
	* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
	* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
	* SUCH DAMAGE.
	*/


#ifndef SHA2_H
#define SHA2_H

#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA256_BLOCK_SIZE  ( 512 / 8)

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

	typedef struct {
		unsigned int tot_len;
		unsigned int len;
		unsigned char block[2 * SHA256_BLOCK_SIZE];
		uint32_t h[8];
	} sha256_ctx;

	extern uint32_t sha256_k[64];

	void sha256_init(sha256_ctx * ctx);
	void sha256_update(sha256_ctx *ctx, const unsigned char *message,
		unsigned int len);
	void sha256_final(sha256_ctx *ctx, unsigned char *digest);
	void sha256(const unsigned char *message, unsigned int len,
		unsigned char *digest);

#endif /* !SHA2_H */

	// #define the macros below to 1/0 to enable/disable the mode of operation.
	//
	// CBC enables AES encryption in CBC-mode of operation.
	// CTR enables encryption in counter-mode.
	// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously.

	// The #ifndef-guard allows it to be configured before #include'ing or at compile time.
#ifndef CBC
#define CBC 1
#endif

#ifndef ECB
#define ECB 1
#endif

#ifndef CTR
#define CTR 1
#endif


#define AES128 1
	//#define AES192 1
	//#define AES256 1

#define AES_BLOCKLEN 16 //Block length in bytes AES is 128b block only

#if defined(AES256) && (AES256 == 1)
#define AES_KEYLEN 32
#define AES_keyExpSize 240
#elif defined(AES192) && (AES192 == 1)
#define AES_KEYLEN 24
#define AES_keyExpSize 208
#else
#define AES_KEYLEN 16   // Key length in bytes
#define AES_keyExpSize 176
#endif

	struct AES_ctx
	{
		uint8_t RoundKey[AES_keyExpSize];
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
		uint8_t Iv[AES_BLOCKLEN];
#endif
	};

	void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);
#if (defined(CBC) && (CBC == 1)) || (defined(CTR) && (CTR == 1))
	void AES_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
	void AES_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);
#endif

#if defined(ECB) && (ECB == 1)
	// buffer size is exactly AES_BLOCKLEN bytes; 
	// you need only AES_init_ctx as IV is not used in ECB 
	// NB: ECB is considered insecure for most uses
	void AES_ECB_encrypt(struct AES_ctx* ctx, const uint8_t* buf);
	void AES_ECB_decrypt(struct AES_ctx* ctx, const uint8_t* buf);

#endif // #if defined(ECB) && (ECB == !)


#if defined(CBC) && (CBC == 1)
	// buffer size MUST be mutile of AES_BLOCKLEN;
	// Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
	// NOTES: you need to set IV in ctx via AES_init_ctx_iv() or AES_ctx_set_iv()
	//        no IV should ever be reused with the same key 
	void AES_CBC_encrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
	void AES_CBC_decrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);

#endif // #if defined(CBC) && (CBC == 1)


#if defined(CTR) && (CTR == 1)

	// Same function for encrypting as for decrypting. 
	// IV is incremented for every block, and used after encryption as XOR-compliment for output
	// Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme
	// NOTES: you need to set IV in ctx with AES_init_ctx_iv() or AES_ctx_set_iv()
	//        no IV should ever be reused with the same key 
	void AES_CTR_xcrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);

#endif // #if defined(CTR) && (CTR == 1)

#ifdef __cplusplus
}
#endif

#endif //_AES_H_
