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

static PROGMEM const uint8_t bag_f [] = {
0x3c, 0x42, 0x8d, 0x9d, 0xb9, 0xb1, 0x42, 0x3c,
0x3c, 0x42, 0xb1, 0xb9, 0x9d, 0x8d, 0x42, 0x3c,
0x3c, 0x42, 0x99, 0x99, 0x99, 0x99, 0x42, 0x3c,
0x3c, 0x42, 0x81, 0xbd, 0xbd, 0x81, 0x42, 0x3c,
};

void Warsztat::init(int8_t ng)
{
    ngame = ng;
    score = 0;
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
    exit_x = random(5,16);
    exit_y = random(5,ysize);
    dell(5,3);
    int8_t i;
    ntools = ysize; //N_TOOLS;
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
    nbags = ysize; //N_TOOLS;
    for (i=0; i< ntools;) {
        int8_t x, y;
        x=random(16);
        y=random(ysize);
        if (abs(x-xpos) + abs(y-ypos) < 5) continue;
        if (!cell(x,y)) continue;
        dell(x,y);
        bag_x[i] = x * 8;
        bag_y[i] = y * 8;
        bag_dir[i] = 0;
        i++;
    }
    nspider = ysize/2; //N_SPID;
    for (i=0; i < nspider;) {
        int8_t x, y;
        x=random(16);
        y=random(ysize);
        if (abs(x-xpos) + abs(y-ypos) < 5) continue;
        if (!cell(x,y)) continue;
        dell(x,y);
        spider_x[i] = x * 8;
        spider_y[i] = y * 8;
        spider_d[i] = 0;
        i++;
    }

}


bool Warsztat::compXY(int8_t xin, int8_t yin, int8_t *xout, int8_t *yout)
{
    *xout = xin - xposd;
    *yout = yin - yposd;
    return (*xout <= -8) || (*xout >= 84) || (*yout < -8) || (*yout >= 40);
}


void Warsztat::drawTool(int8_t n)
{
    if (!tool_types[n]) return;
    int8_t dx, dy;
    if (compXY(tool_x[n] * 8, tool_y[n] * 8, &dx, &dy)) return;
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
    int8_t dx, dy;
    if (compXY(exit_x * 8, exit_y * 8, &dx, &dy)) return;
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
    int8_t dx, dy;
    if (compXY(x * 8, y * 8, &dx, &dy)) return;
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

void Warsztat::drawSpider(int8_t n)
{
    int8_t dx, dy, dir, phase;
    if (spider_d[n] == -1) return;
    if (compXY(spider_x[n], spider_y[n], &dx, &dy)) return;
    dir = spider_d[n] & 15;
    if (dir & 8) dir = 0;
    if (!dir) {
        phase = ((spider_d[n] >> 4) + 1 ) & 3;
        spider_d[n] = (spider_d[n] & 15) | (phase << 4);
    }
    else {
        phase = (spider_x[n] | spider_y[n]) & 3;
    }
    if (phase == 3) phase = 2;
    else if (phase == 2) phase = 0;
    dir = 0;
    int8_t nr = 3 * dir + phase;
    const uint8_t *c =  spider_f + 8 * nr;
    int8_t s = 8;

    if (dy < 0) {
        s += dy;
        c += - dy;
        dy = 0;
    }

    display.drawBitmap(dx,dy+8,c,8,s,BLACK);
}

#define BAG_STABLE 0
#define BAG_RIGHT 1
#define BAG_LEFT 2
#define BAG_DOWN 3
#define BAG_WAIT 4

void Warsztat::drawBag(int8_t n)
{
    int8_t dx, dy;
    if (compXY(bag_x[n], bag_y[n], &dx, &dy)) return;
    int8_t s = 8;
    const uint8_t *c =  bag_f + 8 * ((n + bag_x[n]/2) & 3);
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
    else if (yposd > (ysize * 8) - 40) yposd = (ysize * 8) - 40;
    display.clearDisplay();
    for (x = 0; x < 16; x++) {
        for (y = 0; y < ysize;y++) {
            drawCell(x,y);
        }
    }
    for (i=0;i<ntools;i++) {
        drawTool(i);
    }
    for (i=0; i<nbags;i++) {
        drawBag(i);
    }
    for (i=0; i<nspider; i++) {
        drawSpider(i);
    }
    drawExit();
    drawKazik();
    displayScore();


    display.display();
}

void Warsztat::displayScore(void)
{
    displayInt(48,0,5,score);
    int8_t i;
    for (i=0; i<lives; i++) {
        display.drawBitmap(i*9,0,kazik_f, 8, 8, BLACK);
    }
}

static PROGMEM const int8_t edx[]={0,1,-1,0,0};
static PROGMEM const int8_t edy[]={0,0,0,-1,1};
static PROGMEM const int8_t sdx[]={0,1,0,-1,0};
static PROGMEM const int8_t sdy[]={0,0,1,0,-1};

static PROGMEM const int8_t tup1_e[] = {1,1,64};
static PROGMEM const int8_t tup2_e[] = {1,1,74};
static PROGMEM const int8_t tool_e[] = {1, 5, 58, 65, 73, 87, 110};
static PROGMEM const uint8_t spider_e[] = {1, 10, 100, 95,90,85,80,77,74,72,71,70};
/*
 * Pająk zatrzymany - 0x08
 * Pająk może się ruszyć: jeśli zatrzymany, czeka na 0x48
 *
 * Pająk może się skierować w daną stronę, jeśli:
 * - jest w obrębie warsztatu
 * - nie ma śmieci
 * - nie ma narzędzia
 * - nie zbliży się do innego pająka o 8 lub mniej
 * - nie zbiży się do worka na tym samym poziomie o 8 lub mniej
 *
 * Preferowany kierunek to ten, gdzie jest Kazik
 * Pająk wyczuwa Kazika z odległości 48 lub w dowolnej po prostej
 * bez przeszkód
 */

void Warsztat::moveSpider(int8_t n)
{

    if (spider_d[n] < 0) return;
    int8_t asa[4],nt,i,j;
    int8_t dir = spider_d[n] & 15;
    if (dir & 8) dir = 0;
    if ((spider_x[n] | spider_y[n]) & 7) {
        spider_x[n] += 2 * pgm_read_byte(&sdx[dir]);
        spider_y[n] += 2 * pgm_read_byte(&sdy[dir]);
        return;
    }
    nt = 0;
    for (i = 1; i <= 4; i++) {
        int8_t x = spider_x[n] / 8 + pgm_read_byte(&sdx[i]);
        int8_t y = spider_y[n] / 8 + pgm_read_byte(&sdy[i]);
        if (x < 0 || x > 15 || y < 0 || y >= ysize) continue;
        if (cell(x,y)) continue;
        for (j=0; j< ntools; j++) {
            if (!tool_types[j]) continue;
            if (tool_x[j] == x && tool_y[j] == y) break;
        }
        if (j < ntools) continue;
        x = spider_x[n] + 2 * pgm_read_byte(&sdx[i]);
        y = spider_y[n] + 2 * pgm_read_byte(&sdy[i]);
        for (j = 0; j < nspider;j ++) {
            if (j == n || spider_d[j] < 0) continue;
            if (abs(x - spider_x[j]) + abs(y - spider_y[j]) < 8) {
                break;
            }
        }
        if (j < nspider) continue;
        for (j = 0; j < nbags; j++) {
            if (abs(x - bag_x[j]) + abs(y - bag_y[j]) < 8) {
                break;
            }
        }

        if (j < nbags) continue;
        asa[nt++]=i;
    }
    if (!nt) {
        //Serial.print("Stopped ");
        //Serial.println(n);
        spider_d[n] = (spider_d[n] & 0xf0) | 0x08;
        return;
    }
    if (spider_d[n] & 0x08) {
        //Serial.print(n);Serial.print(" ");Serial.println(spider_d[n],HEX);
        if ((spider_d[n] & 7) != 7) {
            spider_d[n] = (spider_d[n] & 0xf8) |
                ((spider_d[n] + 1) & 0x7);
            Serial.print(n);Serial.print(" ");Serial.println(spider_d[n]);
            return;
        }
    }

    spider_d[n] = asa[random(nt)];
    spider_x[n] += 2 * pgm_read_byte(&sdx[spider_d[n]]);
    spider_y[n] += 2 * pgm_read_byte(&sdy[spider_d[n]]);
    return;
}

void Warsztat::warLoop(void)
{
    int8_t level;
    lives = 3;
    for (level = 0; level < 3; level++) {
        int8_t rc = levelLoop(level);
        if (rc < 0) return;
        if (!rc) {
            defeat(ngame);
            return;
        }
    }
    victory(ngame, score);
}

/*
 * -1 - pauza
 * 0 - przegrana
 * 1 - przejście
 */
int8_t Warsztat::levelLoop(int8_t level)
{
    int8_t i,*effect;
    bool eforce;
    ysize = 8 + 4 * level;
    newWorkshop();
    dir = 0;
    for (;;) {
        getStick();
        if (!keyStatus) break;
        delay(10);
    }
    for (;;) {
        i = getCommand();
        if (i < 0) return -1;
        effect = NULL;
        eforce = false;
        if (xpos > 0 && (keyStatus & KSTAT_LEFT) && !(ypos & 7)) dir = 2;
        else if (xpos < 120 && (keyStatus & KSTAT_RIGHT) && !(ypos & 7)) dir = 1;
        else if (ypos > 0 && (keyStatus & KSTAT_FWD) && !(xpos & 7)) dir = 3;
        else if (ypos < (ysize - 1) * 8 && (keyStatus & KSTAT_BACK) && !(xpos & 7)) dir = 4;
        else if (!(xpos & 7) && !(ypos & 7))dir = 0;

        // czy mogę tam pójść?
        if (dir && !(xpos & 7) && !(ypos & 7)) {
            int8_t nx = xpos + 8 * (int8_t)pgm_read_byte(&edx[dir]);
            int8_t ny = ypos + 8 *(int8_t)pgm_read_byte(&edy[dir]);

            for (i=0;i<nbags;i++) {
                if (bag_dir[i] == 0 || (bag_dir[i] & BAG_WAIT)) { // nie rusza się
                    if (bag_x[i] == nx && bag_y[i] == ny) {
                        if (dir == 1 || dir == 2) {
                            dir = 0;
                        }
                        else {
                            dir = 0;
                        }
                        break;
                    }
                }
            }
        }

        xpos += 2 * (int8_t)pgm_read_byte(&edx[dir]);
        ypos += 2 *(int8_t)pgm_read_byte(&edy[dir]);
        for (i=0; i<nspider;i++) {
            if (dir && spider_d[i] >= 0 &&
                (abs(xpos - spider_x[i]) + abs(ypos-spider_y[i])) <= 4) {
                    spider_d[i] = -1;
                    score -= 100;
                    if (score < 0) score = 0;
                    effect = spider_e;
                    lives--;
            }
            moveSpider(i);
        }

        if (dir) {
            int8_t vx, vy;
            int8_t pts = -1;
            if (!effect && !((xpos | ypos) & 2)) effect = ((xpos | ypos) & 4)?tup1_e : tup2_e;
            if (dir == 1) {
                vx = (xpos + 7) / 8;
                vy = ypos / 8;
            }
            else if (dir == 2) {
                vx = xpos / 8;
                vy = ypos / 8;
            }
            else if (dir == 3) {
                vx =xpos / 8; vy = ypos / 8;
            }
            else {
                vx = xpos / 8; vy = (ypos + 7) / 8;
            }
            if (cell(vx, vy)) {
                dell(vx, vy);
                pts = 10;
            }
            for (i=0; i< ntools; i++) {
                if (tool_types[i] && tool_x[i] == vx && tool_y[i] == vy) {
                    tool_types[i] = 0;
                    effect = tool_e;
                    eforce = true;
                    pts = 50;
                }
            }
            score += pts;
            if (score < 0) score = 0;
            else if (score > 30000) score = 30000;
        }



        if (!xpos && dir == 2) dir = 0;
        if (xpos == 120 && dir == 1) dir = 0;
        if (ypos == 0 && dir == 3) dir = 0;
        if (ypos == (ysize - 1) * 8 && dir == 4) dir = 0;


        if (effect) playEffect(effect, eforce);
        drawWorkshop();
        delay(100);
        if (lives < 0) return 0;
    }
    return -1;
}
