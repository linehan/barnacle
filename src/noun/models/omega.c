/* 
 * omega.c -- Omega noun class.
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
/****************************************************************************** 
 * MODEL: Omega 
 * NOTES: Test unit for combat and collision
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
struct ani_t *omega_mv_u;
struct ani_t *omega_mv_d;
struct ani_t *omega_mv_l;
struct ani_t *omega_mv_r;
struct ani_t *omega_die; 
struct ani_t *omega_roll;
struct ani_t *omega_hit1;
struct ani_t *omega_hit2;
/*
ᘐᘑᘒᘓᘔᘕᘖᘤᘥᘦᘧᘪᘫᘬᘭᘮᘯᘰᘱᘲᘳᘶᘷᘸᘹ
 ᘯᘪᘯᘭ℧
*/
static void build_omega_animations(void)
{
        static bool built = false;

        if (built)
                return;

        omega_hit1  = mk_ani(L"ⰹⰹⰶⰶⰶⰆⰆⰆⰹⰹⰉ", 0, 0);
        omega_hit2  = mk_ani(L"ⰹⰹⰆⰆⰆⰶⰶⰶⰆⰆⰆⰹⰹⰉ", 0, 0);
        omega_mv_u = mk_ani(L"ᘯᘤᘥᘦᘪᘪᘪᘯ", 4, 'u');
        omega_mv_d = mk_ani(L"ᘯᘤᘥᘦᘪᘪᘪᘯ", 4, 'd');
        omega_mv_l = mk_ani(L"ᘯᘤᘥᘦᘪᘪᘪᘯ", 4, 'l');
        omega_mv_r = mk_ani(L"ᘯᘤᘥᘦᘪᘪᘪᘯ", 4, 'r');
        omega_die  = mk_ani(L"ⰊⰊⰊⰊⰉ✶✶✺✺✺❁❁✴៹៹ᔊᔊⸯⸯ", 0, 0);

        built = true;
}
