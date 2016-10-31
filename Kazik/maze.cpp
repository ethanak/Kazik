#include "Game.h"
#include "maze.h"


#define MEXT_KEY 1
#define MEXT_MAP 2
#define MEXT_CMP 3
#define MEXT_VISITED 4

#define MAZE_SIZE 16

static PROGMEM const uint8_t bm_key[] = {
    0xcc, 0x0e, 0xcc, 0x11, 0xff, 0xf1, 0x00, 0x11, 0x00, 0x0e};
static PROGMEM const uint8_t bm_cmp[] = {
    0x0f, 0xf0, 0x70, 0x4e, 0x81, 0x81, 0x72, 0x0e, 0x0f, 0xf0};
static PROGMEM const uint8_t bm_map[] = {
    0xbb, 0xc4, 0x89, 0x5f, 0xeb, 0xf1, 0x2e, 0x9c, 0xfb, 0x87};

uint8_t Maze::cell(int8_t x, int8_t y)
{
    return (table[y] & (1 << x)) ? 1 : 0;
}

uint8_t Maze::vcell(int8_t x, int8_t y)
{
    return (visited[y] & (1 << x)) ? 1 : 0;
}

uint8_t Maze::scell(int8_t x, int8_t y)
{
    if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return 0;
    return (table[y] & (1 << x)) ? 1 : 0;
}

int Maze::set(uint8_t x, uint8_t y)
{
    table[y] |= 1 << x;
    if (nprs >= 160) return true;
    prs[nprs++] = x | (y << 4);
    return false;

}
void Maze::sset(uint8_t x, uint8_t y)
{
    table[y] |= 1 << x;
}
void Maze::vset(uint8_t x, uint8_t y)
{
    visited[y] |= 1 << x;
}

uint8_t Maze::cell(uint8_t xy)
{
    return (table[(xy >> 4) & 15] & (1 << (xy & 15))) ? 1 : 0;
}

void Maze::init(int8_t ng)
{
    uint8_t ps[160];
    prs = ps;
    int x, y;
    ngame = ng;
    memset(visited, 0, 32);
    for (;;) {
        nprs = 0;
        memset(table, 0, 32);
        x=random(15);
        y=random(15);
        set(x, y);
        move();
        mloop();
        if (findStart()) break;
    }
    nkeys = nmaps = ncmps = 0;
    vset(startx, starty);
    score=250;
}

bool Maze::possible(int8_t x, int8_t y) {
    int8_t n = 0, i ,dx,dy, ex, ey, edx, edy;
    if (cell(x,y)) {
        return false;
    }
    for (i = 0; i < 4; i++) {
        dx = (i & 1) ? 0 : (i - 1);
        dy = (i & 1) ? (i - 2) : 0;

        ex = x + dx;
        if (ex < 0 || ex >= MAZE_SIZE) continue;
        ey = y + dy;
        if (ey < 0 || ey >= MAZE_SIZE) continue;
        if (cell(ex, ey)) {
            if (n) {
                return false;
            }
            n = 1;
            edx = dx;
            edy = dy;
        }
    }
    if (!n) return false;
    ex = x;
    ey = y;
    for (i=0; i<4; i++) {
        ex += edx;
        ey += edy;
        if (!scell(ex,ey)) {
            return true;
        }
    }
    return false;
}

int8_t Maze::cmoves(int8_t x, int8_t y, uint8_t *rc)
{
    int nrc = 0,i,dx,dy, ex, ey;
    for (i = 0; i < 4; i++) {
        dx = (i & 1) ? 0 : (i - 1);
        dy = (i & 1) ? (i - 2) : 0;
        ex = x + dx;
        if (ex < 0 || ex >= MAZE_SIZE) continue;
        ey = y + dy;
        if (ey < 0 || ey >= MAZE_SIZE) continue;
        if (possible(ex, ey)) {
            rc[nrc++] = ex | (ey << 4);
        }
    }
    return nrc;
}

void Maze::move(void)
{
    uint8_t rc[4], x, y;
    int n, nrc;

    while (nprs) {
        int n = random(nprs - 1);
        x = prs[n] & 15;
        y = (prs[n] >> 4 ) & 15;
        nrc = cmoves(x, y, rc);
        if (!nrc) {
            nprs--;
            for (; n < nprs; n++) {
                prs[n] = prs[n+1];
            }
            continue;
        }
        n = random(nrc - 1);
        x=rc[n] & 15;
        y = (rc[n] >> 4) & 15;
        if (set(x, y)) break;
    }
}

bool Maze::cloop(int8_t x, int8_t y)
{
    int8_t dx, dy, ix, iy, i;
    if (cell(x,y)) return false;
    if (scell(x-1,y) && scell(x+1,y) && !scell(x,y-1) && !scell(x,y+1)) {
        dx = 1;
        dy = 0;
    }
    else if (!scell(x-1,y) && !scell(x+1,y) && scell(x,y-1) && scell(x,y+1)) {
        dx = 0;
        dy = 1;
    }
    else return false;
    ix = x;
    iy = y;
    for (i=0; i<4; i++) {
        ix += dx;
        iy += dy;
        if (!scell(ix, iy)) {
            break;
        }
    }
    ix = x;
    iy = y;
    for (; i<4; i++) {
        ix -= dx;
        iy -= dy;
        if (!scell(ix, iy)) {
            break;
        }
    }
    return i < 4;
}

void Maze::mloop(void)
{
    int8_t x, y;
    for (x=0; x < MAZE_SIZE; x++) {
        for (y = 0; y < MAZE_SIZE; y++) {
            if (cloop(x,y)) {
                sset(x, y);
            }
        }
    }
}


void Maze::addStart(int8_t x, int8_t y, int8_t d)
{
    if (nprs < 160) {
        prs[nprs++] = x | (y << 4);
        prs[nprs++] = d;
    }
}

int8_t Maze::canStart(int8_t x, int8_t y)
{
    int8_t n=0, ex, ey, d;
    if (!cell(x,y)) return -1;
    if (scell(x-1,y)) {n = 1; ex=x-1; ey = y; d = 3;}
    if (scell(x+1,y)) {if (n) return -1; n = 1; ex=x+1; ey = y; d = 1;}
    if (scell(x,y-1)) {if (n) return -1; n = 1; ex=x; ey = y-1; d = 0;}
    if (scell(x,y+1)) {if (n) return -1; n = 1; ex=x; ey = y+1; d = 2;}
    if (!n) return -1;
    n = 0;
    if (scell(ex-1,ey)) n++;
    if (scell(ex+1,ey)) n++;
    if (scell(ex,ey-1)) n++;
    if (scell(ex,ey+1)) n++;
    if (n < 3) return -1;
    return d;
}

int8_t Maze::quadrant(int8_t x, int8_t y)
{
    if (x < MAZE_SIZE/2 && y < MAZE_SIZE/2) return 0;
    if (x >= MAZE_SIZE/2 && y < MAZE_SIZE/2) return 1;
    if (x < MAZE_SIZE/2 && y >= MAZE_SIZE/2) return 3;
    return 2;
}

int8_t Maze::quadrant(uint8_t xy)
{
    return quadrant(xy & 15, (xy >> 4) & 15);
}


bool Maze::findStart(void)
{
    int8_t x, y, d;
    uint8_t  i, j, tprs;
    nprs = 0;
    for (x = 0; x < MAZE_SIZE; x++) {
        if ((d=canStart(x, 0)) >= 0) addStart(x,0,d);
        if ((d=canStart(x, MAZE_SIZE-1)) >= 0) addStart(x,MAZE_SIZE-1,d);
    }
    for (y = 1; y < MAZE_SIZE-1; y++) {
        if ((d=canStart(0, y)) >= 0) addStart(0,y,d);
        if ((d=canStart(MAZE_SIZE-1, y)) >= 0) addStart(MAZE_SIZE-1, y,d);
    }
    if (nprs < 4) return false;
    tprs = nprs;
    for (i = 0; i < nprs && tprs < 160; i+= 2) {
        d=quadrant(prs[i]) ^ 2;
        for (j = 0; j < nprs && tprs < 160; j+=2) {
            if (i != j && quadrant(prs[j]) == d) {
                prs[tprs++] = i;
                prs[tprs++] = j;
            }
        }
    }
    if (tprs == nprs) return false;
    i=nprs + (random(tprs - nprs) & 0xfe);
    j=prs[i];
    startd = prs[j+1] ^ 2;
    j=prs[j];
    startx = j & 15;
    starty = (j >> 4) & 15;
    j=prs[i+1];
    endd = prs[j+1];
    j=prs[j];
    endx = j & 15;
    endy = (j >> 4) & 15;
    return putItems();
}


void Maze::drawCell(int8_t n, bool left, bool front, bool right,
    uint8_t extra, // przedmiot na polu
    int8_t door // 0 - brak, 1 - lewo, 2 - prosto, 3 - w prawo
)
{
    int8_t ws[]={5,8,12,18,21};
    n = 3 - n;
    int8_t x = 2 * ws[n];
    int8_t x2 = 2 * ws[n+1];
    int8_t h = ws[n];
    int8_t h2 = ws[n+1];
    if (front) {
        display.drawLine(42 - x, 20 - h, 42 + x, 20 - h, BLACK);
        display.drawLine(42 - x, 20 + h, 42 + x, 20 + h, BLACK);
    }
    display.drawLine(
        42 - x2, 20 - (left ? h2 : h), 42 - x, 20 - h, BLACK);
    display.drawLine(
        42 - x, 20 - h, 42 - x,  20  + h, BLACK);
    display.drawLine(
        42 - x, 20 + h, 42 - x2, 20 + (left ? h2 : h), BLACK);
    display.drawLine(
        42 + x2, 20 - (right ? h2 : h), 42 + x, 20 - h, BLACK);
    display.drawLine(
        42 + x, 20 - h, 42 + x,  20  + h, BLACK);
    display.drawLine(
        42 + x, 20 + h, 42 + x2, 20 + (right ? h2 : h), BLACK);

    if (front && door == 2) {
        int8_t xd = (x * 7)/10;
        int8_t yd = (h * 7)/10;
        display.drawLine(
            42 - xd, 20 + h, 42 - xd, 20 - yd, BLACK);
        display.drawLine(
            42 - xd, 20 - yd, 42 + xd, 20 - yd, BLACK);
        display.drawLine(
            42 + xd, 20 + h, 42 + xd, 20 - yd, BLACK);
    }
    if (left && door == 1) {
        if (n < 3) {
            int8_t xd1 = (x+2);
            int8_t xd2 = (x2 - 2);
            int8_t yd1 = (h + 1);
            int8_t yd2 = (h2 - 1);
            int8_t yd3 = (yd1 * 7) /10;
            int8_t yd4 = (yd2 * 7) / 10;
            display.drawLine(
                42 - xd1, 20 + yd1, 42 - xd1, 20 - yd3, BLACK);
            display.drawLine(
                42 - xd1, 20 - yd3, 42 - xd2, 20 - yd4, BLACK);
            display.drawLine(
                42 - xd2, 20 + yd2, 42 - xd2, 20 - yd4, BLACK);
        }
        else {
            int8_t xd1 = (x + 3);
            int8_t xd2 = 42;
            int8_t yd1 = (h + 1);
            int8_t yd3 = (yd1 * 7) / 10;
            int8_t yd4 = (yd3 + 1);
            display.drawLine(
                42 - xd1, 20 - yd3, 42 - xd2, 20 - yd4, BLACK);
            display.drawLine(
                42 - xd1, 20 + yd1, 42 - xd1, 20 - yd3, BLACK);
        }
    }
    if (right && door == 3) {
        if (n < 3) {
            int8_t xd1 = (x+2);
            int8_t xd2 = (x2 - 2);
            int8_t yd1 = (h + 1);
            int8_t yd2 = (h2 - 1);
            int8_t yd3 = (yd1 * 7) /10;
            int8_t yd4 = (yd2 * 7) / 10;
            display.drawLine(
                42 + xd1, 20 + yd1, 42 + xd1, 20 - yd3, BLACK);
            display.drawLine(
                42 + xd1, 20 - yd3, 42 + xd2, 20 - yd4, BLACK);
            display.drawLine(
                42 + xd2, 20 + yd2, 42 + xd2, 20 - yd4, BLACK);
        }
        else {
            int8_t xd1 = (x + 3);
            int8_t xd2 = 42;
            int8_t yd1 = (h + 1);
            int8_t yd3 = (yd1 * 7) / 10;
            int8_t yd4 = (yd3 + 1);
            display.drawLine(
                42 + xd1, 20 - yd3, 42 + xd2, 20 - yd4, BLACK);
            display.drawLine(
                42 + xd1, 20 + yd1, 42 + xd1, 20 - yd3, BLACK);
        }
    }



    if (extra && n < 3) {
        int h3 = (h + h2) / 2;
        if (extra == MEXT_VISITED) {
            display.drawLine(42 - x / 4, 20 + h3, 42 + x/4, 20 + h3, BLACK);
        }
        else if (n < 2) {
            display.drawLine(42 - x / 3, 20 + h3, 42 + x/3, 20 + h3, BLACK);
            display.drawLine(42 - x / 3, 17 + h3 - n, 42 + x/3, 17 + h3 - n, BLACK);
        }
        else {
            display.drawLine(42 - x / 3, 20 + h3, 42 + x/3, 20 + h3, BLACK);
            display.drawBitmap(35, 26,
                (extra == MEXT_KEY) ? bm_key :
                (extra == MEXT_MAP) ? bm_map :bm_cmp, 16, 5, BLACK);
        }

    }
}

bool Maze::putItems(void)
{
    int8_t x,y,i,j,n;
    nprs = 0;
    for (x = 0; x < 16; x++) {
        for (y = n = 0; y < 16; y++) {
            if (!cell(x,y)) continue;
            if (x == startx && y == starty) continue;
            if (x == endx && y == endy) continue;
            if (scell(x-1,y)) {n = 1;}
            if (scell(x+1,y)) {if (n) continue; n = 1;}
            if (scell(x,y-1)) {if (n) continue; n = 1;}
            if (scell(x,y+1)) {if (n) continue; n = 1;}
            set(x,y);
        }
    }
    if (nprs < 9) return false;
    if(nprs > 9) {
        for (i = 0; i < nprs && nprs >= 9;) {
            x = prs[i] & 15;
            y = (prs[i] >> 4) & 15;
            if (abs(x - startx) + abs(y - starty) > 6) {
                i++;
                continue;
            }
            nprs --;
            for (j=i; j < nprs; j++) prs[j] = prs[j+1];
        }
    }
    for (i = 0; i< 9;i++) {
        n = random(nprs-1);
        items[i][0] = prs[n] & 15;
        items[i][1] = (prs[n] >> 4) & 15;
        nprs --;
        for (j=n; j < nprs; j++) prs[j] = prs[j+1];
    }
    return true;
}

void Maze::itemOff(int8_t x, int8_t y)
{
    int8_t i;
    for (i=0; i< 9; i++) {
        if (items[i][0] == x && items[i][1] == y) {
            items[i][0] = -1;
            items[i][1] = -1;
            return;
        }
    }
}


int8_t Maze::itemOn(int8_t x, int8_t y)
{
    int8_t i;
    for (i=0; i< 9; i++) {
        if (items[i][0] == x && items[i][1] == y) {
            return (i < 5) ? MEXT_KEY :
                   (i < 7) ? MEXT_CMP : MEXT_MAP;
        }
    }
    if (vcell(x,y)) return MEXT_VISITED;
    return 0;
}

static int8_t dxs[]={0, -1, 0, 1};
static int8_t dys[]={1, 0, -1, 0};

void Maze::drawView(void)
{
    int8_t x, y, i, left, right, fwd, door;
    x = startx;
    y = starty;
    for (i=0; i<4; i++) {
        left = !scell(x + dxs[(startd + 3) & 3], y + dys[(startd + 3) & 3]);
        right = !scell(x + dxs[(startd + 1) & 3], y + dys[(startd + 1) & 3]);
        fwd = !scell(x + dxs[startd], y+dys[startd]);
        if (x == endx && y == endy) {
            door = ((startd - endd) & 3) ^ 2;
        }
        else {
            door = 0;
        }
        drawCell(i, left, fwd, right, itemOn(x,y), door);
        if (fwd) break;
        x += dxs[startd];
        y += dys[startd];
    }
}

void Maze::drawMap(void)
{
    int8_t x;
    int8_t y;
    for (x = 0; x < 16; x++) {
        for (y = 0; y < 16; y++) {
            if (scell(x, y)) {
                display.fillRect(3*x+18,3*y,3,3,BLACK);
            }
        }
    }
    if ((millis() % 1000) >= 500) {
        display.drawLine(3*startx+18+1,3*starty,3*startx+18+1, 3*starty+2, WHITE);
        display.drawPixel(3*endx + 18 + 1, 3 * endy + 1, WHITE);
    }
    else {
        display.drawLine(3*startx+18,3*starty + 1,3*startx+18+2, 3*starty+1, WHITE);
        display.drawRect(3*endx + 18, 3 * endy, 3, 3, WHITE);
    }
}

static PROGMEM const uint8_t effect_l[] = {1,3,100,110,120};
static PROGMEM const uint8_t effect_r[] = {1,3,120,110,100};
static PROGMEM const uint8_t effect_g[] = {1,7,120,80,110,90,105,95,100};
static PROGMEM const uint8_t effect_f[] = {2,1,60};

// Åses død
static PROGMEM const uint8_t aza[] = {
    18, 14,
    0x82, 16, 0x82, 21, 0x84, 23,
    0x82, 16, 0x82, 21, 0x84, 23,
    0x82, 24, 0x82, 23, 0x82, 21, 0x81, 23, 0x81, 24,
    0x82, 26, 0x82, 24, 0x84, 23};

static PROGMEM const uint8_t anitra[] = {
    9, 18,
    1, 22, 1, 21, 1, 22, 1, 24, 1, 25, 1, 27,
    1, 29, 1, 34, 1, 29, 1, 27, 1, 25, 1, 24,
    1, 22, 1, 24, 1, 25, 1, 27, 2, 29, 2, 34 };

void Maze::mazeLoop(void)
{
    uint8_t mode, finito = 0;
    uint32_t last_breath = millis(), breath;
    mode = 0;
    for (;;) {
        breath = millis() - last_breath;
        int32_t i=getCommand();
        if (i < 0) return;
        if (i > 0) {
            last_breath = millis() - breath;
            mode = 0;
        }
        if (keyStatus & KSTAT_BDOWN) {
            if (nmaps) mode = 2;
        }
        else if (keyStatus & KSTAT_BUP) {
            mode = 0;
        }
        else if (keyStatus & KSTAT_LDOWN) {
            startd = (startd + 3) & 3;
            playEffect(effect_l);
            mode = 0;
        }
        else if (keyStatus & KSTAT_RDOWN) {
            startd = (startd + 1) & 3;
            playEffect(effect_r);
            mode = 0;
        }
        else if (keyStatus & KSTAT_FDOWN) {
            int8_t x,y,n;
            // jesli koniec i drzwi prosto to break
            if (startx == endx && starty == endy && startd == endd && nkeys > 2) {
                finito = 1;break;
            }

            x = startx + dxs[startd];
            y = starty + dys[startd];
            if (scell(x,y)) {
                startx = x;
                starty = y;
                mode = 0;
                n=itemOn(x,y);
                if (n > 0 && n < MEXT_VISITED) {
                    itemOff(x,y);
                    if (n == MEXT_KEY) {
                        if (nkeys < 5) nkeys++;
                        score += 50;
                    }
                    else if (n == MEXT_MAP) {
                        nmaps = 1;
                        score += 100;
                    }
                    else if (n == MEXT_CMP) {
                        ncmps = 1;
                        score += 150;
                    }
                    playEffect(effect_g);

                }
                else playEffect(effect_f);
                vset(x,y);
                score--;
                if (score <= 0) { finito = 0; break;}
            }
        }
        breath = millis() - last_breath;
        if (breath > BREATH_TIME) {
            last_breath = millis();
            score--;
            if (score <= 0) { finito = 0; break;}
            if (mode == 1) {
                displayInt(6,40,4,score);
                display.display();
            }
        }
        if (mode == 0) {
            display.clearDisplay();
            drawView();
            drawPanel();
            display.display();
            mode = 1;
        }
        else if (mode == 2) {
            display.clearDisplay();
            drawMap();
            display.display();
        }
        delay(50);
    }
    if (finito) {
        playMusic(anitra);
        victory(ngame, score);
    }
    else {
        playMusic(aza);
        defeat(ngame);
    }
}

void Maze::drawPanel(void)
{
    int8_t i;
    displayInt(6,40,4,score);
    if (nmaps) {
        display.setCursor(36,40);
        display.write('M');
    }
    if (ncmps) {
        display.setCursor(42,40);
        display.write(027 + startd);
    }
    display.setCursor(48,40);
    for (i=0;i<nkeys;i++) {
        display.write(026);
    }
}
