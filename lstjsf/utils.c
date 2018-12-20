#include "jsf2.h"

/*    get_double()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE double precision
 *    floating point numbers (8 bytes) in files
 *    transferred from a mc680xx machine.
 *
 */

double get_double(unsigned char *inbuf, int location)
{
    int i;
    unsigned char *ptr1, *ptr3;
    double *ptr2;
    double value;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (double));

    if(BIG)
        {
        ptr1 += location;
        for(i = 0; i < 8; i++)
              *ptr3++ = *ptr1++;
        }
      else
        {
        ptr1 += location;
        for(i = 0; i < 8; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 8;

    ptr2 = (double *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*    get_float()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE single precision
 *    floating point numbers (4 bytes) in files
 *    transferred from a mc680xx machine.
 */

float get_float(unsigned char *inbuf, int location)
{
    int i;
    unsigned char *ptr1, *ptr3;
    float *ptr2;
    float value;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (float));

    if(BIG)
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 4;

    ptr2 = (float *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*    get_int()
 *
 *    Subroutine to enable Intel based processors
 *    (little-endian) to read IEEE integer
 *    numbers (4 bytes) in files
 *    transferred from a mc680xx machine and vice versa.
 */

int get_int(unsigned char *buf, int location)
{
    int i;
    unsigned char *ptr1, *ptr3;
    int *ptr2;
    int value;

    ptr1 = buf;
    ptr3 = (unsigned char *) calloc (1, sizeof (int));

    if(BIG)
        {
        ptr1 += (location + 3);
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 4; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 4;

    ptr2 = (int *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

/*    get_short()
 *
 *    Subroutine to enable Intel based processors to
 *    read short integer numbers (2 bytes)
 *    in files transferred from a mc680xx machine.
 *
 */

short get_short(unsigned char *inbuf, int location)
{
    int i;
    unsigned char *ptr1, *ptr3;
    short value;
    short *ptr2;

    ptr1 = inbuf;
    ptr3 = (unsigned char *) calloc (1, sizeof (short));

    if(BIG)
        {
        ptr1 += (location + 1);
        for(i = 0; i < 2; i++)
              *ptr3++ = *ptr1--;
        }
    else
        {
        ptr1 += location;
        for(i = 0; i < 2; i++)
              *ptr3++ = *ptr1++;
        }

    ptr3 -= 2;

    ptr2 = (short *) ptr3;

    value = *ptr2;

    free(ptr3);

    return(value);
}

