/*
 * int_float_converter.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * The interface that contains structure definitions used to represent the
 * various color representations used in image compression and decompression.
 *
 */

/* A struct that stores rgb float values */
typedef struct Pnm_rgb_f {
        float red, green, blue;
} *Pnm_rgb_f;

/* A struct that stores component video float values */
typedef struct Pnm_cv_f { 
        float y, pb, pr;
} *Pnm_cv_f;

/* A struct that stores average chromas and cosine coefficient 
float values */
typedef struct Pnm_chromas_cos_f {
        float a, b, c, d, pb_avg, pr_avg;
} *Pnm_chromas_cos_f;

/* A struct that stores average chromas and cosine coefficient 
integer values */
typedef struct Pnm_chromas_cos_int {
        uint16_t a;
        int8_t b, c, d;
        uint8_t pb_avg, pr_avg;
} *Pnm_chromas_cos_int;
