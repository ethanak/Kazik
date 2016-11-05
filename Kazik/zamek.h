/*
 * zamek.h - simple game platform for Arduino
 * Copyright (C) Bohdan R. Rau 2016 <ethanak@polip.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#ifndef ETH_ZAMEK_H
#define ETH_ZAMEK_H

#include <Arduino.h>

class Kazik {
    public:
        Kazik(void){};
        void init(int8_t);
        void kazikLoop(void);
    private:
        void drawKazik(int8_t pos, int8_t dir, bool stone);
        void drawLadder(int8_t x, int8_t y);
        void drawTatar(int8_t x, int8_t y);
        void drawTatarF(int8_t x, int8_t y);
        void drawTatarU(int8_t x);
        void drawHTatar(int8_t x, int8_t dir);
        void drawStone(void);
        bool drawFullLadder(int8_t n);
        int findFreeLadder(void);
        int8_t dir;
        int8_t xpos;
        int8_t stone;
        int8_t stonex;
        int8_t ladder_status[9];
        int8_t ladder_pos[9];
        int8_t ladder_x[9];
        int8_t ladder_step[9];
        int8_t ladderNum;
        int8_t ngame;
        int8_t ladderFall;


};

#endif
