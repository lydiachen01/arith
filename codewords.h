/*
 * codewords.h
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Interface for codewords
 *
 */

#ifndef _CODEWORD
#define _CODEWORD

#include <string.h>
#include <assert.h>
#include <bitpack.h>
#include <pnm.h>

Pnm_ppm codewords(Pnm_ppm image, char *command);

#endif
