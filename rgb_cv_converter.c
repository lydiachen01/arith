 /*
 * rgb_cv_converter.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of rgb_cv_converter which converts between the 
 * RGB float values and the component video (Y, Pb, Pr) color space float 
 * values
 *
 */

#include "rgb_cv_converter.h"


/****************************** Structs ****************************/
/* A struct that stores rgb float values */
typedef struct Pnm_rgb_f {
        float red, green, blue;
} *Pnm_rgb_f;

/* A struct that stores component video float values */
typedef struct Pnm_cv_f { 
        float y, pb, pr;
} *Pnm_cv_f;
/********************************************************************/


/************************** Helper Functions ************************/
/* Helper functions to convert between rgb and component representations */
static void rgb_to_cv(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);
static void cv_to_rgb(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl);

/* Helper functions to convert between integer and float respresentation */
static Pnm_ppm rgb_int_float_convert(Pnm_ppm image, char *command);
static void rgb_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
static void rgb_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, 
                void *cl);
/********************************************************************/


/********** rgb_cv_convert ********
 *
 * convert between rgb and cv representations
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
 *      rgb to cv               "to_cv"
 *      cv to rgb               "to_rgb"
 *      
 ************************/
Pnm_ppm rgb_cv_convert(Pnm_ppm image, char *command) 
{
        A2Methods_UArray2 new_pixels;
        if (strcmp(command, "rgb_to_cv") == 0) {
                /* Convert rgb scaled integers to floats */
                image = rgb_int_float_convert(image, "rgb_to_float");
                
                /* convert to component video */
                new_pixels = image->methods->new(image->width, 
                                                image->height, 
                                                sizeof(struct Pnm_cv_f));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, rgb_to_cv, image);
                image->methods->free(&(image->pixels));
                image->pixels = new_pixels;

        } else if (strcmp(command, "cv_to_rgb") == 0) {                
                /* convert to component video */
                new_pixels = image->methods->new(image->width, 
                                                image->height, 
                                                sizeof(struct Pnm_rgb_f));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, cv_to_rgb, image);
                image->methods->free(&(image->pixels));
                image->pixels = new_pixels;

                /* Convert rgb floats to integers*/
                image = rgb_int_float_convert(image, "rgb_to_int"); 
        }

        return image;
}

/********** rgb_to_cv ********
 *
 * apply function for converting rgb to cv
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
 *          struct Pnm_rgb_f floats
 *      
 * Notes: Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
static void rgb_to_cv(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl;
        Pnm_rgb_f rgb = image->methods->at(image->pixels, i, j);
        assert(rgb != NULL);
 
        Pnm_cv_f cv_f = (Pnm_cv_f)elem;
        cv_f->y = 0.299 * rgb->red + 0.587 * rgb->green 
                        + 0.114 * rgb->blue;
        cv_f->pb = -0.168736 * rgb->red - 0.331264 * rgb->green 
                        + 0.5 * rgb->blue;
        cv_f->pr = 0.5 * rgb->red - 0.418688 * rgb->green 
                        - 0.081312 * rgb->blue;
        
        (void)array2;
}

/********** cv_to_rgb ********
 *
 * apply function for converting cv to rgb
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
 *          struct Pnm_cv_f floats
 *      
 * Notes: Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
static void cv_to_rgb(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl;
        Pnm_cv_f cv_f = image->methods->at(image->pixels, i, j);
        assert(cv_f != NULL);

        Pnm_rgb_f rgb = (Pnm_rgb_f)elem;
        rgb->red = 1.0 * cv_f->y + 0.0 * cv_f->pb + 1.402 * cv_f->pr;
        rgb->green = 1.0 * cv_f->y - 0.344136 * cv_f->pb 
                        - 0.714136 * cv_f->pr;
        rgb->blue = 1.0 * cv_f->y + 1.772 * cv_f->pb + 0.0 * cv_f->pr;

        (void)array2;
}

/********** rgb_int_float_convert ********
 *
 * convert between int and float representations for RGB values
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
static Pnm_ppm rgb_int_float_convert(Pnm_ppm image, char *command)
{
        A2Methods_UArray2 new_pixels;
        if (strcmp(command, "rgb_to_float") == 0) {
                /* Convert rgb integers to floats */
                new_pixels = image->methods->new(image->width, 
                                                image->height,
                                                sizeof(struct Pnm_rgb_f));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, rgb_to_float, image);
        
        } else if (strcmp(command, "rgb_to_int") == 0) {
                /* Convert rgb floats to integers */
                new_pixels = image->methods->new(image->width, 
                                                image->height,
                                                sizeof(struct Pnm_rgb));
                assert(new_pixels != NULL);
                image->methods->map_row_major(new_pixels, rgb_to_int, image);
        
        }

        image->methods->free(&(image->pixels)); /* Free the old pixmap */
        image->pixels = new_pixels; /* Update pixels with new array */

        return image;
}

/********** rgb_to_float ********
 *
 * apply function for converting rgb ints to floats
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
 * Expects: The pixmap in image will contain elements of type 
 *          struct Pnm_rgb integers
 *      
 * Notes: Asserts that rgb is not null at index [i, j] in pixmap.
 *        Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
static void rgb_to_float(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl; /* Cast to Pnm_ppm */
        assert(image != NULL);
        
        /* Get address to the element at index [i, j] in pixmap*/
        Pnm_rgb rgb = image->methods->at(image->pixels, i, j); 
        assert(rgb != NULL);
        assert(elem != NULL);

        Pnm_rgb_f rgb_f = (Pnm_rgb_f)elem; /* Cast to Pnm_rgb_f */
        /* Compute floating point values*/
        rgb_f->red = (float)rgb->red / image->denominator;
        rgb_f->green = (float)rgb->green / image->denominator;
        rgb_f->blue = (float)rgb->blue / image->denominator;
        
        (void)array2;
}

/********** rgb_to_int ********
 *
 * apply function for converting rgb floats to ints
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
 * Expects: The pixmap in image will contain elements of type 
 *          struct Pnm_rgb_f floats
 *      
 * Notes: Asserts that rgb is not null at index [i, j] in pixmap.
 *        Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
static void rgb_to_int(int i, int j, A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{        
        Pnm_ppm image = (Pnm_ppm)cl; /* Cast to Pnm_ppm */
        assert(image != NULL);

        /* Get address to the element at index [i, j] in pixmap*/
        Pnm_rgb_f rgb_f = image->methods->at(image->pixels, i, j);
        assert(rgb_f != NULL);
        assert(elem != NULL);

        Pnm_rgb rgb = (Pnm_rgb)elem; /* Cast to Pnm_rgb */
        /* Compute integer values*/
        rgb->red = rgb_f->red * image->denominator;
        rgb->green = rgb_f->green * image->denominator;
        rgb->blue = rgb_f->blue * image->denominator;

        (void)array2;
}
