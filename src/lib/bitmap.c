/* 
 * bitmap.c -- Bitboard-like structures 
 *
 * Copyright (C) 2012 Jason Linehan 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "bitmap.h"


/*
 * Return an allocated BITMAP large enough for an nrows x ncols
 */
struct matrix_t *new_bitmap(int nrows, int ncols)
{
        struct matrix_t *new;

        new = new_matrix(nrows, BITNSEGS(ncols));

        return (new);
}


