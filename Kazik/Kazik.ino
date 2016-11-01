
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include "Game.h"
#include "maze.h"
#include "zamek.h"
#include "warsztat.h"

#ifndef SWAP_HV
#define HORIZ_PIN A0
#define VERT_PIN A1
#else
#define HORIZ_PIN A1
#define VERT_PIN A0
#endif
#define SELECT_PIN 2
#define LIGHT_PIN 6
#define TONE_PIN 7

#ifdef SWAP_FB
#define KST_FDOWN KSTAT_BDOWN
#define KST_FUP KSTAT_BUP
#define KST_BDOWN KSTAT_FDOWN
#define KST_BUP KSTAT_FUP
#define KST_FWD KSTAT_BACK
#define KST_BACK KSTAT_FWD
#else
#define KST_FDOWN KSTAT_FDOWN
#define KST_FUP KSTAT_FUP
#define KST_BDOWN KSTAT_BDOWN
#define KST_BUP KSTAT_BUP
#define KST_FWD KSTAT_FWD
#define KST_BACK KSTAT_BACK
#endif

Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);


int16_t keyStatus;
int16_t lastXStick, lastYStick;

#define LOW_POT 200
#define HIGH_POT 400

void getStick(void)
{
    static int8_t lxp=1, lyp = 1;
    static uint32_t lastt = 0;
    static int8_t lastb=1;
    int8_t thisb;

    lastXStick = analogRead(HORIZ_PIN);
    lastYStick = analogRead(VERT_PIN);
    thisb = digitalRead(SELECT_PIN);
    keyStatus = 0;

    if (lastXStick < LOW_POT) {
        if (lxp == 1) keyStatus |= KST_LUP;
        if (lxp != -1) keyStatus |= KST_RDOWN;
        keyStatus |= KST_RIGHT;
        lxp = -1;
    }
    else if (lastXStick > HIGH_POT && lastXStick < 1023 - HIGH_POT) {
        if (lxp == -1) keyStatus |= KST_RUP;
        if (lxp ==  1) keyStatus |= KST_LUP;
        lxp = 0;
    }
    else if (lastXStick > 1023 - LOW_POT) {
        if (lxp == -1) keyStatus |= KST_RUP;
        if (lxp != 1) keyStatus |= KST_LDOWN;
        keyStatus |= KST_LEFT;
        lxp = 1;
    }

    if (lastYStick < LOW_POT) {
        if (lyp == 1) keyStatus |= KST_FUP;
        if (lyp != -1) keyStatus |= KST_BDOWN;
        keyStatus |= KST_BACK;
        lyp = -1;
    }
    else if (lastYStick > HIGH_POT && lastYStick < 1023 - HIGH_POT) {
        if (lyp == -1) keyStatus |= KST_BUP;
        if (lyp ==  1) keyStatus |= KST_FUP;
        lyp = 0;
    }
    else if (lastYStick > 1023 - LOW_POT) {
        if (lyp == -1) keyStatus |= KST_BUP;
        if (lyp != 1) keyStatus |= KST_FDOWN;
        keyStatus |= KST_FWD;
        lyp = 1;
    }
    if (thisb != lastb && millis() - lastt > 200) {
		lastb = thisb;
		if (lastb) keyStatus |= KSTAT_SELUP;
		else keyStatus |= KSTAT_SELDOWN;
		lastt = millis();
	}
	if (!lastb) keyStatus |= KSTAT_SELECT;
}

void switchLight(void)
{
    digitalWrite(LIGHT_PIN, !digitalRead(LIGHT_PIN));
}

int32_t getCommand(void)
{
    getStick();
    if (!(keyStatus & KSTAT_SELDOWN)) {
        return 0;
    }
    uint32_t cajt = millis(), cajt2;
    bool t=false;
    for (;;) {
        getStick();
        if (keyStatus & KSTAT_SELUP) break;
        cajt2 = millis() - cajt;
        if (!t && cajt2 >= 1000) {
            switchLight();
            t=true;
        }
        delay(10);
    }
    if (!t && pauseGame()) return -1;
    return cajt2;
}

#ifdef SERIAL_DEBUG
void screenshot(void)
{
	extern uint8_t pcd8544_buffer[];
	int i;
	Serial.println("==SCREENSHOT==");
	for (i=0; i < LCDWIDTH * LCDHEIGHT / 8; i++) {
		Serial.print((int)(pcd8544_buffer[i]>>4),HEX);
		Serial.print((int)(pcd8544_buffer[i] & 15), HEX);
		if ((i & 31) == 31) Serial.println();
	}
	Serial.println("\n==END==");
}
#endif

static PROGMEM const char _pause_str[]="\
GRA WSTRZYMANA\
<=Wyjd\021 Wr\014\011=>";

bool pauseGame(void)
{
    int8_t i;
#ifdef SERIAL_DEBUG
    screenshot();
#endif
    display.setTextWrap(false);
    display.clearDisplay();
    display.setCursor(0,14);
    for (i=0;i<14;i++) {
        display.write(pgm_read_byte(&_pause_str[i]));
    }
    display.setCursor(0,26);
    for (;i<28;i++) {
        display.write(pgm_read_byte(&_pause_str[i]));
    }
    display.display();
    while (keyStatus) {
        delay(100);
        getStick();
    }
    for (i=0;!i;) {
        if (keyStatus & KSTAT_LUP) i = -1;
        else if (keyStatus & KSTAT_RUP) i = 1;
        else {
            if (keyStatus & KSTAT_SELUP) switchLight();
            delay(100);
        }
        getStick();
    }
    return i < 0;
}

static const uint16_t PROGMEM notefreq [61] = {
0, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 350, 370, 392,
416, 441, 467, 494, 524, 555, 588, 623, 660, 700, 741, 785, 832, 882, 934, 989,
1048, 1110, 1176, 1246, 1320, 1400, 1483, 1571, 1665, 1764, 1868, 1979, 2097, 2221, 2353, 2493,
2641, 2801, 2967, 3143, 3330, 3528, 3737, 3959, 4194, 4442, 4706, 4986, 5282
};

volatile int8_t note_pos;
volatile int8_t tick_pos;
volatile int8_t music_play;

/* efekt:
 *
 * BYTE tempo
 * BYTE length
 * uint8_t[] freq;
 *
 * muzyka:
 * BYTE tempo
 * BYTE length
 * (BYTE note_len, BYTE note_num)[] muzyka
 * jeśli note_len & 0x80, to nuta nie jest wyciszana
 * jeśli note_num == 0, jest to pauza
 */

static PROGMEM const uint8_t effect_1[]={
    1, 5,
    440/4, 420/4, 400/4, 380/4, 360/4};

volatile bool effect_playing;
volatile bool music_playing;
volatile bool music_loop;
volatile int8_t audio_mode;
const uint8_t *player_pos, *player_from;

int8_t note_len, tick_len;
int8_t music_len;
bool note_final;

#define AUDIO_MUSIC 1
#define AUDIO_EFFECT 2

void stopPlayer(void)
{
    audio_mode = 0;
}

void playMusic(const uint8_t *music, bool loop)
{
    noInterrupts();
    music_loop = loop;
    audio_mode = AUDIO_MUSIC;
    effect_playing = 0;
    music_playing = 0;
    player_from = music;
    interrupts();
}

void playEffect(const uint8_t *effect, bool force)
{
    if (!force && audio_mode) return;
    noInterrupts();
    audio_mode = AUDIO_EFFECT;
    effect_playing = 0;
    music_playing = 0;
    player_from = effect;
    interrupts();
}

bool isPlaying(void)
{
    return (audio_mode == AUDIO_MUSIC);
}

void musicPlayer(void)
{
    effect_playing = false;
    if (!music_playing) {
        music_playing = 1;
        player_pos = player_from;
        tick_len = pgm_read_byte(player_pos);
        player_pos ++;
        music_len = pgm_read_byte(player_pos);
        player_pos++;
        tick_pos = 0;
        note_pos = 0;
    }
    if (note_pos >= music_len) {
        if (music_loop) {
            note_pos = 0;
        }
        else {
            audio_mode = 0;
            music_playing = false;
            noTone(TONE_PIN);
            return;
        }
    }
    if (!tick_pos) {
        note_len = pgm_read_byte(&player_pos[2 * note_pos]);
        note_final = !(note_len & 0x80);
        note_len = (note_len & 0x7f) * tick_len;
        int8_t note = pgm_read_byte(&player_pos[2 * note_pos + 1]);
        if (note) {
            tone(TONE_PIN, pgm_read_word(&notefreq[note]));
        }
        else {
            noTone(TONE_PIN);
        }
    }
    else if (note_final && tick_pos == note_len - 1) {
        noTone(TONE_PIN);
    }
    tick_pos = (tick_pos + 1) % note_len;
    if (!tick_pos) {
        note_pos ++;
    }
    return;
}

void musica(void)
{
    if (!audio_mode) {
        music_playing = false;
        effect_playing = false;
        noTone(TONE_PIN);
        return;
    }
    if (audio_mode == AUDIO_MUSIC) return musicPlayer();
    music_playing = false;
    if (!effect_playing) {
        player_pos = player_from;
        note_len = pgm_read_byte(player_pos);
        player_pos++;
        music_len = pgm_read_byte(player_pos);
        player_pos++;
        note_pos = 0;
        tick_pos = 0;
        effect_playing = true;
    }
    if (!tick_pos) {
        tone(TONE_PIN,4 * pgm_read_byte(&player_pos[note_pos]));
    }
    tick_pos = (tick_pos + 1) % note_len;
    if (!tick_pos) {
        note_pos++;
        if (note_pos >= music_len) {
            effect_playing = false;
            audio_mode = 0;
        }
    }
}

static PROGMEM const uint8_t logo[] = {
0xf0, 0x00, 0x00, 0x03, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00,
0xf0, 0x00, 0x00, 0x03, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00,
0xf0, 0x00, 0x00, 0x03, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00,
0xf0, 0x00, 0x00, 0x03, 0xc0, 0x07, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00,
0xf7, 0xbf, 0xef, 0xfb, 0xdf, 0xf7, 0xbf, 0xef, 0xfb, 0xfe,
0xf7, 0xbf, 0xef, 0xfb, 0xdf, 0xf7, 0xbf, 0xef, 0xfb, 0xfe,
0xf7, 0xbf, 0xef, 0xfb, 0xdf, 0xf7, 0xbf, 0xef, 0xfb, 0xfe,
0xf7, 0xbf, 0xe0, 0x7b, 0xdf, 0xf7, 0xbf, 0xef, 0xfb, 0xfe,
0xf7, 0xbc, 0x0f, 0xfb, 0xde, 0xf7, 0xbc, 0x0f, 0xfb, 0xde,
0xff, 0xbc, 0x0f, 0xfb, 0xde, 0xf7, 0xbc, 0x0f, 0xfb, 0xde,
0xff, 0xbc, 0x0f, 0x9b, 0xde, 0xf7, 0xbc, 0x0f, 0x9b, 0xde,
0xff, 0xbc, 0x0f, 0x9b, 0xde, 0xf7, 0xbc, 0x0f, 0x9b, 0xde,
0xff, 0xbc, 0x0f, 0xfb, 0xde, 0xf7, 0xbc, 0x0f, 0xfb, 0xde,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xe4, 0x00, 0x88, 0x08, 0x08, 0x00, 0x80, 0x00, 0x00, 0x00,
0x94, 0x00, 0xd8, 0x00, 0x1c, 0x00, 0x80, 0x00, 0x00, 0x00,
0x94, 0x70, 0xab, 0x89, 0xc9, 0x96, 0x93, 0x22, 0x60, 0xe4,
0x94, 0x08, 0x88, 0x4a, 0x0a, 0x58, 0xa4, 0xa2, 0x90, 0x94,
0x94, 0x38, 0x89, 0xc8, 0x8b, 0xd0, 0xc4, 0xaa, 0x90, 0x94,
0x94, 0x48, 0x8a, 0x48, 0x4a, 0x10, 0xa4, 0xb6, 0x96, 0x94,
0xe3, 0x38, 0x89, 0xc8, 0x84, 0xd0, 0x93, 0x22, 0x66, 0xe3,
0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
};


void setup(void)
{
    int i;
#ifdef SERIAL_DEBUG
    Serial.begin(9600);
#endif
    display.begin();
    pinMode(SELECT_PIN, INPUT_PULLUP);
    pinMode(LIGHT_PIN, OUTPUT);
    digitalWrite(LIGHT_PIN, 0);
    pinMode(TONE_PIN, OUTPUT);

    display.setContrast(DISPLAY_CONTRAST);
    display.setRotation(
#ifdef ROTATE_DISPLAY
    2
#else
    0
#endif
    );
    display.clearDisplay();
    display.display();
    delay(20);
    getStick();
    getStick();
    if (keyStatus & KSTAT_SELECT) {
        for (i=0; i< 20; i++) {
            delay(50);
            getStick();
            if (!(keyStatus & KSTAT_SELECT)) break;
        }
        if (i >= 20) askEraseHS();
    }
    display.drawBitmap(2,10,logo,80,28,BLACK);
    display.display(); // show splashscreen
    for (i=0; i<500; i++) {
        getStick();
        if (keyStatus & (KSTAT_LDOWN | KSTAT_RDOWN | KSTAT_FDOWN | KSTAT_BDOWN)) break;
        delay(10);
    }
    display.clearDisplay();   // clears the screen and buffer


    Timer1.initialize(20000);
    Timer1.attachInterrupt(musica);
}

static PROGMEM const char main_menu[]="\
  Labirynt  \
   Zamek    \
  Warsztat  ";
#define COUNT_GAMES 3
#define MENUY (24 - (COUNT_GAMES * 8) / 2)
static bool random_initialize = false;



static void menuShowPos(int8_t pos)
{
    int8_t i;
    for (i = 0; i < COUNT_GAMES; i++) {
        displayText(0,MENUY + 8 * i,2,(i == pos) ? PSTR("=>") : PSTR("  "));
    }
    display.display();
}

void displayText(int8_t x, int8_t y, int8_t len, const char *txt)
{
    display.setCursor(x,y);
    while (len-- > 0) {
        display.write(pgm_read_byte(txt++));
    }
}

int doMenu(void)
{
    int8_t i;
    static int8_t pos=0;
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    for (i=0; i<COUNT_GAMES; i++) {
        displayText(12, MENUY + 8 * i, 12, main_menu + 12 * i);
    }
    menuShowPos(pos);
    for (;;) {
        getStick();
        if (keyStatus & KSTAT_SELDOWN) switchLight();
        if (keyStatus & KSTAT_FDOWN) {
            pos = (pos + COUNT_GAMES - 1) % COUNT_GAMES;
            menuShowPos(pos);
        }
        else if (keyStatus & KSTAT_BDOWN) {
            pos = (pos + 1) % COUNT_GAMES;
            menuShowPos(pos);
        }
        else if (keyStatus & (KSTAT_RDOWN | KSTAT_LDOWN)) {
            return pos;
        }
        delay(50);
    }
}

#define EEPROM_MAGIC 0x3c

struct highscore_res {
    int16_t result[5];
};

void getHighScores(int ngame, struct highscore_res *r)
{
    int addr = 11 * ngame;
    uint8_t magic;
    EEPROM.get(addr, magic);
    if (magic != EEPROM_MAGIC) {
        magic = EEPROM_MAGIC;
        memset(r, 0, 10);
        EEPROM.put(addr, magic);
        EEPROM.put(addr + 1, *r);
    }
    else {
        EEPROM.get(addr + 1, *r);
    }
}

/*
 * zwraca numer nowego rekordu lub -1
 */
int8_t setHighScore(int ngame, int16_t score,  struct highscore_res *r)
{
    int addr = 11 * ngame;
    uint8_t magic;
    int8_t i, j;
    EEPROM.get(addr, magic);
    if (magic != EEPROM_MAGIC) {
        magic = EEPROM_MAGIC;
        memset(r, 0, 10);
        r->result[0] = score;
        EEPROM.put(addr, magic);
        EEPROM.put(addr + 1, *r);
        return 0;
    }
    for (i=0; i<5; i++) r->result[i] = 11 + 22 * i;
    EEPROM.get(addr + 1, *r);
    for (i = 0; i <= 4; i ++) {
        if (r->result[i] <= score) break;
    }
    if (i <= 4) {
        for (j=4; j>i; j--) r->result[j] = r->result[j-1];
        if (r->result[i] != score) {
            r->result[i] = score;
            EEPROM.put(addr + 1, *r);
        }
        return i;
    }
    return -1;
}

static void askEraseHS(void)
{
    display.setTextColor(BLACK, WHITE);
    display.setTextWrap(false);
    displayText(0,10,14,PSTR("Kasuj rekordy?"));
    displayText(8,20,12,PSTR("<=Nie  Tak=>"));
    display.display();
    for (;;) {
        getStick();
        if (keyStatus & KSTAT_LEFT) break;
        if (keyStatus & KSTAT_RIGHT) {
            int8_t i;
            for (i=0; i< COUNT_GAMES;i++) {
                EEPROM.update(11 * i, 0);
            }
            break;
        }
        delay(10);
    }
    display.clearDisplay();
}

void waitForButton(void)
{
    while (keyStatus) {
        delay(50);
        getStick();
    }
    while (!(keyStatus & KSTAT_SELDOWN)) {
        delay(50);
        getStick();
    }
}

void displayInt(int8_t x, int8_t y, int8_t len, int16_t nr)
{
    int8_t i;
    for (i=len - 1; i >= 0; i--) {
        display.setCursor(x + 6 * i, y);
        if (i < len -1 && !nr) {
            display.write(' ');
        }
        else {
            display.write('0' + (nr % 10));
        }
        nr /= 10;
    }
}

void victory(int8_t ng, int16_t score)
{
    struct highscore_res r;
    int8_t hstatus, i;
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    hstatus = setHighScore(ng, score, &r);
    if (hstatus >= 0) {
        displayText(15,0,9,PSTR("GRATULUJ\004"));
        for (i=0; i<5; i++) {
            displayInt(27,8*i+8,5,r.result[i]);
            if (i == hstatus) {
                displayText(15,8*i+8,2,PSTR("=>"));
                displayText(57,8*i+9,2,PSTR("<="));
            }
        }
    }
    else {
        displayText(0,0,8,PSTR("REKORDY:"));
        for (i=0;i<5;i++) {
            displayInt(52, 8 * i, 5, r.result[i]);
        }
        displayText(0,40,6,PSTR("WYNIK:"));
        displayInt(52,40,5,score);
    }
    display.display();
    waitForButton();
}

static PROGMEM const char _lose[]="\
KONIEC GRY\
Przegra\025e\007";

void defeat(int8_t ng)
{
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    displayText(12, 16, 10, _lose);
    displayText(12, 24, 10, _lose + 10);
    display.display();
    waitForButton();
}


void loop(void)
{
    int8_t game;

    stopPlayer();
    game = doMenu();
    if (!random_initialize) {
        random_initialize = true;
        randomSeed(micros());
    }
    if (game == 0) {
        Maze maze;
        maze.init(game);
        maze.mazeLoop();
        return;
    }
    if (game == 2) {
        Warsztat ws;
        ws.init(game);
        ws.warLoop();
        return;
    }
    else {
        Kazik kazik;
        kazik.init(game);
        kazik.kazikLoop();
    }
    delay(10);
}

#ifdef SERIAL_DEBUG
int freeRam (void) {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
#endif

