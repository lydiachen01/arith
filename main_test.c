#include "read_write.h"
#include "rgb_cv_converter.h"
#include "luma_cos_converter.h"
#include "bitpack.h"
#include "codewords.h"
#include "pnm_types.h"

#include <pnm.h>
#include <a2methods.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <assert.h>

void test1(FILE *fp);
void test2(FILE *fp);
void test3(FILE *fp);
void test4(FILE *fp);
void test5();
void test6();
void test7();
void test8();
void reverse1(FILE *fp);
void reverse2(FILE *fp);
void reverse3(FILE *fp);
void reverse4(FILE *fp);
void reverse5(FILE *fp);
void pack1(FILE *fp);
void unpack1(FILE *fp);
void apply2(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
void apply3(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);
void apply4(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl);

int main(int argc, char *argv[])
{
        (void)argc;
        FILE *fp = fopen(argv[1], "r");
        test7();
        fclose(fp);    
}

/*-------------------------- MAIN TEST FUNCTIONS --------------------------*/

/* convert to rgb to component video and back */
void reverse1(FILE *fp)
{
        Pnm_ppm image = read_original(fp);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image = rgb_cv_convert(image, "cv_to_rgb"); 
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}

/* convert to rgb to component video to cosine coefficients and back */
void reverse2(FILE *fp)
{
        Pnm_ppm image = read_original(fp);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image = luma_cos_convert(image, "luma_to_cos");
        image = luma_cos_convert(image, "cos_to_luma");
        image = rgb_cv_convert(image, "cv_to_rgb");
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}

/* a complete compression and decompression */
void reverse3(FILE *fp)
{
        Pnm_ppm image = read_original(fp);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image = luma_cos_convert(image, "luma_to_cos");
        image = codewords(image, "pack");
        image = codewords(image, "unpack");
        image = luma_cos_convert(image, "cos_to_luma");
        image = rgb_cv_convert(image, "cv_to_rgb");
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}

/* test an iteration of compression */
void pack(FILE *fp)
{
        Pnm_ppm image = read_original(fp);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image = luma_cos_convert(image, "luma_to_cos");
        image = codewords(image, "pack");
        write_compressed(image);
        Pnm_ppmfree(&image);
}

/* test an iteration of decompression */
void unpack(FILE *fp)
{
        Pnm_ppm image = read_compressed(fp);
        image = codewords(image, "unpack");
        image = luma_cos_convert(image, "cos_to_luma");
        image = rgb_cv_convert(image, "cv_to_rgb");
        write_original(image);
        Pnm_ppmfree(&image);
}

/* test read_original */
void test1(FILE *fp)
{
        Pnm_ppm image = read_original(fp);
        printf("%d ", image->width);
        printf("%d ", image->height);
        printf("%d\n", image->denominator);
        assert(image->width % 2 == 0);
        assert(image->height % 2 == 0);
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}

/* print rgb int values */
void test2(FILE *fp) 
{
        Pnm_ppm image = read_original(fp);
        image->methods->map_row_major(image->pixels, apply2, NULL);
        Pnm_ppmfree(&image);
}

/* print cv float values */
void test3(FILE *fp) 
{
        Pnm_ppm image = read_original(fp);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image->methods->map_row_major(image->pixels, apply3, NULL);
        Pnm_ppmfree(&image);
}

/* print cv int values */
void test4(FILE *fp) 
{
        Pnm_ppm image = read_original(fp);
        image = rgb_cv_convert(image, "rgb_to_cv");
        image = luma_cos_convert(image, "luma_to_cos");
        image->methods->map_row_major(image->pixels, apply4, NULL);
        Pnm_ppmfree(&image);
}

/* test bitpack fit functions */
void test5()
{
        bool trial_1 = Bitpack_fitsu(5, 3);
        bool trial_2 = Bitpack_fitss(5, 3);
        bool trial_3 = Bitpack_fitsu(0xf, 4);
        assert(trial_1 == true);
        assert(trial_2 == false);
        printf("FIT = #1: %d || #2: %d\n", trial_1, trial_2);
        printf("FIT = #3: %d\n", trial_3);
}

/* test bitpack get functions */
void test6()
{
        int64_t trial_1 = Bitpack_getu(0x3f4, 6, 2);
        int64_t trial_2 = Bitpack_gets(0x3f4, 6, 2);
        assert(trial_1 == 61);
        assert(trial_2 == -3);
        printf("GET = 1: %ld || #2: %ld\n", trial_1, trial_2);
}

/* test bitpack new functions */
void test7()
{
        uint64_t word = 0xabcd;
        unsigned width = 4;
        unsigned lsb = 8;
        uint64_t new_value_0 = 0xf;
        uint64_t trial_0 = Bitpack_newu(word, width, lsb, new_value_0);
        assert(trial_0 == 0xafcd); 

        uint64_t word_1 = 0x12345678;
        unsigned width_1 = 8;
        unsigned lsb_1 = 16;
        uint64_t new_value_1 = 0xab;
        uint64_t trial_1 = Bitpack_newu(word_1, width_1, lsb_1, new_value_1);
        assert(trial_1 == 0x12ab5678);

        uint64_t word_2 = 0x87654321;
        unsigned width_2 = 8;
        unsigned lsb_2 = 4;
        uint64_t new_value_2 = 0x5;
        uint64_t trial_2 = Bitpack_newu(word_2, width_2, lsb_2, new_value_2);
        assert(trial_2 == 0x87654051);

        uint64_t word_3 = 0xabcdef12;
        unsigned width_3 = 8;
        unsigned lsb_3 = 20;
        uint64_t new_value_3 = 0x1f;
        uint64_t trial_3 = Bitpack_newu(word_3, width_3, lsb_3, new_value_3);
        assert(trial_3 == 0xa1fdef12);

        printf("#0: %lx\n", trial_0);
        printf("#1: %lx\n", trial_1);
        printf("#2: %lx\n", trial_2);
        printf("#3: %lx\n", trial_3);
}

/* test bitpack functions together */
void test8()
{
        uint64_t word = 0xabcd;
        unsigned w = 4;
        unsigned lsb = 8;
        uint64_t val = 0xf;
        assert(Bitpack_getu(Bitpack_newu(word, w, lsb, val), w, lsb) == val);
}

/*---------------------------- APPLY FUNCTIONS ----------------------------*/


/* apply function for printing rgb integers */
void apply2(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl) 
{
        Pnm_rgb new_elem = (Pnm_rgb)elem;
        printf("%d ", new_elem->red);
        printf("%d ", new_elem->green);
        printf("%d\n", new_elem->blue);
        (void)i;
        (void)j;
        (void)array2;
        (void)cl;
}

/* apply function for printing cv floats */
void apply3(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl) 
{
        Pnm_cv_f new_elem = (Pnm_cv_f)elem;
        printf("CV: %f ", new_elem->y);
        printf("%f ", new_elem->pb);
        printf("%f\n", new_elem->pr);
        (void)i;
        (void)j;
        (void)array2;
        (void)cl;
}

/* apply function for printing cosine coefficients and avg chroma int */
void apply4(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl) 
{
        Pnm_chromas_cos_int new_elem = (Pnm_chromas_cos_int)elem;
        printf("a = %d  || ", new_elem->a);
        printf("b = %d  || ", new_elem->b);
        printf("c = %d  || ", new_elem->c);
        printf("d = %d  || ", new_elem->d);
        printf("pb = %d  || ", new_elem->pb_avg & 0xF);
        printf("pr = %d\n", new_elem->pr_avg & 0xF);
        (void)elem;
        (void)i;
        (void)j;
        (void)array2;
        (void)cl;
}