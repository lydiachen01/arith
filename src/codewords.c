 /*
 * codewords.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of codewords where apply functions are used to pack and 
 * unpack 32-bit codewords
 *
 */

#include "codewords.h"

#define WIDTH_A 6
#define WIDTH_B 6
#define WIDTH_C 6
#define WIDTH_D 6
#define WIDTH_PB 4
#define WIDTH_PR 4
#define LSB_A 26
#define LSB_B 20
#define LSB_C 14
#define LSB_D 8
#define LSB_PB 4
#define LSB_PR 0


/****************************** Structs ****************************/
/* A struct that stores average chromas and cosine coefficient 
integer values */
typedef struct Pnm_chromas_cos_int {
        uint16_t a;
        int8_t b, c, d;
        uint8_t pb_avg, pr_avg;
} *Pnm_chromas_cos_int;
/********************************************************************/


/************************** Helper Functions ************************/
/* Helper functions to pack and unpack 32-bit codewords */
static void apply_pack(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);
static void apply_unpack(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);
/********************************************************************/


/********** codewords ********
 *
 * Pack and unpack image data within 32-bit codewords
 *
 * Parameters:
 *      Pnm_ppm image           Pnm_ppm containing image
 *      char *command           string containing conversion option
 *
 * Return: Pnm_ppm containing the converted image data representation
 *
 * Expects:
 *      image is not NULL
 *      command is either "pack" or "unpack"
 * 
 * Notes: See inline comments
 *      
 ************************/
Pnm_ppm codewords(Pnm_ppm image, char *command)
{
        /* Width and height are the original trimmed dimensions*/
        A2Methods_UArray2 new_pixels;
        
        if (strcmp(command, "pack") == 0) { 
                /* pack into 32-bit codewords*/
                new_pixels = image->methods->new (image->width / 2,
                                                image->height / 2,
                                                sizeof(uint64_t));
                assert(new_pixels != NULL);
                /* map through the pixmap */
                image->methods->map_row_major(new_pixels, apply_pack, image);
        } else if (strcmp(command, "unpack")  == 0) { 
                /* unpack 32-bit codewords*/
                new_pixels = image->methods->new(image->width / 2,
                                                image->height / 2,
                                        sizeof(struct Pnm_chromas_cos_int));
                assert(new_pixels != NULL);
                /* map through the pixmap */
                image->methods->map_row_major(new_pixels, apply_unpack, image);
        }
         
        image->methods->free(&(image->pixels));
        image->pixels = new_pixels;

        return image;
}

/********** apply_pack ********
 *
 * Apply function for packing 32-bit codewords in big-endian order
 *
 * Parameters:
 *      int i                           column index of element
 *      int j                           row index of element
 *      A2Methods_UArray2 array2        UArray2 to contain new data
 *      void *elem                      element at (i, j) of array2
 *      void *cl                        Pnm_ppm containing image
 *
 * Return: none
 *
 * Expects: The pixmap contain elements of type 
 *          Pnm_chromas_cos_int integers
 *      
 * Notes: Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *        Applied as a mapping function using map_row_major
 *      
 ************************/
static void apply_pack(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl;
        assert(image != NULL);
        assert(elem != NULL);
        Pnm_chromas_cos_int curr = image->methods->at(image->pixels, i, j);

        uint64_t word = 0xf;
        word = Bitpack_newu(word, WIDTH_A, LSB_A, curr->a);
        word = Bitpack_news(word, WIDTH_B, LSB_B, curr->b);
        word = Bitpack_news(word, WIDTH_C, LSB_C, curr->c);
        word = Bitpack_news(word, WIDTH_D, LSB_D, curr->d);
        word = Bitpack_newu(word, WIDTH_PB, LSB_PB, curr->pb_avg);
        word = Bitpack_newu(word, WIDTH_PR, LSB_PR, curr->pr_avg);

        *(int64_t *)elem = word; /* 32-bit codeword*/

        (void)i;
        (void)j;
        (void)array2;
}

/********** apply_unpack ********
 *
 * Apply function for unpacking 32-bit codewords
 *
 * Parameters:
 *      int i                           column index of element
 *      int j                           row index of element
 *      A2Methods_UArray2 array2        UArray2 to contain new data
 *      void *elem                      element at (i, j) of array2
 *      void *cl                        Pnm_ppm containing image
 *
 * Return: none
 *
 * Expects: The pixmap contain elements of type uint64_t
 *      
 * Notes: Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *        Applied as a mapping function using map_row_major
 *      
 ************************/
static void apply_unpack(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl;
        assert(image != NULL);
        assert(elem != NULL);
        Pnm_chromas_cos_int chroma_int = (Pnm_chromas_cos_int)elem;
        uint64_t *codeword = image->methods->at(image->pixels, i, j);
        
        /* Unpack each part*/
        chroma_int->a = (uint16_t)Bitpack_getu(*codeword, WIDTH_A, LSB_A);
        chroma_int->b = (uint8_t)Bitpack_gets(*codeword, WIDTH_B, LSB_B);
        chroma_int->c = (uint8_t)Bitpack_gets(*codeword, WIDTH_C, LSB_C);
        chroma_int->d = (uint8_t)Bitpack_gets(*codeword, WIDTH_D, LSB_D);
        chroma_int->pb_avg = (uint8_t)Bitpack_getu(*codeword, WIDTH_PB, 
                                                        LSB_PB);
        chroma_int->pr_avg = (uint8_t)Bitpack_getu(*codeword, WIDTH_PR, 
                                                        LSB_PR);

        (void)i;
        (void)j;
        (void)array2;
}