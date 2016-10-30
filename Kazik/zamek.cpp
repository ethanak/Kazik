#include "Game.h"
#include "zamek.h"

#define TATAR_SPEED 7

enum e_ladder {
    LADDER_FREE = 0, // nic nie ma
    LADDER_LEFT, // tatar idzie w lewo
    LADDER_RIGHT, // tatar idzie w prawo
    LADDER_GROW, // drabina rośnie
    LADDER_BUSY, // tatar idzie do góry
    LADDER_FAIL, // tatar dostał kamieniem
    LADDER_UNGROW // kamień spada na rozkładaną drabinę
};

void Kazik::init(int8_t ng)
{
    dir = 0;
    xpos = 42;
    stone = 0;
    ngame = ng;
    int8_t i;
    for (i=0; i< 9;i++) {
        ladder_status[i] = 0;
        ladder_pos[i] = random(10,50);
    }
    ladderNum = 1;
}

static PROGMEM const uint8_t effect_sg[]={
    1, 5,
    100, 110, 120, 133, 150};

static PROGMEM const uint8_t effect_sd[]={
1, 20,
    110, 105, 100, 96, 92, 90, 88, 86, 85, 84,
     82,  80,  78, 76, 74 ,72 ,70 ,68 ,66 ,64};
static PROGMEM const uint8_t effect_tup[]={
1, 1, 64};
static PROGMEM const uint8_t effect_tup2[]={
1, 1, 74};

static PROGMEM const uint8_t kobza[] = {
    18, 32,
    1, 24, 1, 27, 1, 27, 1, 27,
    1, 27, 1, 31, 2, 29,
    4, 27,
    1, 27, 1, 31, 1, 31, 1, 31,
    1, 31, 1, 34, 1, 34, 1, 31,
    1, 32, 1, 29, 1, 29, 1, 29,
    1, 29, 1, 31, 1, 32, 1, 29,
    1, 31, 1, 29, 1, 27, 1, 27,
    1, 27, 1, 31, 2, 29, 4, 27};



static const PROGMEM uint8_t tatarstep[] = {
    2, 5, 8, 13, 18, 25, 30, 40
};
void Kazik::kazikLoop(void)
{
    int8_t x = 0;
    int8_t i;
    uint32_t om;
    int16_t dly;
    int16_t steps = 0;
    ladderFall = -1;
    int16_t tatar = 0;
    bool failed = false;
    for (;;) {
        om = millis();
        if (steps < 32767) steps++;
        if (ladderNum < 9 && tatar >= pgm_read_byte(&tatarstep[ladderNum])) {
            ladderNum++;
        }
        i=getCommand();
        if (i < 0) return;
        if (i > 0) continue;
        display.clearDisplay();
        if (keyStatus & KSTAT_LEFT) dir = -1;
        else if (keyStatus & KSTAT_RIGHT) dir = 1;
        else if (((xpos/2) & 3) == 3) dir = 0;
        if ((xpos < 2 && dir < 0) || (xpos > 75 && dir > 0)) {
            dir = 0;
            if (!stone) {
                stone = 1;
                playEffect(effect_sg);
            }
        }
        xpos += 2 * dir;
        if (dir && !(xpos & 3)) {
            playEffect((xpos & 4) ? effect_tup : effect_tup2);
        }
        if (stone == 1 && (keyStatus & KSTAT_FWD) && (xpos & 7) == 6) {
            playEffect(effect_sd);
            stone = 2;
            stonex = (xpos - 6) / 8;
            ladderFall = -1;
            for (i = 0; i < ladderNum; i++) {
                if (ladder_x[i] == stonex && ladder_status[i] == LADDER_BUSY) {
                    ladder_status[i] = LADDER_FAIL;
                    ladderFall = i;
                    break;
                }
                else if (ladder_x[i] == stonex && ladder_status[i] == LADDER_GROW) {
                    ladder_status[i] = LADDER_UNGROW;
                    ladderFall = i;
                    break;
                }
            }
            if (ladderFall >= 0) {
                if (steps < 32767 - (20 * ladderNum)) {
                    steps += 20 * ladderNum;
                }
                else {
                    steps = 32767;
                }
                tatar += 1;
            }
        }

        drawKazik(xpos,dir, stone == 1);
        if (stone >= 2) {
            drawStone();
            stone += 3;
            if (stone >= 22) {
                stone = 0;
                if (ladderFall >= 0) {
                    ladder_status[ladderFall] = 0;
                    ladder_pos[ladderFall] = random(20,50);
                }
            }
        }


        display.drawLine(0,11,83,11,BLACK);

        for (i=0; i< ladderNum; i++) {
            if (drawFullLadder(i)) failed = true;
        }
        if (!failed) {
            if (xpos >= 38) {
                display.setCursor(0,0);
                display.print(steps);
            }
            else {
                displayInt(52,0,5,steps);
            }
        }
        display.display();
        if (failed) break;
        dly = millis() - om;
        if (dly < 100) delay(100 - dly);

    }
    playMusic(kobza);
    for (i=0; i < 100;i++) {
        getStick();
        delay(20);
    }
    if (!tatar) {
        defeat(ngame);
    }
    else {
        victory(ngame, steps);
    }
}

static const PROGMEM uint8_t kazik_f[]={
    0x00, 0x00, 0x00, 0x38, 0x38, 0xfe, 0xba, 0xba, 0x7c, 0x44, 0xc6, // front
    0x7c, 0xfe, 0x82, 0xba, 0xba, 0xfe, 0x38, 0x38, 0x7c, 0x44, 0xc6, // ufront
    0x00, 0x00, 0x00, 0x3c, 0x30, 0x38, 0x38, 0x38, 0x10, 0x10, 0x18, // right1
    0x00, 0x00, 0x00, 0x3c, 0x30, 0x38, 0x7e, 0xb8, 0x2c, 0x46, 0x60, // right2
    0x7c, 0xfe, 0x10, 0x3c, 0x30, 0x38, 0x38, 0x38, 0x10, 0x10, 0x18, // uright1
    0x7c, 0xfe, 0x10, 0x3c, 0x30, 0x38, 0x38, 0x38, 0x2c, 0x46, 0x60, // uright2
    0x00, 0x00, 0x00, 0x78, 0x18, 0x38, 0x38, 0x38, 0x10, 0x10, 0x30, // left1
    0x00, 0x00, 0x00, 0x78, 0x18, 0x38, 0xfc, 0x3a, 0x68, 0xc4, 0x86, // left2
    0x7c, 0xfe, 0x10, 0x78, 0x18, 0x38, 0x38, 0x38, 0x10, 0x10, 0x30, // uleft1
    0x7c, 0xfe, 0x10, 0x78, 0x18, 0x38, 0x38, 0x38, 0x68, 0xc4, 0x86, // uleft2

};

static const PROGMEM uint8_t tatar_fu[] {
    0x80, 0x80, 0xb8, 0x98, 0xfe, 0x3a, 0x3a, 0x7c, 0x6c, 0xc6
};

static const PROGMEM uint8_t tatar_f[] = {
    0x80, 0x80, 0xf0, 0xf0, 0x78, 0x70, 0x70, 0x50, 0x10, 0x10, 0x10, 0x10,
    0x00, 0x80, 0xf0, 0xf8, 0x78, 0x50, 0x50, 0x50, 0x50, 0x10, 0x10, 0x00,
    0x00, 0x00, 0xf8, 0xf8, 0x70, 0x70, 0x70, 0x50, 0x50, 0x50, 0x00, 0x00,
    0x00, 0x08, 0x78, 0xf8, 0xf0, 0x70, 0x70, 0x50, 0x50, 0x40, 0x40, 0x00,
    0x08, 0x08, 0x78, 0x78, 0xf0, 0xf0, 0x70, 0x50, 0x50, 0x40, 0x40, 0x00,
    0x00, 0x08, 0x78, 0xf8, 0xf0, 0x70, 0x70, 0x50, 0x50, 0x40, 0x40, 0x00,
    0x00, 0x00, 0xf8, 0xf8, 0x70, 0x70, 0x70, 0x50, 0x50, 0x50, 0x00, 0x00,
    0x00, 0x80, 0xf0, 0xf8, 0x78, 0x50, 0x50, 0x50, 0x50, 0x10, 0x10, 0x00
};

static const PROGMEM uint8_t stone_f[] = {
    0x70, 0xf8, 0x70};

void Kazik::drawKazik(int8_t x, int8_t dir, bool stone)
{
    uint8_t offset;

    //display.drawBitmap(10, 0, kazik_f + 33, 8, 11, BLACK);
    //return;

    if (!dir) {
        offset = stone ? 11 : 0;
    }
    else {
        offset = (dir > 0) ? 22 : 66;
        if (x & 2) offset += 11;
        if (stone) offset += 22;
    }

    display.drawBitmap(x, 0, kazik_f + offset, 8, 11, BLACK);
}

void Kazik::drawLadder(int8_t x, int8_t y)
{
    int i;
    display.drawLine(x,y,x,40,BLACK);
    display.drawLine(x+6,y,x+6,40,BLACK);
    for (i=13; i<40;i+=4) if (i >= y) {
        display.drawLine(x+1,i,x+5,i,BLACK);
    }
}

void Kazik::drawStone(void)
{
    display.drawBitmap(6 + stonex * 8, stone + 9, stone_f, 7, 3, BLACK);
}

void Kazik::drawTatar(int8_t x, int8_t y)
{
    int p = (y + 2) & 7;
    display.drawBitmap(x * 8 + 7, y+8, tatar_f + 12 * p, 5, 12, BLACK);

}

void Kazik::drawTatarF(int8_t x, int8_t y)
{
    display.drawBitmap(x * 8 + 7, y+8, tatar_f + 12 * 2, 5, 12, BLACK);
}
void Kazik::drawTatarU(int8_t x)
{
    display.drawBitmap(x * 8 + 6, 1, tatar_fu, 7, 10, BLACK);
}


void Kazik::drawHTatar(int8_t x, int8_t dir)
{
    int offset = 0;
    if (dir) {
        offset = (x & 2)? 33 : 22;
        if (dir < 0) offset += 44;
    }
    display.drawBitmap(x, 40, kazik_f + offset + 3, 8, 8, BLACK);
}


int Kazik::findFreeLadder(void)
{
    int8_t lno[10];
    int8_t i,n,j;
    int8_t lpl = 0;
    int8_t lpr = 0;
    for (i = 0; i < 9; i++) {
        if (ladder_status[i] == LADDER_GROW || ladder_status[i] == LADDER_UNGROW) {
            if (ladder_x[i] == 0) lpl = 1;
            if (ladder_x[i] == 8) lpr = 1;
        }
        else if (ladder_status[i] == LADDER_RIGHT) {
            if (ladder_pos[i] < 12) lpl = 1;
        }
        else if (ladder_status[i] == LADDER_LEFT) {
            if (ladder_pos[i] > 64) lpr = 1;
        }
    }
    if (lpl && lpr) return 0;
    for (i=n=0; i < 9; i++) {
        if (i < 4 && lpl) continue;
        if (i > 4 && lpr) continue;
        for (j=0; j < 9; j++) {
            if (ladder_x[j] == i && ladder_status[j] != LADDER_FREE) break;
        }
        if (j < 9) continue;
        if (i <= 4 && !lpl) {
            lno[n++] = -(i + 1);
        }
        if (i >= 4 && !lpr) {
            lno[n++] = i  +1;
        }
    }
    if (!n) return 0;
    return lno[random(n-1)];
}



bool Kazik::drawFullLadder(int8_t n)
{
    int i;
    switch(ladder_status[n]) {
        case LADDER_FREE:
        i = findFreeLadder();
        if (i) {
            if (ladder_pos[n]-- > 0) {
                return false;
            }
        }
        if (i < 0) {
            ladder_x[n] = -i - 1;
            ladder_pos[n] = 0;
            ladder_status[n] = LADDER_RIGHT;
        }
        else if (i > 0) {
            ladder_x[n] = i - 1;
            ladder_pos[n] = 76;
            ladder_status[n] = LADDER_LEFT;
        }

        return false;
        case LADDER_LEFT:
        drawHTatar(ladder_pos[n], -1);
        ladder_pos[n] -= 2;
        if (ladder_pos[n] <= 6 + 8 * ladder_x[n] && (stonex != ladder_x[n] || stone < 2)) {
            ladder_status[n] = LADDER_GROW;
            ladder_pos[n] = 30;
        }
        return false;
        case LADDER_RIGHT:
        drawHTatar(ladder_pos[n], 1);
        ladder_pos[n] += 2;
        if (ladder_pos[n] >= 6 + 8 * ladder_x[n] && (stonex != ladder_x[n] || stone < 2)) {
            ladder_status[n] = LADDER_GROW;
            ladder_pos[n] = 30;
        }
        return false;
        case LADDER_GROW:
        drawLadder(ladder_x[n] * 8 + 6, ladder_pos[n]);
        drawHTatar(ladder_x[n] * 8 + 6, 0);
        ladder_pos[n] -= 3;
        if (ladder_pos[n] < 12) {
            ladder_pos[n] = 24;
            ladder_status[n] = LADDER_BUSY;
            ladder_step[n] = 0;
        }
        return false;
        case LADDER_UNGROW:
        if (stone < 2) {
            ladder_status[n] = LADDER_FREE;
            return false;
        }
        if (stone > ladder_pos[n]) ladder_pos[n] = stone;
        drawLadder(ladder_x[n] * 8 + 6, ladder_pos[n]);
        drawHTatar(ladder_x[n] * 8 + 6, 0);
        return false;

        case LADDER_BUSY:
        drawLadder(ladder_x[n] * 8 + 6, 12);
        if (ladder_pos[n] <= 0) {
            drawTatarU(ladder_x[n]);
            return true;
        }
        drawTatar(ladder_x[n], ladder_pos[n]);
        ladder_step[n] = (ladder_step[n] + 1) % TATAR_SPEED;
        if (!ladder_step[n]) {
            ladder_pos[n] -= 1;
        }
        return false;
        case LADDER_FAIL:
        if (stone < 2) {
            ladder_status[n] = LADDER_FREE;
            return false;
        }
        if (stone > ladder_pos[n]) ladder_pos[n] = stone;
        drawLadder(ladder_x[n] * 8 + 6, ladder_pos[n]);
        drawTatarF(ladder_x[n], ladder_pos[n]);
        return false;
        default:
        ladder_status[n] = 0;
        return false;
    }
    return false;
}


