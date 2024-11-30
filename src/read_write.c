/*
 * read_write.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of read_write which reads and writes PPM images in both 
 * original and compressed formats and additional helper 
 *
 */

#include "read_write.h"

#define DENOMINATOR 255
#define BYTE 8

/************************** Helper Functions ************************/
/* Trim pixmap to even dimensions */
static void trim_edge(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);

/* Read in compressed file */
static void read_32_bits(int i, int j, A2Methods_UArray2 array2, 
                void *elem, void *cl);

/* Print compressed image */
static void print(int i, int j, A2Methods_UArray2 array2, 
                void *elem, void *cl);
/********************************************************************/

/********** read_original ********
 *
 * Read in the original image from given file pointer
 *
 * Parameters:
 *      FILE *file              pointer to file containing original image
 *
 * Return: Pnm_ppm containing the original image
 *
 * Expects:
 *      file is not NULL
 *      *file is not NULL
 * 
 * Notes: Utilizes the uarray2_methods_plain methods to read the image
 *        Checks that image dimensions are even; trims if necessary
 *        Assert that image is not NULL
 *        Assert that methods is not NULL
 *        Assert that dimensions are at least 2 x 2
 *        Assert that new pixels is not NULL
 *      
 ************************/
Pnm_ppm read_original(FILE *file)
{
        A2Methods_T methods = uarray2_methods_plain; 
        Pnm_ppm image = Pnm_ppmread(file, methods);
        assert(methods != NULL);
        assert(image != NULL);
        assert(image->width >= 2 && image->height >= 2); 
        
        if (image->width % 2 != 0 || image->height % 2 != 0) {
                /* ensures even dimensions */
                int new_w = image->width % 2 != 0 ? image->width - 1 \
                : image->width;
                int new_h = image->height % 2 != 0 ? image->height - 1 \
                : image->height;
                image->width = new_w;
                image->height = new_h;

                A2Methods_UArray2 new_pixels = image->methods->new(image->width,
                                                                image->height,
                                                        sizeof(struct Pnm_ppm));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, trim_edge, image);
                image->methods->free(&(image->pixels));
                image->pixels = new_pixels;
        }

        return image;
}

/********** write_compressed ********
 *
 * Print the compressed image
 *
 * Parameters:
 *      Pnm_ppm image           Pnm_ppm containing compressed image
 *
 * Return: none
 *
 * Expects:
 *      image is not NULL
 * 
 * Notes: Print to standard output
 *      
 ************************/
void write_compressed(Pnm_ppm image)
{
        int width = image->width;
        int height = image->height;
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        image->methods->map_row_major(image->pixels, print, NULL);
        printf("\n");
}

/********** read_compressed ********
 *
 * Read in the compressed image
 *
 * Parameters:
 *      FILE *file              pointer to file containing compressed image
 *
 * Return: Pnm_ppm containing the original image
 *
 * Expects:
 *   *file is not NULL
 * 
 * Notes: Asserts that file is not NULL
 *        Assert that pixels is not NULL
 *        Assert that image is not NULL
 *        Assert that read is 2 and c is a new line
 *      
 ************************/
Pnm_ppm read_compressed(FILE *file) 
{
        assert(file != NULL);
        unsigned width, height;
        int read = fscanf(file, "COMP40 Compressed image format 2\n%u %u",
                          &width, &height);
        assert(read == 2);
        int c = getc(file);
        assert(c == '\n'); 

        A2Methods_T methods = uarray2_methods_plain;
        A2Methods_UArray2 pixels = methods->new(width / 2, height / 2, 
                                                sizeof(uint64_t));
        assert(pixels != NULL);
        Pnm_ppm image = ALLOC(sizeof(struct Pnm_ppm));
        assert(image != NULL);

        /* Intialize a Pnm_ppm struct */
        image->width = width;
        image->height = height;
        image->denominator = DENOMINATOR;
        image->pixels = pixels;
        image->methods = methods;

        methods->map_row_major(pixels, read_32_bits, file);
        return image;
}

/********** write_original ********
 *
 * Print the original image
 *
 * Parameters:
 *      Pnm_ppm image           Pnm_ppm containing original image
 *
 * Return: none
 *
 * Expects:
 *      image is not NULL
 * 
 * Notes: Print to standard output
 *      
 ************************/
void write_original(Pnm_ppm image)
{
        Pnm_ppmwrite(stdout, image);
}

/********** trim_edge ********
 *
 * Apply function for trimming edge pixels to ensure even image dimensions
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
 * Expects: Column and row are within bounds;
 *          array2 is not NULL;
 *      
 * Notes: Iterating through new pixmap, while getting old elemets through
 *        the closure and methods to copy into new pixmap
 *      
 ************************/
static void trim_edge(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl;
        Pnm_rgb old_elem = (Pnm_rgb)(image->methods->at(image->pixels, i, j));
        assert(old_elem != NULL);
        *(Pnm_rgb)elem = *old_elem;
        (void) array2;
}

/********** read_32_bits ********
 *
 * Apply function for reading in 32-bit codewords
 *
 * Parameters:
 *      int i                           column index of element
 *      int j                           row index of element
 *      A2Methods_UArray2 array2        UArray2 to contain new data
 *      void *elem                      element at (i, j) of array2
 *      void *cl                        pointer to file
 *
 * Return: none
 *
 * Expects: Column and row are within bounds;
 *          array2 is not NULL;
 *      
 * Notes: none
 *      
 ************************/
static void read_32_bits(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        FILE *file = (FILE*)cl;
        uint64_t codeword = 0;
        for (int lsb = 24; lsb >= 0; lsb-=8) {
                // Get leftmost digit (i.e. least significant digit)
                codeword = Bitpack_newu(codeword, 8, lsb, getc(file));
        }
        *(uint64_t*)elem = codeword; /* Update element with codeword */
        (void)i;
        (void)j;
        (void)array2;
}

/********** print ********
 *
 * Apply function for printing compressed image
 *
 * Parameters:
 *      int i                           column index of element
 *      int j                           row index of element
 *      A2Methods_UArray2 array2        UArray2 containing compressed image
 *      void *elem                      element at (i, j) of array2
 *      void *cl                        NULL
 *
 * Return: none
 *
 * Expects: Column and row are within bounds;
 *          array2 is not NULL;
 *      
 * Notes: none
 *      
 ************************/
static void print(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
        uint64_t codeword = *(uint64_t *)elem;
        for (int byte = 24; byte >= 0; byte-=8) {
                // Get leftmost digit (i.e. least significant digit)
                putchar(Bitpack_getu(codeword, 8, byte));
        }
        (void)i;
        (void)j;
        (void)cl;
        (void)array2;
}
