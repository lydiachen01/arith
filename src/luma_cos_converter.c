/*
 * luma_cos_converter.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of luma_cos_converter which converts between the 
 * component video (Y, Pb, Pr) color space float values and cosine 
 * coefficients/average chromas (a, b, c, d, Pb_avg, Pr_avg) float values.
 *
 */

#include "luma_cos_converter.h"


/****************************** Structs ****************************/
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
/********************************************************************/


/************************** Helper Functions ************************/
/* Helper functions to convert between luma and cos representations */
static void luma_to_cos(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);
static void cos_to_luma(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);

/* Helper functions to convert between integer and float respresentation */
static Pnm_ppm cos_int_float_convert(Pnm_ppm image, char *command);
static void cos_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
static void cos_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
/********************************************************************/


/********** luma_cos_convert ********
 *
 * Convert between lumaniance values and cosine coefficient representations
 *
 * Parameters:
 *      Pnm_ppm image           Pnm_ppm containing image
 *      char *command           string containing conversion option
 *
 * Return: Pnm_ppm containing the converted image data representation
 *
 * Expects:
 *      image is not NULL
 * 
 * Notes:
 *      luma to cos             "to_cos"
 *      cos to luma             "to_luma"
 *      Asserts that image is not NULL
 *      
 ************************/
Pnm_ppm luma_cos_convert(Pnm_ppm image, char *command) 
{       
        assert(image != NULL);
        A2Methods_UArray2 new_pixels;
        if (strcmp(command, "luma_to_cos") == 0) {
                new_pixels = image->methods->new(image->width / 2, 
                                                image->height / 2, 
                                        sizeof(struct Pnm_chromas_cos_f));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, luma_to_cos, image);
                image->methods->free(&(image->pixels));
                image->pixels = new_pixels;

                /* Convert to integers */
                image = cos_int_float_convert(image, "cos_to_int"); 

        } else if (strcmp(command, "cos_to_luma") == 0) {
                /* Convert average chromas and cosine coefficients integers
                to floats */
                image = cos_int_float_convert(image, "cos_to_float");

                new_pixels = image->methods->new(image->width, 
                                                image->height, 
                                                sizeof(struct Pnm_cv_f));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, cos_to_luma, image);
                image->methods->free(&(image->pixels));
                image->pixels = new_pixels;
        }

        return image;
}

/********** luma_to_cos ********
 *
 * Apply function for converting lumaniance values to cosine coefficients and 
 * average chromas
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
 * Expects: The width and height is even;
 *          The pixmap in image contain elements of type 
 *          struct Pnm_cv_f floats      
 *          New pixmap is half the size of the current incoming pixmap
 * 
 * Notes: Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *        Applied as a mapping function using map_row_major
 *      
 ************************/
static void luma_to_cos(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl;
        assert(image != NULL);
        assert(elem != NULL);

        int n_i = i * 2;
        int n_j = j * 2;

        /*
         * each element containing a four-pixel average at (i, j) maps to the
         * first pixel of a 2x2 block in the original image via (i * 2, j * 2)
         */
        Pnm_cv_f px1 = image->methods->at(image->pixels, n_i, n_j);
        Pnm_cv_f px2 = image->methods->at(image->pixels, n_i + 1, n_j);
        Pnm_cv_f px3 = image->methods->at(image->pixels, n_i, n_j + 1);
        Pnm_cv_f px4 = image->methods->at(image->pixels, n_i + 1, n_j + 1);
        assert(px1 != NULL && px2 != NULL && px3 != NULL && px4 != NULL);

        /* compute average chromas and cosine coefficients */
        Pnm_chromas_cos_f new_elem = (Pnm_chromas_cos_f)elem;
        new_elem->a = ((px4->y + px3->y + px2->y + px1->y) / 4);
        new_elem->b = ((px4->y + px3->y - px2->y - px1->y) / 4);
        new_elem->c = ((px4->y - px3->y + px2->y - px1->y) / 4);
        new_elem->d = ((px4->y - px3->y - px2->y + px1->y) / 4);
        new_elem->pb_avg = ((px1->pb + px2->pb + px3->pb + px4->pb) / 4);
        new_elem->pr_avg = ((px1->pr + px2->pr + px3->pr + px4->pr) / 4);

        /* assign b, c, d values to +0.3 or -0.3 when out-of-bounds */
        if (new_elem->b > 0.3) new_elem->b = 0.3;
        if (new_elem->c > 0.3) new_elem->c = 0.3;
        if (new_elem->d > 0.3) new_elem->d = 0.3;
        if (new_elem->b < -0.3) new_elem->b = -0.3;
        if (new_elem->c < -0.3) new_elem->c = -0.3;
        if (new_elem->d < -0.3) new_elem->d = -0.3;

        (void)array2;
}

/********** cos_to_luma ********
 *
 * Apply function for converting cosine coefficients and average chromas 
 * to lumaniance values
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
 * Expects: The width and height is even;
 *          The pixmap in image  contain elements of type 
 *          struct Pnm_chromas_cos_f floats; 
 *          New pixmap is double the size of the current incoming pixmap
 *      
 * Notes: Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *        Applied as a mapping function using map_row_major
 *      
 ************************/
static void cos_to_luma(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        /* Runs when at the top right index of each 2x2 block */
        if (i % 2 == 0 && j % 2 == 0) { /* even coordinates */
                Pnm_ppm image = (Pnm_ppm)cl;
                Pnm_chromas_cos_f block = image->methods->at(image->pixels, 
                                                                i/2, j/2);
                assert(block != NULL);
                assert(elem != NULL);

                /* Get the address of each pixel in a 2x2 block */
                Pnm_cv_f px1 = image->methods->at(array2, i, j);
                Pnm_cv_f px2 = image->methods->at(array2, i + 1, j);
                Pnm_cv_f px3 = image->methods->at(array2, i, j + 1);
                Pnm_cv_f px4 = image->methods->at(array2, i + 1, j + 1);

                /* Update each pixel with lumaniance value */
                px1->y = block->a - block->b - block->c + block->d;
                px2->y = block->a - block->b + block->c - block->d;
                px3->y = block->a + block->b - block->c - block->d;
                px4->y = block->a + block->b + block->c + block->d;

                /* Update each pixel with average blue chroma */
                px1->pb = block->pb_avg;
                px2->pb = block->pb_avg;
                px3->pb = block->pb_avg;
                px4->pb = block->pb_avg;

                /* Update each pixel with average red chroma */
                px1->pr = block->pr_avg;
                px2->pr = block->pr_avg;
                px3->pr = block->pr_avg;
                px4->pr = block->pr_avg;

                (void)elem;
        }
}

/********** cos_int_float_convert ********
 *
 * Convert between integer and float representations for average chromas 
 * and cosine coefficients
 *
 * Parameters:
 *      Pnm_ppm image           Pnm_ppm containing image
 *      char *command           string containing conversion option
 *
 * Return: Pnm_ppm containing the converted image data representation
 *
 * Expects:
 *      image is not NULL
 * 
 * Notes:
 *      rgb ints to floats                    "rgb_to_float"
 *      rgb floats to ints                    "rgb_to_int"
 *      cosine coefficent floats to ints      "cos_to_int"
 *      cosine coefficent ints to floats      "cos_to_float"
 *      
 ************************/
static Pnm_ppm cos_int_float_convert(Pnm_ppm image, char *command)
{
        A2Methods_UArray2 new_pixels;
        if (strcmp(command, "cos_to_int") == 0) {
                /* Convert component video integers to floats */
                new_pixels = image->methods->new(image->width / 2, 
                                                image->height / 2,
                                        sizeof(struct Pnm_chromas_cos_int));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, cos_to_int, image);

        } else if (strcmp(command, "cos_to_float") == 0) {
                /* Convert component video floats to integers */
                new_pixels = image->methods->new(image->width / 2, 
                                                image->height / 2,
                                        sizeof(struct Pnm_chromas_cos_f));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, cos_to_float, image);
        }

        image->methods->free(&(image->pixels)); /* Free the old pixmap */
        image->pixels = new_pixels; /* Update pixels with new array */

        return image;
}

/********** cos_to_int ********
 *
 * Apply function for converting cosine floats to integers
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
 * Expects: The pixmap in image contain elements of type 
 *          struct Pnm_chromas_cos_f floats
 *      
 * Notes: Asserts that rgb is not null at index [i, j] in pixmap.
 *        Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
static void cos_to_int(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl; /* Cast to Pnm_ppm */
        assert(image != NULL);
        
        /* Get address to the element at index [i, j] in pixmap*/
        Pnm_chromas_cos_f chroma_f = image->methods->at(image->pixels, i, j);
        assert(chroma_f != NULL);
        assert(elem != NULL);

        /* Converr to various bit-sized integers */
        Pnm_chromas_cos_int chroma_int = (Pnm_chromas_cos_int)elem;
        chroma_int->a = (chroma_f->a * 63); /* Convert to 9-bit integer */
        chroma_int->b = (chroma_f->b * (31 / 0.3)); /* 5-bit integer => [-15, 15] */
        chroma_int->c = (chroma_f->c * (31 / 0.3));
        chroma_int->d = (chroma_f->d * (31 / 0.3));
        /* Convert to 4-bit integer */
        chroma_int->pb_avg = Arith40_index_of_chroma(chroma_f->pb_avg);
        chroma_int->pr_avg = Arith40_index_of_chroma(chroma_f->pr_avg);

        (void)array2;
}

/********** cos_to_float ********
 *
 * Apply function for converting cosine integers to floats
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
 * Expects: The pixmap in image contain elements of type 
 *          struct Pnm_chromas_cos_int integers
 *      
 * Notes: Asserts that rgb is not null at index [i, j] in pixmap.
 *        Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
static void cos_to_float(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl; /* Cast to Pnm_ppm */
        assert(image != NULL);

        /* Get address to the element at index [i, j] in pixmap*/
        Pnm_chromas_cos_int chroma_int = image->methods->at(image->pixels, 
                                                                i, j);
        assert(chroma_int != NULL);
        assert(elem != NULL);

        /* Convert to float values */
        Pnm_chromas_cos_f chroma_f = (Pnm_chromas_cos_f)elem;
        chroma_f->a = (float)chroma_int->a / 63; 
        chroma_f->b = (float)chroma_int->b / (31 / 0.3); /* Between [-0.3, +0.3] */
        chroma_f->c = (float)chroma_int->c / (31 / 0.3); /* Between [-0.3, +0.3] */
        chroma_f->d = (float)chroma_int->d / (31 / 0.3); /* Between [-0.3, +0.3] */
        chroma_f->pb_avg = Arith40_chroma_of_index(chroma_int->pb_avg);
        chroma_f->pr_avg = Arith40_chroma_of_index(chroma_int->pr_avg);

        (void)array2;
}
