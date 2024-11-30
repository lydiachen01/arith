/*
 * compress40.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of compress40 and the compress and decompress functions.
 * The compress function reads an original image from an input file,
 * performs various conversions on its color representations,
 * packs the data into 32-bit codewords, and then writes the compressed
 * image to standard output. The decompress40 function reads a
 * compressed image from a specified file, unpacks 32-bit codewords, and
 * undergoes reverse conversions to reconstruct the original image, which
 * is written to standard output
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compress40.h"
#include "a2methods.h"
#include "assert.h"
#include "read_write.h"
#include "rgb_cv_converter.h"
#include "luma_cos_converter.h"
#include "bitpack.h"
#include "codewords.h"

/********** compress40 ********
 *
 * Compresses an image in Pnm_ppm format and writes the compressed data
 * to standard output.
 *
 * Parameters:
 *      FILE *input         Input file pointer containing the original image
 *
 * Return: none
 *
 * Expects: input is a valid file pointer to an existing file containing
 *          a Pnm_ppm image in RGB format
 *
 * Notes: Asserts that input is not NULL
 *        Reads the original image from the input file
 *        Converts RGB to component video space
 *        Converts component to average chromas and cosine coefficients
 *        Packs data into 32-bit codewords
 *        Writes the compressed data to standard output
 *        Frees allocated memory for the Pnm_ppm image
 ************************/
void compress40(FILE *input)
{
        assert(input != NULL);
        Pnm_ppm image = read_original(input);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image = luma_cos_convert(image, "luma_to_cos");
        image = codewords(image, "pack");
        write_compressed(image);
        Pnm_ppmfree(&image);
}

/********** decompress40 ********
 *
 * Decompresses an image in Pnm_ppm format and writes the decompressed data
 * to standard output.
 *
 * Parameters:
 *      FILE *input        Input file pointer containing the compressed image
 *
 * Return: none
 *
 * Expects: input is a valid file pointer to an existing file containing
 *          a compressed Pnm_ppm image
 *
 * Notes: Asserts that input is not NULL
 *        Reads the compressed image from the input file
 *        Unpacks 32-bit codewords
 *        Converts average chromas and cosine coefficients to component video
 *        Converts component video to RGB floats
 *        Writes the decompressed data to standard output
 *        Frees allocated memory for the Pnm_ppm image
 ************************/
void decompress40(FILE *input)
{
        assert(input != NULL);
        Pnm_ppm image = read_compressed(input);
        image = codewords(image, "unpack");
        image = luma_cos_convert(image, "cos_to_luma");
        image = rgb_cv_convert(image, "cv_to_rgb");
        write_original(image);
        Pnm_ppmfree(&image);
}
