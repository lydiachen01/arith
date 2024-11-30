/*
 * int_float_converter.h
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Interface for int_float_converter
 *
 */

#ifndef INT_FLOAT_CONVERT
#define INT_FLOAT_CONVERT

#include <string.h>
#include <mem.h>
#include <pnm.h>
#include <arith40.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <assert.h>

Pnm_ppm int_float_convert(Pnm_ppm image, char *command);
void rgb_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
void rgb_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
void cos_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
void cos_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);

#endif
