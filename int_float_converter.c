/*
 * int_float_converter.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of int_float_converter
 * Converted between integer and floating-point representations of 
 * image data (e.g. rbg, component video, and cosine coefficients).
 * 
 * [NOTE:] This file was used to test that our conversion from integer to 
 * float was correct. In the actual implementation, we combine the conversion 
 * functions with their corresponding transformations (e.g. rbg_to_cv, etc).
 * It was later removed from Makefile for test due to overlapping function 
 * names and the unit tests created were later deleted to test the final 
 * 40compress program.
 *
 */

#include "int_float_converter.h"
#include "pnm_types.h"

/********** int_float_convert ********
 *
 * convert between int and float representations
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
Pnm_ppm int_float_convert(Pnm_ppm image, char *command)
{
        A2Methods_UArray2 new_pixels;
        if (strcmp(command, "rgb_to_float") == 0) {
                /* Convert rgb integers to floats */
                new_pixels = image->methods->new(image->width, 
                                                image->height,
                                                sizeof(struct Pnm_rgb_f));
                image->methods->map_row_major(new_pixels, rgb_to_float, image);
        
        } else if (strcmp(command, "rgb_to_int") == 0) {
                /* Convert rgb floats to integers */
                new_pixels = image->methods->new(image->width, 
                                                image->height,
                                                sizeof(struct Pnm_rgb));
                image->methods->map_row_major(new_pixels, rgb_to_int, image);
        
        } else if (strcmp(command, "cos_to_int") == 0) {
                /* Convert component video integers to floats */
                new_pixels = image->methods->new(image->width / 2, 
                                                image->height / 2,
                                        sizeof(struct Pnm_chromas_cos_int));
                image->methods->map_row_major(new_pixels, cos_to_int, image);

        } else if (strcmp(command, "cos_to_float") == 0) {
                /* Convert component video floats to integers */
                new_pixels = image->methods->new(image->width / 2, 
                                                image->height / 2,
                                        sizeof(struct Pnm_chromas_cos_f));
                image->methods->map_row_major(new_pixels, cos_to_float, image);
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
void rgb_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
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
void rgb_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
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

/********** cos_to_int ********
 *
 * apply function for converting cos floats to ints
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
 *          struct Pnm_chromas_cos_f floats
 *      
 * Notes: Asserts that rgb is not null at index [i, j] in pixmap.
 *        Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
void cos_to_int(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl; /* Cast to Pnm_ppm */
        assert(image != NULL);
        
        /* Get address to the element at index [i, j] in pixmap*/
        Pnm_chromas_cos_f chroma_f = image->methods->at(image->pixels, i, j);
        assert(chroma_f != NULL);
        assert(elem != NULL);

        /* Cast to Pnm_chromas_cos_int */
        Pnm_chromas_cos_int chroma_int = (Pnm_chromas_cos_int)elem;
        chroma_int->a = (chroma_f->a * 511); /* Convert to 9-bit integer */
        chroma_int->b = (chroma_f->b * 50); /* 5-bit integer => [-15, 15] */
        chroma_int->c = (chroma_f->c * 50);
        chroma_int->d = (chroma_f->d * 50);
        /* Convert to 4-bit integer */
        chroma_int->pb_avg = Arith40_index_of_chroma(chroma_f->pb_avg);
        chroma_int->pr_avg = Arith40_index_of_chroma(chroma_f->pr_avg);

        (void)array2;
}

/********** cos_to_float ********
 *
 * apply function for converting cos ints to floats
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
 *          struct Pnm_chromas_cos_int integers
 *      
 * Notes: Asserts that rgb is not null at index [i, j] in pixmap.
 *        Asserts that image is not NULL
 *        Asserts that elem in not NULL 
 *      
 ************************/
void cos_to_float(int i, int j, A2Methods_UArray2 array2, void *elem, void *cl)
{
        Pnm_ppm image = (Pnm_ppm)cl; /* Cast to Pnm_ppm */
        assert(image != NULL);

        /* Get address to the element at index [i, j] in pixmap*/
        Pnm_chromas_cos_int chroma_int = image->methods->at(image->pixels, 
                                                                i, j);
        assert(chroma_int != NULL);
        assert(elem != NULL);

        /* Cast to Pnm_chromas_cos_f */
        Pnm_chromas_cos_f chroma_f = (Pnm_chromas_cos_f)elem;
        chroma_f->a = (float)chroma_int->a / 511; 
        chroma_f->b = (float)chroma_int->b / 50; /* Between [-0.3, +0.3] */
        chroma_f->c = (float)chroma_int->c / 50; /* Between [-0.3, +0.3] */
        chroma_f->d = (float)chroma_int->d / 50; /* Between [-0.3, +0.3] */
        chroma_f->pb_avg = Arith40_chroma_of_index(chroma_int->pb_avg);
        chroma_f->pr_avg = Arith40_chroma_of_index(chroma_int->pr_avg);

        (void)array2;
}
