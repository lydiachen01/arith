/*
 * luma_cos_converter.h
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Interface for luma_cos_converter
 *
 */

#ifndef LUMA_COS_CONVERTER
#define LUMA_COS_CONVERTER

#include <pnm.h>
#include <string.h>
#include <arith40.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <assert.h>

Pnm_ppm luma_cos_convert(Pnm_ppm image, char *command);

#endif
