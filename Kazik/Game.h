#ifndef ETH_GAME_H
#define ETH_GAME_H 1

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


// pin 13 - SCLK
// pin 11 - DIN
// pin 6 - backlight (via resistor)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)

// pin A0 - horizontal
// pin A1 - vertical
// pin 2 - button
// pin 7 - audio


#ifndef ARDUINO_AVR_PRO
#define SERIAL_DEBUG 1
#endif

// swap left-right
#define SWAP_LR 1
// swap up/down
#define SWAP_FB 1
// swap horizontal/vertical
//#define SWAP_HV 1

// rotate display
#define ROTATE_DISPLAY 1

// set display contrast
#define DISPLAY_CONTRAST 60

//do not touch lines below!

extern int16_t keyStatus;
extern int16_t lastXStick, lastYStick;

#define KSTAT_LDOWN 1
#define KSTAT_LUP 2
#define KSTAT_RDOWN 0x4
#define KSTAT_RUP 0x8
#define KSTAT_FDOWN 0x10
#define KSTAT_FUP 0x20
#define KSTAT_BDOWN 0x40
#define KSTAT_BUP 0x80
#define KSTAT_LEFT 0x100
#define KSTAT_RIGHT 0x200
#define KSTAT_FWD 0x400
#define KSTAT_BACK 0x800
#define KSTAT_SELDOWN 0x1000
#define KSTAT_SELUP 0x2000
#define KSTAT_SELECT 0x4000

#ifdef SWAP_LR
#define KST_LDOWN KSTAT_RDOWN
#define KST_LUP KSTAT_RUP
#define KST_RDOWN KSTAT_LDOWN
#define KST_RUP KSTAT_LUP
#define KST_LEFT KSTAT_RIGHT
#define KST_RIGHT KSTAT_LEFT
#else
#define KST_LDOWN KSTAT_LDOWN
#define KST_LUP KSTAT_LUP
#define KST_RDOWN KSTAT_RDOWN
#define KST_RUP KSTAT_RUP
#define KST_LEFT KSTAT_LEFT
#define KST_RIGHT KSTAT_RIGHT
#endif


extern void getStick(void);
/* getCommand zwraca:
 * 0 - normalny przebieg
 * -1 - zakończ grę
 * >0 - ilość milisekund spędzonych w pauzie
 */
extern int32_t getCommand(void);
extern Adafruit_PCD8544 display;

#ifdef SERIAL_DEBUG
extern void screenshot();
extern int freeRam (void);
#endif

extern bool pauseGame(void);
extern void displayInt(int8_t x, int8_t y, int8_t len, int16_t nr);
extern void displayText(int8_t x, int8_t y, int8_t len, const char *txt);
extern void victory(int8_t ng, int16_t score);
extern void defeat(int8_t ng);
extern void playEffect(const uint8_t *effect, bool force = true);
extern void playMusic(const uint8_t *music, bool loop = false);
extern bool isPlaying(void);

#endif

