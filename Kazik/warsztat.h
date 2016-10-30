#ifndef ETH_WARSZTAT_H
#define ETH_WARSZTAT_H

#include <Arduino.h>

#define N_TOOLS 32

class Warsztat
{
    public:
        Warsztat(void){};
        void init(int8_t);
        void warLoop(void);



    private:
        uint8_t cell(int8_t x, int8_t y);
        void dell(int8_t x, int8_t y);
        void drawWorkshop(void);
        void drawExit(void);
        void newWorkshop(void);
        void drawCell(int8_t x, int8_t y);
        void drawKazik(void);
        void drawTool(int8_t);
        void drawSpider(int8_t x,int8_t y,int8_t dir, int8_t phase);


        uint16_t tabela[16];
        int8_t ngame;
        int16_t xpos;
        int16_t ypos;
        int dir; // 1 prawo; 2 - lewo; 3- góra; 4 - dół
        int16_t xposd;
        int16_t yposd;
        uint8_t tool_types[N_TOOLS];
        uint8_t tool_x[N_TOOLS];
        uint8_t tool_y[N_TOOLS];
        int8_t ntools;
        int8_t exit_x, exit_y;

};

#endif
