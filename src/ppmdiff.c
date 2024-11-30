/*
 * ppmdiff.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of ppmdiff
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pnm.h>
#include <a2methods.h>
#include <uarray2b.h>
#include <uarray2b.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2.h>

int main(int argc, char *argv[])
{
        if (argc != 3) {
                printf("Usage: ./ppmdiff file_name file_name\n");
                exit(EXIT_FAILURE);
        }

        if (strcmp(argv[1], "-") == 0 && strcmp(argv[2], "-") == 0) {
                printf("Error: both arguments may not be -\n");
                exit(EXIT_FAILURE);
        }

        FILE *i, *i_prime;

        if (strcmp(argv[1], "-") != 0) {
                i = fopen(argv[1], "r");
        } else {
                i = stdin;
        }

        if (strcmp(argv[2], "-") != 0) {
                i_prime = fopen(argv[2], "r");
        } else {
                i = stdin;
        }

        A2Methods_T methods = uarray2_methods_plain; 

        Pnm_ppm i_raster = Pnm_ppmread(i, methods);
        Pnm_ppm i_prime_raster = Pnm_ppmread(i_prime, methods);

        int w1 = i_raster->width;
        int w2 = i_prime_raster->width;
        int h1 = i_raster->height;
        int h2 = i_prime_raster->height;

        if ((abs(w1 - w2) <= 1) && (abs(h1 - h2) <= 1)) {
                float E;
                A2Methods_UArray2 p1 = i_raster->pixels;
                A2Methods_UArray2 p2 = i_prime_raster->pixels;

                int w = w1 <= w2 ? w1 : w2;
                int h = h1 <= h2 ? h1 : h2;

                for (int i = 0; i < w; i++) {
                        for (int j = 0; j < h; j++) {
                                Pnm_rgb i_rgb = methods->at(p1, i, j);
                                Pnm_rgb i_prime_rgb = methods->at(p2, i, j);

                                int r_diff = i_rgb->red - i_prime_rgb->red;
                                int g_diff = i_rgb->green - i_prime_rgb->green;
                                int b_diff = i_rgb->blue - i_prime_rgb->blue;

                                E += pow(r_diff, 2) + pow(g_diff, 2) 
                                     + pow(b_diff, 2);
                        }
                }

                E = sqrt(E)/(3 * w * h);
                printf("%0.6f\n", E);
        } else {
                printf("1.0\n");
        }

        return 0;
}
