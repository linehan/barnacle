/*

        Morton ordering -- morton.h

        ---   ---       Morton order, also called Z-order, is the name
          /  /  /       of a space-filling curve first described in 1966
         /  /  /        by G.M. Morton, in a report titled "A computer
        ---'  ---       Oriented Geodetic Data Base; and a New Technique
            ,-'`        in File Sequencing."
        ,-'``           
       ---   ---        The ordering is easily described once the idea of
         /  /  /        binary encoding has been grasped, as it is simply
        /  /  /         the interleave of the binary representations of 2
       ---'  ---        integers (e.g. x and y).




-.   ,-;"`-:-.   ,-;"`-:-.   ,-;"`-:-.   ,-;"`-:-.   ,-;"`-:-.   ,-;"`-:-.   ,-;
`=`,'=/     `=`,'=/     `=`,'=/     `=`,'=/     `=`,'=/     `=`,'=/     `=`,'=/ 
  y==/        y==/        y==/        y==/        y==/        y==/        y==/  
,=,-<=`.    ,=,-<=`.    ,=,-<=`.    ,=,-<=`.    ,=,-<=`.    ,=,-<=`.    ,=,-<=`.
-'   `-=_,-'-'   `-=_,-'-'   `-=_,-'-'   `-=_,-'-'   `-=_,-'-'   `-=_,-'-'   `*/
                                                                              


#include "morton.h"



/*
 * mort -- compute the Z-index of two values (y, x)
 * @y: unsigned 32-bit integer y value
 * @x: unsigned 32-bit integer x value
 *
 *      ---- ---- ---- ---- FEDC BA98 7654 3210
 *      ---- ---- FEDC BA98 ---- ---- 7654 3210
 *      ---- FEDC ---- BA98 ---- 7654 ---- 3210
 *      --FE --DC --BA --98 --76 --54 --32 --10
 *      -F-E -D-C -B-A -9-8 -7-6 -5-4 -3-2 -1-0
 */
uint32_t mort(uint32_t y, uint32_t x)
{
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;

        y = (y | (y << 8)) & 0x00FF00FF;
        y = (y | (y << 4)) & 0x0F0F0F0F;
        y = (y | (y << 2)) & 0x33333333;
        y = (y | (y << 1)) & 0x55555555;

        return (x | (y << 1));
}




/*
 * trom -- given a z-index z, return un-interleaved value (even or odd)
 * @z: unsigned 32-bit integer z-index value
 *
 *      -F-E -D-C -B-A -9-8 -7-6 -5-4 -3-2 -1-0
 *      --FE --DC --BA --98 --76 --54 --32 --10
 *      ---- FEDC ---- BA98 ---- 7654 ---- 3210
 *      ---- ---- FEDC BA98 ---- ---- 7654 3210
 *      ---- ---- ---- ---- FEDC BA98 7654 3210
 */
uint32_t trom(uint32_t z)
{
        z &= 0x55555555;                 
        z = (z ^ (z >> 1)) & 0x33333333;
        z = (z ^ (z >> 2)) & 0x0F0F0F0F;
        z = (z ^ (z >> 4)) & 0x00FF00FF;
        z = (z ^ (z >> 8)) & 0x0000FFFF;

        return z;
}




/*
 * tromx -- call trom() with no offset, to receive x-value 
 * @z: unsigned 32-bit integer z-index value
 */
uint32_t tromx(uint32_t z)
{
  return trom(z >> 0);
}




/*
 * tromy -- call trom() with offset of 1, to receive y-value 
 * @z: unsigned 32-bit integer z-index value
 */
uint32_t tromy(uint32_t z)
{
  return trom(z >> 1);
}

