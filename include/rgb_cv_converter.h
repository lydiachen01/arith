/*
 * rgb_cv_converter.h
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Interface for rgb_cv_converter
 *
 */

#ifndef RGB_CV_CONVERTER
#define RGB_CV_CONVERTER

#include <pnm.h>
#include <string.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <assert.h>

Pnm_ppm rgb_cv_convert(Pnm_ppm image, char *command);

#endif
