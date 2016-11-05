/*
 * maze.h - simple game platform for Arduino
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

#ifndef ETH_MAZE_H
#define ETH_MAZE_H

#include <Arduino.h>

// oddech w milisekundach
#define BREATH_TIME 1000

class Maze {
    public:
        Maze(void){};
        void init(int8_t);
        void mazeLoop(void);

    private:
        void drawCell(int8_t n, bool left, bool front, bool right, uint8_t extra, int8_t door);
        void drawMap(void);
        void drawView(void);
        void drawPanel(void);
        uint8_t cell(int8_t x, int8_t y);
        uint8_t scell(int8_t x, int8_t y);
        uint8_t cell(uint8_t xy);
        int set(uint8_t x, uint8_t y);
        void sset(uint8_t x, uint8_t y);
        void vset(uint8_t x, uint8_t y);
        uint8_t vcell(int8_t x, int8_t y);
        bool possible(int8_t, int8_t);
        int8_t cmoves(int8_t x, int8_t y, uint8_t *rc);
        void move(void);
        bool cloop(int8_t, int8_t);
        void mloop(void);
        void addStart(int8_t x, int8_t y, int8_t d);
        int8_t canStart(int8_t x, int8_t y);
        bool findStart(void);
        int8_t quadrant(int8_t x, int8_t y);
        int8_t quadrant(uint8_t xy);
        bool putItems(void);
        int8_t itemOn(int8_t x, int8_t y);
        void itemOff(int8_t x, int8_t y);
        uint16_t table[16];
        uint16_t visited[16];
        uint8_t *prs;
        uint8_t nprs;
        uint8_t startx, starty, endx, endy, startd, endd;
        int8_t items[9][2];
        int8_t nkeys, nmaps, ncmps;
        int16_t score;
        int8_t ngame;

};

#endif
