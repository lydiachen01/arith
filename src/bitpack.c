/*
 * bitpack.c
 * Lydia Chen (lchen16) & Geneva Cheng (gcheng01)
 * February 23, 2024
 *
 * CS 40 Homework 4 arith
 *
 * Implementation of bitpack
 *
 */

#include <bitpack.h>
#include <except.h>
#include <stdio.h>

#define ONE_UINT64 1
#define MAXBYTES 64

Except_T Bitpack_Overflow = {"Overflow packing bits"};

/************************** Helper Functions ************************/
uint64_t shift_left(uint64_t n, unsigned offset);
uint64_t shift_right(uint64_t n, unsigned offset);
/********************************************************************/

/********** Bitpack_fitsu ********
 *
 * Test if an unsigned int can be represented in k bits where k = width
 *
 * Parameters:
 *      uint64_t n              unsigned int to represent
 *      unsigned width          width of bit field
 *
 * Return: bool indicating if int can be represented in k bits
 *
 * Expects: none
 *
 * Notes: Raise Bitpack_Overflow when width exceed MAXBYTES
 *
 ************************/
extern bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (width > MAXBYTES)
        {
                RAISE(Bitpack_Overflow);
        }

        return (n <= (shift_left(ONE_UINT64, width) - 1));
}

/********** Bitpack_fitss ********
 *
 * Test if a signed int can be represented in k = width
 *
 * Parameters:
 *      int64_t n               signed int to represent
 *      unsigned width          width of bit field
 *
 * Return: bool indicating if int can be represented in k bits
 *
 * Expects: none
 *
 * Notes: Raise Bitpack_Overflow when width exceed MAXBYTES
 *
 ************************/
extern bool Bitpack_fitss(int64_t n, unsigned width)
{
        if (width > MAXBYTES)
        {
                RAISE(Bitpack_Overflow);
        }
        /* Find upper and lower bounds and checks if n within this range*/
        int upper_limit = shift_left(ONE_UINT64, width - 1) - 1;
        int lower_limit = -shift_left(ONE_UINT64, width - 1);
        return (n <= upper_limit && n >= lower_limit);
}

/********** Bitpack_getu ********
 *
 * Extract an unsigned int from a word
 *
 * Parameters:
 *      uint64_t word           unsigned int containing word
 *      unsigned width          width of field
 *      unsigned lsb            location of least significant bit
 *
 * Return: unsigned int from a field in the word
 *
 * Expects: none
 *
 * Notes: Raise Bitpack_Overflow when width + lsb exceed MAXBYTES
 *
 ************************/
extern uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        if (width + lsb > MAXBYTES)
        {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = shift_left((shift_left(ONE_UINT64, width) - 1), lsb);
        return shift_right((word & mask), lsb);
}

/********** Bitpack_gets ********
 *
 * Extract a signed int from a word
 *
 * Parameters:
 *      uint64_t word           unsigned int containing word
 *      unsigned width          width of field
 *      unsigned lsb            location of least significant bit
 *
 * Return: signed int from a field in the word
 *
 * Expects: none
 *
 * Notes: Raise Bitpack_Overflow when width + lsb exceed MAXBYTES
 *
 ************************/
extern int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        if (width + lsb > 64)
        {
                RAISE(Bitpack_Overflow);
        }

        int64_t result = Bitpack_getu(word, width, lsb);

        /* checks if most significant bit is a 1*/
        if (result & shift_left(ONE_UINT64, width - 1))
        {
                result |= ~(shift_left(ONE_UINT64, width) - 1);
        }

        return result;
}

/********** Bitpack_newu ********
 *
 * Update a field in a word
 *
 * Parameters:
 *      uint64_t word           unsigned int containing word
 *      unsigned width          width of field
 *      unsigned lsb            location of least significant bit
 *      uint64_t value          unsigned int containing new value
 *
 * Return: word containing the updated field
 *
 * Expects: none
 *
 * Notes: Raise Bitpack_Overflow when width + lsb exceed MAXBYTES or if
 *        Bitpack_fitsu does not return true
 *
 ************************/
extern uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                             uint64_t value)
{
        if (width + lsb > MAXBYTES || Bitpack_fitsu(value, width) == 0)
        {
                RAISE(Bitpack_Overflow);
        }

        /* create a mask of 0s */
        uint64_t mask = shift_right(~0ULL, (MAXBYTES - width));
        mask = ~shift_left(mask, lsb); /* shift mask by lsb and invert */
        word = word & mask;            /* take intersection of word and mask */
        /* it empties a space to intersect new value */

        value = shift_left(value, lsb);

        return (word | value); /* concatenate word with new value*/
}

/********** Bitpack_news ********
 *
 * Update a field in a word
 *
 * Parameters:
 *      uint64_t word           unsigned int containing word
 *      unsigned width          width of field
 *      unsigned lsb            location of least significant bit
 *      int64_t value           signed int containing new value
 *
 * Return: word containing the updated field
 *
 * Expects: none
 *
 * Notes: Raise Bitpack_Overflow when width + lsb exceed MAXBYTES or if
 *        Bitpack_fitss does not return true
 *
 ************************/
extern uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                             int64_t value)
{
        if (width + lsb > MAXBYTES || Bitpack_fitss(value, width) == 0)
        {
                RAISE(Bitpack_Overflow);
        }

        if (value > 0)
        { /* if value is positive, run Bitpack_newu */
                return Bitpack_newu(word, width, lsb, value);
        }

        /* create a mask of 0s */
        uint64_t mask = shift_right(~0ULL, (MAXBYTES - width));
        mask = ~shift_left(mask, lsb); /* shift mask by lsb and invert */
        word = word & mask;            /* take intersection of word and mask */
        /* it empties a space to intersect the new value */

        uint64_t leading_one = ~0; /* initalize a 64-bit of 1s */
        leading_one = shift_right(leading_one, (MAXBYTES - (width)));
        /* shift right until 1s only cover the word */
        /* all bits to the left of width are 0s */

        /* take intersection of word and mask */
        value = value & leading_one;
        value = shift_left(value, lsb); /* shift value to correct position */

        return (word | value); /* concatenate word with new value */
}

/********** shift_left ********
 *
 * Shift the binary representation of an unsigned int left by some offset;
 * if the offset is 64, we return 0;
 *
 * Parameters:
 *      uint64_t n              unsigned int to represent
 *      unsigned offset         shift amount
 *
 * Return: unsigned int with bits shifted left by offset
 *
 * Expects: The offset is be at most 64
 *
 * Notes: none
 *
 ************************/
uint64_t shift_left(uint64_t n, unsigned offset)
{
        if (offset == MAXBYTES)
        {
                return 0;
        }
        return (n << offset);
}

/********** shift_right ********
 *
 * Shift the binary representation of an unsigned int right by some offset
 * if the offset is 64, we return 0;
 *
 * Parameters:
 *      uint64_t n              unsigned int to represent
 *      unsigned offset         shift amount
 *
 * Return: unsigned int with bits shifted right by offset
 *
 * Expects: The offset is be at most 64
 *
 * Notes: none
 *
 ************************/
uint64_t shift_right(uint64_t n, unsigned offset)
{
        if (offset == MAXBYTES)
        {
                return 0;
        }
        return (n >> offset);
}
