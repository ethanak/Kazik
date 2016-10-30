#include "Game.h"
#include "warsztat.h"

static PROGMEM const uint8_t smieci_t[]={
0x00, 0x10, 0x40, 0x06, 0x00, 0x00, 0x24, 0x00,
0x00, 0x08, 0x00, 0x40, 0x40, 0x40, 0x0c, 0x00,
0x00, 0x00, 0x00, 0x14, 0x22, 0x00, 0x60, 0x00,
0x00, 0x02, 0x20, 0x00, 0x10, 0x00, 0x38, 0x00,

0x1c, 0x91, 0x90, 0xc1, 0x06, 0x03, 0x09, 0x52,
0x11, 0x2d, 0x90, 0x07, 0x01, 0x2d, 0x41, 0x0a,
0x64, 0x01, 0x22, 0x18, 0x9a, 0x01, 0x1d, 0x29,
0x16, 0x9a, 0x38, 0x00, 0xa7, 0x0c, 0x10, 0x0c,
};
static PROGMEM const uint8_t narzedzia_t[]={
0x24, 0xc3, 0x24, 0x18, 0x24, 0x42, 0xc3, 0xc3,
0xe0, 0xf0, 0xf0, 0x78, 0x18, 0x04, 0x02, 0x01,
0x48, 0xcc, 0xc9, 0xff, 0x79, 0x31, 0x30, 0xe0,
0x0c, 0x78, 0x70, 0xf8, 0x4c, 0x06, 0x03, 0x01,
0x48, 0xcc, 0xf8, 0x70, 0x18, 0x0c, 0x06, 0x03,
};

static PROGMEM const uint8_t kazik_f[]={
    0x38, 0x38, 0xfe, 0xba, 0xba, 0x7c, 0x44, 0xc6, // front
    0x3c, 0x30, 0x38, 0x38, 0x38, 0x10, 0x10, 0x18, // right1
    0x3c, 0x30, 0x38, 0x7e, 0xb8, 0x2c, 0x46, 0x60, // right2
    0x78, 0x18, 0x38, 0x38, 0x38, 0x10, 0x10, 0x30, // left1
    0x78, 0x18, 0x38, 0xfc, 0x3a, 0x68, 0xc4, 0x86, // left2
};

static PROGMEM const uint8_t kazik_v[]={
    0x38, 0x38, 0xfe, 0xba, 0xba, 0x7c, 0x4c, 0x40, 0x40, 0x40, 0xc0, // front1
    0x38, 0x38, 0xfe, 0xba, 0xba, 0x7c, 0xc4, 0x04, 0x04, 0x04, 0x06 // front2
};

static PROGMEM const uint8_t spider_f[]={

0x00, 0xdb, 0x3c, 0xff, 0x18, 0x3c, 0xc3, 0x00,
0xc0, 0x58, 0x3f, 0x38, 0xd8, 0xf8, 0x06, 0x03,
0x03, 0x1a, 0xfc, 0x1c, 0x1b, 0x1f, 0x60, 0xc0,
0x00, 0xc3, 0x3c, 0x18, 0xff, 0x3c, 0xdb, 0x00,
0x03, 0x06, 0xf8, 0xd8, 0x38, 0x3f, 0x58, 0xc0,
0xc0, 0x60, 0x1f, 0x1b, 0x1c, 0xfc, 0x1a, 0x03,
0x52, 0x52, 0x34, 0x7c, 0x7c, 0x34, 0x52, 0x52,
0x8c, 0xcc, 0x34, 0x7c, 0x7c, 0x22, 0x23, 0x21,
0x21, 0x23, 0x22, 0x7c, 0x7c, 0x34, 0xcc, 0x8c,
0x4a, 0x4a, 0x2c, 0x3e, 0x3e, 0x2c, 0x4a, 0x4a,
0x31, 0x33, 0x2c, 0x3e, 0x3e, 0x44, 0xc4, 0x84,
0x84, 0xc4, 0x44, 0x3e, 0x3e, 0x2c, 0x33, 0x31,
};

static PROGMEM const uint8_t exit_f[]={
    0xff, 0x81, 0x81, 0x99, 0x99, 0x81, 0x81, 0xff,
    0x00, 0x00, 0x3c, 0x24, 0x24, 0x3c, 0x00, 0x00,
    0x00, 0x7e, 0x42, 0x42, 0x42, 0x42, 0x7e, 0x00
};

void Warsztat::init(int8_t ng)
{
    ngame = ng;
}

uint8_t Warsztat::cell(int8_t x, int8_t y)
{
    return (tabela[y] & (1 << x)) ? 1 : 0;

}

void Warsztat::dell(int8_t x, int8_t y)
{
    tabela[y] &= ~ (1<<x);
}

void Warsztat::newWorkshop(void)
{
    memset(tabela,0xff,32);
    xpos = 0;
    ypos = 0;
    dell(0,0);
    exit_x = 5;
    exit_y = 3;
    dell(5,3);
    int8_t i;
    ntools = N_TOOLS;
    for (i=0; i< ntools;) {
        int8_t x, y;
        x=random(16);
        y=random(16);
        if (abs(x-xpos) + abs(y-ypos) < 5) continue;
        if (!cell(x,y)) continue;
        dell(x,y);
        tool_x[i] = x;
        tool_y[i] = y;
        tool_types[i] = random(1,6);
        i++;
    }
}


void Warsztat::drawTool(int8_t n)
{
    if (!tool_types[n]) return;
    int8_t dx = tool_x[n] * 8 - xposd;
    if (dx <= -8) return;
    if (dx >= 84) return;
    int8_t dy = tool_y[n] * 8 - yposd;
    if (dy < -8) return;
    if (dy >=40) return;
    const uint8_t *c =  narzedzia_t + 8 * (tool_types[n] - 1);
    int8_t s = 8;

    if (dy < 0) {
        s += dy;
        c += - dy;
        dy = 0;
    }

    display.drawBitmap(dx,dy+8,c,8,s,BLACK);
}

void Warsztat::drawExit(void)
{
    static int8_t phase = 0;
    int8_t dx = exit_x * 8 - xposd;
    if (dx <= -8) return;
    if (dx >= 84) return;
    int8_t dy = exit_y * 8 - yposd;
    if (dy < -8) return;
    if (dy >=40) return;
    const uint8_t *c =  exit_f + 8 * phase;
    phase = (phase + 1) % 3;
    int8_t s = 8;

    if (dy < 0) {
        s += dy;
        c += - dy;
        dy = 0;
    }

    display.drawBitmap(dx,dy+8,c,8,s,BLACK);
}


void Warsztat::drawCell(int8_t x, int8_t y)
{
    if (!cell(x,y)) return;
    int8_t dx = x * 8 - xposd;
    if (dx <= -8) return;
    if (dx >= 84) return;
    int8_t dy = y * 8 - yposd;
    if (dy < -8) return;
    if (dy >=40) return;
    int8_t n = (17*x + 21*y) & 3;
    const uint8_t *c =  smieci_t + 8 * n;
    int8_t s = 8;

    if (dy < 0) {
        s += dy;
        c += - dy;
        dy = 0;
    }
    display.drawBitmap(dx,dy+8,c,8,s,BLACK);
}

void Warsztat::drawSpider(int8_t x,int8_t y,int8_t dir, int8_t phase)
{
    int8_t dx = x - xposd;
    if (dx <= -8) return;
    if (dx >= 84) return;
    int8_t dy = y - yposd;
    if (dy < -8) return;
    if (dy >=40) return;
    if (phase == 3) phase = 2;
    else if (phase == 2) phase = 0;
    int8_t n = 3 * dir + phase;
    const uint8_t *c =  spider_f + 8 * n;
    int8_t s = 8;

    if (dy < 0) {
        s += dy;
        c += - dy;
        dy = 0;
    }
    display.drawBitmap(dx,dy+8,c,8,s,BLACK);
}

void Warsztat::drawKazik(void)
{
    int16_t x = xpos - xposd;
    int16_t y = ypos - yposd;
    const uint8_t *c;
    int8_t len=8;
    if (!dir) {
        c = kazik_f;
    }
    else if (dir <= 2) {
        c = kazik_f + 8 + 16 * (dir - 1);
        if (xpos & 2) {
            c += 8;
        }
    }
    else {
        if (!(ypos & 2)) c = kazik_f;
        else {
            c = (ypos & 4) ? kazik_v: kazik_v + 10;
            len=10;
        }
    }
    display.drawBitmap(x,y+8,c,8,len,BLACK);
}

void Warsztat::drawWorkshop(void)
{
    int8_t x,y,i;
    xposd = xpos - 38;
    yposd = ypos - 16;
    if (xposd < 0) xposd = 0;
    else if (xposd > 128 - 84) xposd = 128 - 84;
    if (yposd < 0) yposd = 0;
    else if (yposd > 128 - 40) yposd = 128 - 40;
    display.clearDisplay();
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16;y++) {
            drawCell(x,y);
        }
    }
    for (i=0;i<ntools;i++) {
        drawTool(i);
    }
    drawExit();
    drawKazik();
    static int8_t p = 0, pu = 0;
    drawSpider(16, 16, pu/20, p);
    p = (p + 1) % 4;
    pu = (pu + 1) % 80;
    display.display();
}

static PROGMEM const int8_t edx[]={0,1,-1,0,0};
static PROGMEM const int8_t edy[]={0,0,0,-1,1};

void Warsztat::warLoop(void)
{
    int8_t i;
    newWorkshop();
    dir = 0;
    for (;;) {
        getStick();
        if (!keyStatus) break;
        delay(10);
    }
    for (;;) {
        i = getCommand();
        if (i < 0) return;
        if (xpos > 0 && (keyStatus & KSTAT_LEFT) && !(ypos & 7)) dir = 2;
        else if (xpos < 120 && (keyStatus & KSTAT_RIGHT) && !(ypos & 7)) dir = 1;
        else if (ypos > 0 && (keyStatus & KSTAT_FWD) && !(xpos & 7)) dir = 3;
        else if (ypos < 120 && (keyStatus & KSTAT_BACK) && !(xpos & 7)) dir = 4;
        else if (!(xpos & 7) && !(ypos & 7))dir = 0;
        xpos += 2 * (int8_t)pgm_read_byte(&edx[dir]);
        ypos += 2 *(int8_t)pgm_read_byte(&edy[dir]);
        if (!xpos && dir == 2) dir = 0;
        if (xpos == 120 && dir == 1) dir = 0;
        if (ypos == 0 && dir == 3) dir = 0;
        if (ypos == 120 && dir == 4) dir = 0;
        int8_t cx = xpos/8, cy = ypos/8;
        if (cell(cx,cy)) {
            dell(cx, cy);
        }
        for (i=0;i<ntools;i++) {
            if (tool_types[i] && tool_x[i] == cx && tool_y[i] == cy) {
                tool_types[i] = 0;
            }
        }
        drawWorkshop();
        delay(100);
    }
}
