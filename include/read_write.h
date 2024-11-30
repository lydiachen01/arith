/*
 * read_write.h
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Interface for read_write
 *
 */

#ifndef IMAGE_READER
#define IMAGE_READER

#include <stdio.h>
#include <string.h>
#include <mem.h>
#include <pnm.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <assert.h>
#include "bitpack.h"

Pnm_ppm read_original(FILE *file);
void write_compressed(Pnm_ppm image);

Pnm_ppm read_compressed(FILE *file);
void write_original(Pnm_ppm image);

#endif
