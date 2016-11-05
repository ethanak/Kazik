/*
 * warsztat.h - simple game platform for Arduino
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

#ifndef ETH_WARSZTAT_H
#define ETH_WARSZTAT_H

#include <Arduino.h>

#define N_TOOLS 32
#define N_BAGS 32
#define N_SPID 8

class Warsztat
{
    public:
        Warsztat(void){};
        void init(int8_t);
        void warLoop(void);



    private:
        bool cell(int8_t x, int8_t y);
        void dell(int8_t x, int8_t y);
        void drawWorkshop(void);
        void drawExit(void);
        void newWorkshop(void);
        void drawCell(int8_t x, int8_t y);
        void drawKazik(void);
        void drawTool(int8_t);
        void drawBag(int8_t);
        void drawSpider(int8_t);
        void moveSpider(int8_t);
        bool compXY(int8_t xin, int8_t yin, int8_t *xout, int8_t *yout);
        void displayScore(void);
        int8_t levelLoop(int8_t);
        bool bagFieldFree(int8_t n, int8_t dir);
        bool pushBag(int8_t, int8_t);
        void moveBag(int8_t n);


        uint16_t tabela[16];
        int8_t ngame;
        int16_t xpos;
        int16_t ypos;
        int dir; // 1 prawo; 2 - lewo; 3 - góra; 4 - dół
        int16_t xposd;
        int16_t yposd;
        uint8_t tool_types[N_TOOLS];
        uint8_t tool_x[N_TOOLS];
        uint8_t tool_y[N_TOOLS];
        int8_t ntools;
        uint8_t bag_x[N_BAGS];
        uint8_t bag_y[N_BAGS];
        uint8_t bag_dir[N_BAGS];
        int8_t nbags;
        uint8_t spider_x[N_SPID];
        uint8_t spider_y[N_SPID];
        int8_t spider_d[N_SPID];
        int8_t nspider;
        int16_t score;
        int8_t ysize;
        int8_t lives;
        uint8_t level;
        int8_t gtools;

        int8_t exit_x, exit_y;

};

#endif
