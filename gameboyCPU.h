typedef unsigned char BYTE;
typedef char S_BYTE;
typedef unsigned short WORD;
typedef short S_WORD;

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define CHANNELS 3

BYTE memGamePak[0x200000];

BYTE screenData[SCREEN_HEIGHT][SCREEN_WIDTH][CHANNELS];