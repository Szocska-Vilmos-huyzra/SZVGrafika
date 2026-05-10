/*
 * Naprendszer Szimulacio v8
 * SDL2 + SDL2_image + OpenGL 1.x / GLU
 *
 * Forditas (Windows MinGW):
 *   gcc main.c -o solar.exe -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm
 *
 * Forditas (Linux):
 *   gcc main.c -o solar $(sdl2-config --cflags --libs) -lSDL2_image -lGL -lGLU -lm
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI          3.14159265358979323846f
#define DEG2RAD(x)  ((x) * PI / 180.f)
#define QUALITY     48
#define NUM_PLANETS 8
#define NUM_STARS   500

typedef struct { float x,y,z; float r,g,b; float size; } Star;
static Star stars[NUM_STARS];
#define WIN_W       1100
#define WIN_H       650

#define CAM_XROT0  20.f
#define CAM_YROT0   0.f
#define CAM_TX0     0.f
#define CAM_TY0     0.f
#define CAM_TZ0   -35.f

static float animSpeed = 1.0f;
#define ANIM_SPEED_MIN  0.1f
#define ANIM_SPEED_MAX 20.0f
#define ANIM_SPEED_STEP 0.1f

static const unsigned char g_font5x8[][5] = {
/* 32  ' ' */ {0x00,0x00,0x00,0x00,0x00},
/* 33  '!' */ {0x00,0x00,0x5F,0x00,0x00},
/* 34  '"' */ {0x00,0x07,0x00,0x07,0x00},
/* 35  '#' */ {0x14,0x7F,0x14,0x7F,0x14},
/* 36  '$' */ {0x24,0x2A,0x7F,0x2A,0x12},
/* 37  '%' */ {0x23,0x13,0x08,0x64,0x62},
/* 38  '&' */ {0x36,0x49,0x55,0x22,0x50},
/* 39  ''' */ {0x00,0x05,0x03,0x00,0x00},
/* 40  '(' */ {0x00,0x1C,0x22,0x41,0x00},
/* 41  ')' */ {0x00,0x41,0x22,0x1C,0x00},
/* 42  '*' */ {0x08,0x2A,0x1C,0x2A,0x08},
/* 43  '+' */ {0x08,0x08,0x3E,0x08,0x08},
/* 44  ',' */ {0x00,0x50,0x30,0x00,0x00},
/* 45  '-' */ {0x08,0x08,0x08,0x08,0x08},
/* 46  '.' */ {0x00,0x60,0x60,0x00,0x00},
/* 47  '/' */ {0x20,0x10,0x08,0x04,0x02},
/* 48  '0' */ {0x3E,0x51,0x49,0x45,0x3E},
/* 49  '1' */ {0x00,0x42,0x7F,0x40,0x00},
/* 50  '2' */ {0x42,0x61,0x51,0x49,0x46},
/* 51  '3' */ {0x21,0x41,0x45,0x4B,0x31},
/* 52  '4' */ {0x18,0x14,0x12,0x7F,0x10},
/* 53  '5' */ {0x27,0x45,0x45,0x45,0x39},
/* 54  '6' */ {0x3C,0x4A,0x49,0x49,0x30},
/* 55  '7' */ {0x01,0x71,0x09,0x05,0x03},
/* 56  '8' */ {0x36,0x49,0x49,0x49,0x36},
/* 57  '9' */ {0x06,0x49,0x49,0x29,0x1E},
/* 58  ':' */ {0x00,0x36,0x36,0x00,0x00},
/* 59  ';' */ {0x00,0x56,0x36,0x00,0x00},
/* 60  '<' */ {0x08,0x14,0x22,0x41,0x00},
/* 61  '=' */ {0x14,0x14,0x14,0x14,0x14},
/* 62  '>' */ {0x00,0x41,0x22,0x14,0x08},
/* 63  '?' */ {0x02,0x01,0x51,0x09,0x06},
/* 64  '@' */ {0x32,0x49,0x79,0x41,0x3E},
/* 65  'A' */ {0x7E,0x11,0x11,0x11,0x7E},
/* 66  'B' */ {0x7F,0x49,0x49,0x49,0x36},
/* 67  'C' */ {0x3E,0x41,0x41,0x41,0x22},
/* 68  'D' */ {0x7F,0x41,0x41,0x22,0x1C},
/* 69  'E' */ {0x7F,0x49,0x49,0x49,0x41},
/* 70  'F' */ {0x7F,0x09,0x09,0x09,0x01},
/* 71  'G' */ {0x3E,0x41,0x49,0x49,0x7A},
/* 72  'H' */ {0x7F,0x08,0x08,0x08,0x7F},
/* 73  'I' */ {0x00,0x41,0x7F,0x41,0x00},
/* 74  'J' */ {0x20,0x40,0x41,0x3F,0x01},
/* 75  'K' */ {0x7F,0x08,0x14,0x22,0x41},
/* 76  'L' */ {0x7F,0x40,0x40,0x40,0x40},
/* 77  'M' */ {0x7F,0x02,0x0C,0x02,0x7F},
/* 78  'N' */ {0x7F,0x04,0x08,0x10,0x7F},
/* 79  'O' */ {0x3E,0x41,0x41,0x41,0x3E},
/* 80  'P' */ {0x7F,0x09,0x09,0x09,0x06},
/* 81  'Q' */ {0x3E,0x41,0x51,0x21,0x5E},
/* 82  'R' */ {0x7F,0x09,0x19,0x29,0x46},
/* 83  'S' */ {0x46,0x49,0x49,0x49,0x31},
/* 84  'T' */ {0x01,0x01,0x7F,0x01,0x01},
/* 85  'U' */ {0x3F,0x40,0x40,0x40,0x3F},
/* 86  'V' */ {0x1F,0x20,0x40,0x20,0x1F},
/* 87  'W' */ {0x3F,0x40,0x38,0x40,0x3F},
/* 88  'X' */ {0x63,0x14,0x08,0x14,0x63},
/* 89  'Y' */ {0x07,0x08,0x70,0x08,0x07},
/* 90  'Z' */ {0x61,0x51,0x49,0x45,0x43},
/* 91  '[' */ {0x00,0x7F,0x41,0x41,0x00},
/* 92 '\\' */ {0x02,0x04,0x08,0x10,0x20},
/* 93  ']' */ {0x00,0x41,0x41,0x7F,0x00},
/* 94  '^' */ {0x04,0x02,0x01,0x02,0x04},
/* 95  '_' */ {0x40,0x40,0x40,0x40,0x40},
/* 96  '`' */ {0x00,0x01,0x02,0x04,0x00},
/* 97  'a' */ {0x20,0x54,0x54,0x54,0x78},
/* 98  'b' */ {0x7F,0x48,0x44,0x44,0x38},
/* 99  'c' */ {0x38,0x44,0x44,0x44,0x20},
/* 100 'd' */ {0x38,0x44,0x44,0x48,0x7F},
/* 101 'e' */ {0x38,0x54,0x54,0x54,0x18},
/* 102 'f' */ {0x08,0x7E,0x09,0x01,0x02},
/* 103 'g' */ {0x0C,0x52,0x52,0x52,0x3E},
/* 104 'h' */ {0x7F,0x08,0x04,0x04,0x78},
/* 105 'i' */ {0x00,0x44,0x7D,0x40,0x00},
/* 106 'j' */ {0x20,0x40,0x44,0x3D,0x00},
/* 107 'k' */ {0x7F,0x10,0x28,0x44,0x00},
/* 108 'l' */ {0x00,0x41,0x7F,0x40,0x00},
/* 109 'm' */ {0x7C,0x04,0x18,0x04,0x78},
/* 110 'n' */ {0x7C,0x08,0x04,0x04,0x78},
/* 111 'o' */ {0x38,0x44,0x44,0x44,0x38},
/* 112 'p' */ {0x7C,0x14,0x14,0x14,0x08},
/* 113 'q' */ {0x08,0x14,0x14,0x18,0x7C},
/* 114 'r' */ {0x7C,0x08,0x04,0x04,0x08},
/* 115 's' */ {0x48,0x54,0x54,0x54,0x20},
/* 116 't' */ {0x04,0x3F,0x44,0x40,0x20},
/* 117 'u' */ {0x3C,0x40,0x40,0x20,0x7C},
/* 118 'v' */ {0x1C,0x20,0x40,0x20,0x1C},
/* 119 'w' */ {0x3C,0x40,0x30,0x40,0x3C},
/* 120 'x' */ {0x44,0x28,0x10,0x28,0x44},
/* 121 'y' */ {0x0C,0x50,0x50,0x50,0x3C},
/* 122 'z' */ {0x44,0x64,0x54,0x4C,0x44},
/* 123 '{' */ {0x00,0x08,0x36,0x41,0x00},
/* 124 '|' */ {0x00,0x00,0x7F,0x00,0x00},
/* 125 '}' */ {0x00,0x41,0x36,0x08,0x00},
/* 126 '~' */ {0x08,0x08,0x2A,0x1C,0x08},
};


static void fontDrawCharToSurface(SDL_Surface *surf, int cx, int cy,
                                   unsigned char ch,
                                   Uint8 r, Uint8 g, Uint8 b)
{
    int idx = (int)ch - 32;
    int maxidx = (int)(sizeof g_font5x8 / sizeof g_font5x8[0]);
    if (idx < 0 || idx >= maxidx) return;

    Uint32 color = SDL_MapRGBA(surf->format, r, g, b, 255);
    Uint32 *pixels = (Uint32*)surf->pixels;
    int pitch = surf->pitch / 4; /* uint32 lepeskoz */

    for (int col = 0; col < 5; col++) {
        unsigned char coldata = g_font5x8[idx][col];
        for (int row = 0; row < 8; row++) {
            if (coldata & (0x80 >> row)) {
                int px = cx + col;
                int py = cy + row;
                if (px >= 0 && px < surf->w && py >= 0 && py < surf->h)
                    pixels[py * pitch + px] = color;
            }
        }
    }
}

static void fontDrawStringToSurface(SDL_Surface *surf, int x, int y,
                                     const char *str,
                                     Uint8 r, Uint8 g, Uint8 b, int scale)
{
    int cx = x;
    while (*str) {
        if (scale == 1) {
            fontDrawCharToSurface(surf, cx, y, (unsigned char)*str, r, g, b);
        } else {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            SDL_Surface *tmp = SDL_CreateRGBSurface(0,6,8,32,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF);
#else
            SDL_Surface *tmp = SDL_CreateRGBSurface(0,6,8,32,0x000000FF,0x0000FF00,0x00FF0000,0xFF000000);
#endif
            if (tmp) {
                SDL_FillRect(tmp, NULL, SDL_MapRGBA(tmp->format,0,0,0,0));
                fontDrawCharToSurface(tmp, 0, 0, (unsigned char)*str, r, g, b);
                Uint32 *src = (Uint32*)tmp->pixels;
                Uint32 *dst = (Uint32*)surf->pixels;
                int dstpitch = surf->pitch / 4;
                Uint32 blank = SDL_MapRGBA(tmp->format,0,0,0,0);
                for (int row = 0; row < 8; row++) {
                    for (int col = 0; col < 6; col++) {
                        Uint32 px = src[row * (tmp->pitch/4) + col];
                        if (px != blank) {
                            for (int sy = 0; sy < scale; sy++)
                                for (int sx = 0; sx < scale; sx++) {
                                    int fx = cx + col*scale + sx;
                                    int fy = y  + row*scale + sy;
                                    if (fx>=0&&fx<surf->w&&fy>=0&&fy<surf->h)
                                        dst[fy*dstpitch+fx] = px;
                                }
                        }
                    }
                }
                SDL_FreeSurface(tmp);
            }
        }
        cx += 6 * scale;
        str++;
    }
}


static void drawTextGL(float x, float y, const char *str,
                        float cr, float cg, float cb, int scale)
{
    if (!str || !*str) return;

    int len = (int)strlen(str);
    int tw = len * 6 * scale;
    int th = 8 * scale;


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask=0xFF000000, gmask=0x00FF0000, bmask=0x0000FF00, amask=0x000000FF;
#else
    Uint32 rmask=0x000000FF, gmask=0x0000FF00, bmask=0x00FF0000, amask=0xFF000000;
#endif
    SDL_Surface *surf = SDL_CreateRGBSurface(0, tw, th, 32, rmask, gmask, bmask, amask);
    if (!surf) return;
    SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 0, 0, 0, 0));

    Uint8 r8 = (Uint8)(cr * 255);
    Uint8 g8 = (Uint8)(cg * 255);
    Uint8 b8 = (Uint8)(cb * 255);
    fontDrawStringToSurface(surf, 0, 0, str, r8, g8, b8, scale);

 
    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    SDL_FreeSurface(surf);

   
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1); glVertex2f(x,       y);
        glTexCoord2f(1,1); glVertex2f(x+tw,    y);
        glTexCoord2f(1,0); glVertex2f(x+tw,    y+th);
        glTexCoord2f(0,0); glVertex2f(x,       y+th);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glDeleteTextures(1, &texid);
}


static int textW(const char *s, int scale) { return (int)strlen(s) * 6 * scale; }
static int textH(int scale) { return 8 * scale; }

/* ════════════════════════════════════════════════════════════
   ADATSTRUKTURA
   ════════════════════════════════════════════════════════════ */
typedef struct {
    const char *name;
    float radius, orbitRadius, orbitSpeed, rotSpeed;
    float orbitAngle, rotAngle, tilt, orbitTilt;
    const char *texPath;
    GLuint texID;
    float r, g, b;
    int   dragged;
    float worldX, worldY, worldZ;
    float velX, velY, velZ;
    int   returning;
    float returnProgress;
    float returnFromX, returnFromY, returnFromZ;
    int   waitFrames;
    int   absorbed;
    float absorbScale;
    int   bhPulled;
} Planet;

/* ════════════════════════════════════════════════════════════
   HOLD ADATSTRUKTURA
   ════════════════════════════════════════════════════════════ */
typedef struct {
    const char *name;
    float orbitRadius;   
    float orbitSpeed;    
    float orbitAngle;   
    float orbitTilt;   
    float radius;       
} Moon;

#define MAX_MOONS 8

typedef struct {
    int  count;
    Moon moons[MAX_MOONS];
} MoonGroup;


static MoonGroup moonGroups[NUM_PLANETS] = {

    { 0, {{0}} },
    { 0, {{0}} },
    { 1, {
        {"Hold",     1.05f, 0.50f,   0.f,  5.1f, 0.13f},
    }},
    { 2, {
        {"Phobos",   0.72f, 5.50f,   0.f,  1.1f, 0.07f},
        {"Deimos",   1.10f, 2.20f,  60.f,  1.8f, 0.05f},
    }},
    { 4, {
        {"Io",       1.80f, 2.10f,   0.f,  0.0f, 0.13f},
        {"Europa",   2.30f, 1.05f,  90.f,  0.5f, 0.11f},
        {"Ganymede", 3.10f, 0.52f, 180.f,  0.2f, 0.18f},
        {"Callisto", 4.20f, 0.23f, 270.f,  0.2f, 0.15f},
    }},
    { 7, {
        {"Mimas",     1.80f, 3.80f,   0.f,  1.5f, 0.06f},
        {"Enceladus", 2.10f, 2.60f,  51.f,  0.0f, 0.07f},
        {"Tethys",    2.45f, 2.10f, 103.f,  1.1f, 0.09f},
        {"Dione",     2.85f, 1.60f, 155.f,  0.0f, 0.09f},
        {"Rhea",      3.40f, 1.10f, 207.f,  0.3f, 0.11f},
        {"Titan",     5.00f, 0.38f, 259.f,  0.3f, 0.16f},
        {"Iapetus",   9.00f, 0.08f, 311.f, 15.5f, 0.12f},
    }},
    { 5, {
        {"Miranda", 1.45f, 3.40f,   0.f,  4.2f, 0.06f},
        {"Ariel",   1.80f, 2.10f,  72.f,  0.0f, 0.09f},
        {"Umbriel", 2.20f, 1.40f, 144.f,  0.1f, 0.09f},
        {"Titania", 3.10f, 0.73f, 216.f,  0.1f, 0.12f},
        {"Oberon",  4.00f, 0.47f, 288.f,  0.1f, 0.11f},
    }},
    { 2, {
        {"Triton", 1.80f, 0.60f,   0.f, 157.f, 0.14f},
        {"Nereid", 5.00f, 0.05f, 120.f,   7.2f, 0.06f},
    }},
};

static GLuint moonTexID = 0;

/* ════════════════════════════════════════════════════════════
   GLOBÁLIS ÁLLAPOT
   ════════════════════════════════════════════════════════════ */
static float camXRot=CAM_XROT0, camYRot=CAM_YROT0;
static float camTx=CAM_TX0, camTy=CAM_TY0, camTz=CAM_TZ0;
static int   mouseDown=0, lastMX=0, lastMY=0;
static int   paused=0, lightOn=1;
static float lightIntensity=1.6f;
static int   showOrbits=1, showNames=1, showHelp=0, showInfo=0, showCard=1;
static int   running=1;
static float sunRot=0.f;
static GLuint sunTexID=0;
static int   selectedPlanet=-1;
static int   draggedPlanet=-1;
static int   clickMX=0, clickMY=0, didDrag=0;
static int   bhActive=0;
static float bhX=0.f, bhY=0.f, bhZ=0.f, bhPulse=0.f;
static int   introAnim = 1;
static float introTime = 0.f;

static Planet planets[NUM_PLANETS] = {
 {"Merkur",    0.28f,  4.5f, 2.08f, 0.020f,   0.f,0.f,  0.f, 7.f,"img/mercurio.png",0, 0.7f,0.7f,0.6f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Venus",     0.55f,  7.0f, 0.81f, 0.010f,  60.f,0.f,177.f, 3.f,"img/venus.png",   0, 0.9f,0.8f,0.5f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Fold",      0.60f,  9.8f, 0.50f, 0.400f, 120.f,0.f, 23.f, 0.f,"img/terra.png",   0, 0.2f,0.5f,0.9f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Mars",      0.42f, 13.0f, 0.27f, 0.390f, 200.f,0.f, 25.f, 2.f,"img/marte.png",   0, 0.8f,0.3f,0.1f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Jupiter",   1.30f, 19.0f,0.042f, 1.000f, 300.f,0.f,  3.f, 1.f,"img/jupiter.png", 0, 0.8f,0.7f,0.5f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Szaturnusz",1.10f, 26.0f,0.017f, 0.930f,  10.f,0.f, 27.f, 3.f,"img/saturno.png", 0, 0.9f,0.8f,0.6f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Uranusz",   0.80f, 33.0f,0.006f, 0.700f,  80.f,0.f, 98.f, 1.f,"img/urano.png",   0, 0.5f,0.8f,0.9f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
 {"Neptun",    0.75f, 40.0f,0.003f, 0.670f, 160.f,0.f, 28.f, 2.f,"img/netuno.png",  0, 0.2f,0.3f,0.9f, 0,0,0,0,0,0,0, 0,0.f,0,0,0, 0, 0,1.f,0},
};
static float initOrbitAngle[NUM_PLANETS];
static float initMoonAngles[NUM_PLANETS][MAX_MOONS];


/* ════════════════════════════════════════════════════════════
   BOLYGO ADATOK (infokartya)
   ════════════════════════════════════════════════════════════ */
typedef struct {
    const char *name;
    const char *type;
    float  diameter_km;  
    float  mass_1024kg;   
    int    orbit_days;   
    float  dist_mkm;     
    int    moons;       
    float  temp_c;      
    const char *extra;   
} PlanetInfo;


static const PlanetInfo planetInfos[9] = {
    {"Nap",        "Csillag (G-tipus)", 1392700.f, 1989000.f,     0,    0.f,   0,  5505.f,
     "Tomege a Naprendszer 99.86%-a"},

    {"Merkur",     "Ko bolygo",            4879.f,    0.330f,    88,   57.9f,   0,   167.f,
     "Nincs atmosfera, hatalmas homerseklet-ingadozas"},

    {"Venus",      "Ko bolygo",           12104.f,    4.867f,   225,  108.2f,   0,   464.f,
     "A legforrobb bolygo az eros uveghazhatas miatt"},

    {"Fold",       "Ko bolygo",           12756.f,    5.972f,   365,  149.6f,   1,    15.f,
     "Az egyetlen ismert eletet hordozo egitest"},

    {"Mars",       "Ko bolygo",            6792.f,    0.642f,   687,  227.9f,   2,   -65.f,
     "Az Olympus Mons a Naprendszer legnagyobb vulkanja"},

    {"Jupiter",    "Gazorias",           142984.f, 1898.000f,  4333,  778.6f,  95,  -110.f,
     "A Nagy Voros Folt egy oriasi viharrendszer"},

    {"Szaturnusz", "Gazorias",           120536.f,  568.000f, 10759, 1433.5f, 146,  -140.f,
     "Gyurui foleg jeg- es koszemcsekbol allnak"},

    {"Uranusz",    "Jegorias",            51118.f,   86.810f, 30687, 2872.5f,  28,  -195.f,
     "Oldalra dolve kering a Nap korul"},

    {"Neptun",     "Jegorias",            49528.f,  102.000f, 60190, 4495.1f,  16,  -200.f,
     "A legerosebb szelek ezen a bolygon vannak"}
};

/* ════════════════════════════════════════════════════════════
   TEXTÚRA
   ════════════════════════════════════════════════════════════ */
static GLuint loadTexture(const char *path)
{
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) { fprintf(stderr,"Tex hiba (%s): %s\n",path,IMG_GetError()); return 0; }
    SDL_Surface *conv = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);
    if (!conv) return 0;
    GLuint id; glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,conv->w,conv->h,0,GL_RGBA,GL_UNSIGNED_BYTE,conv->pixels);
    SDL_FreeSurface(conv);
    return id;
}


static void planetWorldPos(Planet *p, float *ox, float *oy, float *oz)
{
    float ang  = DEG2RAD(p->orbitAngle);
    float tilt = DEG2RAD(p->orbitTilt);

    float x = p->orbitRadius * cosf(ang);
    float z = p->orbitRadius * sinf(ang);

    float ry = -z * sinf(tilt);
    float rz =  z * cosf(tilt);

    *ox = x;
    *oy = ry;
    *oz = rz;
}

/* ════════════════════════════════════════════════════════════
   2D OVERLAY
   ════════════════════════════════════════════════════════════ */
static void beginOrtho(void)
{
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, WIN_W, WIN_H, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

static void endOrtho(void)
{
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    if (lightOn) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
}

static void fillRect(float x,float y,float w,float h,
                     float r,float g,float b,float a)
{
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r,g,b,a);
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd(); glDisable(GL_BLEND);
}

static void strokeRect(float x,float y,float w,float h,
                        float r,float g,float b,float lw)
{
    glLineWidth(lw); glColor3f(r,g,b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd(); glLineWidth(1.f);
}

/* ════════════════════════════════════════════════════════════
   3D RAJZOLÁS
   ════════════════════════════════════════════════════════════ */
static void drawTexturedSphere(float radius, GLuint texID, int sl)
{
    GLUquadric *q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    if (texID) { gluQuadricTexture(q,GL_TRUE); glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texID); }
    else glDisable(GL_TEXTURE_2D);
    gluSphere(q, radius, sl, sl/2);
    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}

static void drawOrbit(float r, float tilt)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.f,1.f,1.f,0.15f);

    glBegin(GL_LINE_LOOP);

    for(int i=0;i<360;i++){
        float a = DEG2RAD((float)i);

        float x = r * cosf(a);
        float z = r * sinf(a);

        float tr = DEG2RAD(tilt);

        float y2 = -z * sinf(tr);
        float z2 =  z * cosf(tr);

        glVertex3f(x, y2, z2);
    }

    glEnd();

    glDisable(GL_BLEND);
}

static void drawRings(float inner, float outer)
{
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i=0;i<=360;i++){
        float a=DEG2RAD((float)i);
        float al=(i%2==0)?0.55f:0.35f;
        glColor4f(0.90f,0.82f,0.60f,al);
        glVertex3f(inner*cosf(a), 0.f, inner*sinf(a));
        glColor4f(0.80f,0.70f,0.45f,al*0.6f);
        glVertex3f(outer*cosf(a), 0.f, outer*sinf(a));
    }
    glEnd(); glDisable(GL_BLEND);
}

static void configLight(void)
{
    if (!lightOn){glDisable(GL_LIGHTING);glDisable(GL_LIGHT0);return;}
    float iv=lightIntensity;
    GLfloat amb[]={0.02f,0.02f,0.02f,1.f};
    GLfloat dif[]={iv,iv,iv,1.f};
    GLfloat spe[]={iv*.5f,iv*.5f,iv*.5f,1.f};
    GLfloat pos[]={0.f,0.f,0.f,1.f};
    glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
    glLightfv(GL_LIGHT0,GL_SPECULAR,spe);
    glLightfv(GL_LIGHT0,GL_POSITION,pos);
    glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.f);
    glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.005f);
    glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.0002f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
}

static void drawSun(void)
{
    glDisable(GL_LIGHTING);
    glPushMatrix(); glRotatef(sunRot,0.f,1.f,0.f);
    glColor3f(1.f,1.f,1.f);
    drawTexturedSphere(2.0f,sunTexID,QUALITY);
    glPopMatrix();
    if(lightOn){glEnable(GL_LIGHTING);glEnable(GL_LIGHT0);}
}

static void drawPlanet(Planet *p)
{
    if (p->absorbed && p->absorbScale <= 0.01f) return;

    glPushMatrix();

    if (p->returning) {
        float t = p->returnProgress;
        float ft = 1.f - (1.f-t)*(1.f-t)*(1.f-t);
        float tx,ty,tz;
        planetWorldPos(p,&tx,&ty,&tz);
        glTranslatef(
            p->returnFromX + (tx - p->returnFromX)*ft,
            p->returnFromY + (ty - p->returnFromY)*ft,
            p->returnFromZ + (tz - p->returnFromZ)*ft
        );
    } else if (p->absorbed) {
        glTranslatef(p->worldX, p->worldY, p->worldZ);
    } else if (p->dragged || p->waitFrames > 0) {
        glTranslatef(p->worldX, p->worldY, p->worldZ);
    } else {
       float px, py, pz;
        planetWorldPos(p, &px, &py, &pz);
        glTranslatef(px, py, pz);
    }

    if (p->absorbed)
        glScalef(p->absorbScale, p->absorbScale, p->absorbScale);

    glRotatef(p->tilt,     0.f, 0.f, 1.f);
    glRotatef(p->rotAngle, 0.f, 1.f, 0.f);

    if (lightOn) {
        GLfloat ma[]={p->r*.05f,p->g*.05f,p->b*.05f,1.f};
        GLfloat md[]={p->r,p->g,p->b,1.f};
        GLfloat ms[]={.15f,.15f,.15f,1.f};
        GLfloat msh[]={8.f};
        GLfloat me[]={0.f,0.f,0.f,1.f};
        glMaterialfv(GL_FRONT,GL_AMBIENT,ma);
        glMaterialfv(GL_FRONT,GL_DIFFUSE,md);
        glMaterialfv(GL_FRONT,GL_SPECULAR,ms);
        glMaterialfv(GL_FRONT,GL_SHININESS,msh);
        glMaterialfv(GL_FRONT,GL_EMISSION,me);
        glColor3f(1.f,1.f,1.f);
    } else {
        glColor3f(p->r, p->g, p->b);
    }

    drawTexturedSphere(p->radius, p->texID, QUALITY);

    if (strcmp(p->name,"Szaturnusz") == 0)
        drawRings(p->radius*1.35f, p->radius*2.4f);

    {
        int pi = (int)(p - planets);
        MoonGroup *mg = &moonGroups[pi];
        for (int m = 0; m < mg->count; m++) {
            Moon *mo = &mg->moons[m];
            glPushMatrix();
            glRotatef(mo->orbitTilt, 1.f, 0.f, 0.f);
           float ma = DEG2RAD(mo->orbitAngle);

            float mx = mo->orbitRadius * cosf(ma);
            float mz = mo->orbitRadius * sinf(ma);

            float mt = DEG2RAD(mo->orbitTilt);

            float my2 = -mz * sinf(mt);
            float mz2 =  mz * cosf(mt);

glTranslatef(mx, my2, mz2);
            if (lightOn) {
                GLfloat ma[]={0.05f,0.05f,0.05f,1.f};
                GLfloat md[]={0.75f,0.75f,0.75f,1.f};
                GLfloat ms[]={0.05f,0.05f,0.05f,1.f};
                GLfloat msh[]={4.f};
                GLfloat me[]={0.f,0.f,0.f,1.f};
                glMaterialfv(GL_FRONT,GL_AMBIENT,ma);
                glMaterialfv(GL_FRONT,GL_DIFFUSE,md);
                glMaterialfv(GL_FRONT,GL_SPECULAR,ms);
                glMaterialfv(GL_FRONT,GL_SHININESS,msh);
                glMaterialfv(GL_FRONT,GL_EMISSION,me);
                glColor3f(1.f,1.f,1.f);
            } else {
                glColor3f(0.75f,0.75f,0.75f);
            }
            drawTexturedSphere(mo->radius, moonTexID, 16);
            glPopMatrix();
        }
    }

    glPopMatrix();
}

static void drawBlackHole(void)
{
    if (!bhActive) return;
    glPushMatrix(); glTranslatef(bhX,bhY,bhZ);
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glColor3f(0.f,0.f,0.f);
    GLUquadric *q=gluNewQuadric(); gluQuadricNormals(q,GLU_SMOOTH);
    gluSphere(q,0.8f,32,16); gluDeleteQuadric(q);
    float pulse=0.5f+0.5f*sinf(bhPulse);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for(int i=0;i<5;i++){
        float ri=0.9f+i*0.35f+pulse*0.1f;
        float al=(0.7f-i*0.13f)*(1.f-(float)i/7.f);
        glColor4f(0.6f+pulse*0.3f,0.1f,0.9f,al);
        glBegin(GL_LINE_LOOP);
        for(int j=0;j<64;j++){float a=DEG2RAD((float)j*360.f/64.f);glVertex3f(ri*cosf(a),0.f,ri*sinf(a));}
        glEnd();
    }
    glPushMatrix(); glRotatef(bhPulse*10.f,0.f,1.f,0.f);
    glBegin(GL_TRIANGLE_STRIP);
    for(int i=0;i<=72;i++){
        float a=DEG2RAD((float)i*360.f/72.f),ac=(i%2==0)?.55f:.25f;
        glColor4f(.8f,.3f,1.f,ac); glVertex3f(1.1f*cosf(a),0.f,1.1f*sinf(a));
        glColor4f(.4f,0.f,.6f,ac*.4f); glVertex3f(2.2f*cosf(a),0.f,2.2f*sinf(a));
    }
    glEnd(); glPopMatrix();
    glDisable(GL_BLEND);
    if(lightOn){glEnable(GL_LIGHTING);glEnable(GL_LIGHT0);}
    glPopMatrix();
}

/* ════════════════════════════════════════════════════════════
   RAY PICKING
   ════════════════════════════════════════════════════════════ */
static int rayPick(int mx,int my,double *nx,double *ny,double *nz,
                   double *dx,double *dy,double *dz)
{
    GLdouble mv[16],proj[16]; GLint vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX,mv);
    glGetDoublev(GL_PROJECTION_MATRIX,proj);
    glGetIntegerv(GL_VIEWPORT,vp);
    double wy=(double)(WIN_H-my);
    GLdouble fx,fy,fz;
    gluUnProject((double)mx,wy,0.0,mv,proj,vp,nx,ny,nz);
    gluUnProject((double)mx,wy,1.0,mv,proj,vp,&fx,&fy,&fz);
    *dx=fx-*nx; *dy=fy-*ny; *dz=fz-*nz;
    double len=sqrt((*dx)*(*dx)+(*dy)*(*dy)+(*dz)*(*dz));
    if(len<1e-9) return 0;
    *dx/=len; *dy/=len; *dz/=len;
    return 1;
}

static double sphereRayT(double ox,double oy,double oz,
                          double dx,double dy,double dz,
                          double cx,double cy,double cz,double r)
{
    double ex=ox-cx,ey=oy-cy,ez=oz-cz;
    double B=2.*(ex*dx+ey*dy+ez*dz);
    double C=ex*ex+ey*ey+ez*ez-r*r;
    double D=B*B-4.*C;
    if(D<0.) return -1.;
    double t=(-B-sqrt(D))/2.;
    if(t<0.01) t=(-B+sqrt(D))/2.;
    return t;
}

static int pickPlanet(int mx,int my)
{
    double nx,ny,nz,dx,dy,dz;
    if(!rayPick(mx,my,&nx,&ny,&nz,&dx,&dy,&dz)) return -1;
    int best=-1; double bestT=1e18;
    for(int i=0;i<NUM_PLANETS;i++){
        if(planets[i].absorbed && planets[i].absorbScale<0.1f) continue;
        double cx,cy,cz;
        if(planets[i].dragged || planets[i].waitFrames>0 || planets[i].returning){
            cx=planets[i].worldX; cy=planets[i].worldY; cz=planets[i].worldZ;
        } else {
            float ox,oy,oz;
            planetWorldPos(&planets[i],&ox,&oy,&oz);
            cx=ox; cy=oy; cz=oz;
        }
        double t=sphereRayT(nx,ny,nz,dx,dy,dz,cx,cy,cz,(double)(planets[i].radius*1.4));
        if(t>0.01&&t<bestT){bestT=t;best=i;}
    }
    return best;
}

static int mouseToWorld(int mx,int my,float *wx,float *wy2,float *wz)
{
    double nx,ny,nz,dx,dy,dz;
    if(!rayPick(mx,my,&nx,&ny,&nz,&dx,&dy,&dz)) return 0;
    if(fabs(dy)<1e-6){*wx=(float)nx;*wy2=0.f;*wz=(float)nz;return 1;}
    double t=-ny/dy;
    *wx=(float)(nx+t*dx); *wy2=0.f; *wz=(float)(nz+t*dz);
    return 1;
}

/* ════════════════════════════════════════════════════════════
   HUD
   ════════════════════════════════════════════════════════════ */
static void drawHUD(void)
{
    char buf[256];
    int S  = 2;  
    int SB = 2;
    int LH = textH(S) + 5;

    beginOrtho();

    int topH = 5 + textH(SB) + 6;
    fillRect(0, 0, WIN_W, topH, 0.f,0.f,0.f, 0.68f);
    strokeRect(0, topH-1, WIN_W, 1, 0.6f,0.5f,0.1f, 1.f);

    drawTextGL(10, 5, "NAPRENDSZER SZIMULACIO", 1.f,0.78f,0.2f, SB);
    drawTextGL(WIN_W - textW("F1=Sugo  F2=Info", SB) - 10, 5,
               "F1=Sugo  F2=Info", 0.39f,0.9f,0.39f, SB);

    if (selectedPlanet >= 0) {
        snprintf(buf,sizeof buf,">> %s <<",planets[selectedPlanet].name);
        int tw = textW(buf, SB);
        int bx = (WIN_W - tw)/2 - 8;
        fillRect(bx, 3, tw+16, textH(SB)+4, 0.f,0.f,0.f,0.85f);
        strokeRect(bx, 3, tw+16, textH(SB)+4,
                   planets[selectedPlanet].r,
                   planets[selectedPlanet].g,
                   planets[selectedPlanet].b, 2.f);
        drawTextGL(bx+8, 5, buf,
                   planets[selectedPlanet].r,
                   planets[selectedPlanet].g,
                   planets[selectedPlanet].b, SB);
    }

    if (showNames) {
        int pw = 220;
        int ph = textH(S)+8 + NUM_PLANETS*(textH(S)+4) + 6;
        int startY = topH + 6;
        fillRect(WIN_W-pw-4, startY, pw+4, ph, 0.f,0.f,0.f,0.6f);
        drawTextGL(WIN_W-pw, startY+4, "Bolygok:", 0.7f,0.7f,1.f, S);
        for (int i=0;i<NUM_PLANETS;i++){
            float nr=planets[i].r, ng=planets[i].g, nb=planets[i].b;
            if(i==selectedPlanet){nr=1.f;ng=1.f;nb=0.3f;}
            if(planets[i].absorbed)
                snprintf(buf,sizeof buf,"%s [ELNYELT]",planets[i].name);
            else if(planets[i].returning)
                snprintf(buf,sizeof buf,"%s [VISSZA]",planets[i].name);
            else if(planets[i].waitFrames > 0)
                snprintf(buf,sizeof buf,"%s [VAR]",planets[i].name);
            else if(planets[i].dragged)
                snprintf(buf,sizeof buf,"%s [MOZOG]",planets[i].name);
            else
                snprintf(buf,sizeof buf,"%s",planets[i].name);
            drawTextGL(WIN_W-pw,
                       startY+4+textH(S)+6+i*(textH(S)+4),
                       buf, nr,ng,nb, S);
        }
    }

    if (showCard && selectedPlanet >= -1) {
        const PlanetInfo *pi = &planetInfos[selectedPlanet + 1]; 
        float pr = 1.f, pg = 1.f, pb = 1.f;
        if (selectedPlanet >= 0) {
            pr = planets[selectedPlanet].r;
            pg = planets[selectedPlanet].g;
            pb = planets[selectedPlanet].b;
        } else {
            pr = 1.f; pg = 0.78f; pb = 0.2f; 
        }

        char line_type[64], line_diam[64], line_mass[64];
        char line_orbit[64], line_dist[64], line_moons[64];
        char line_temp[64];

        snprintf(line_type,  sizeof line_type,  "Tipus  : %s", pi->type);
        if (pi->diameter_km >= 100000.f)
            snprintf(line_diam, sizeof line_diam, "Atmero : %.0f km", pi->diameter_km);
        else
            snprintf(line_diam, sizeof line_diam, "Atmero : %.0f km", pi->diameter_km);
        if (pi->mass_1024kg >= 1000.f)
            snprintf(line_mass, sizeof line_mass, "Tomeg  : %.0f * 10^24 kg", pi->mass_1024kg);
        else
            snprintf(line_mass, sizeof line_mass, "Tomeg  : %.3f * 10^24 kg", pi->mass_1024kg);
        if (pi->orbit_days == 0)
            snprintf(line_orbit, sizeof line_orbit, "Keringo: -");
        else if (pi->orbit_days < 365)
            snprintf(line_orbit, sizeof line_orbit, "Keringo: %d nap", pi->orbit_days);
        else
            snprintf(line_orbit, sizeof line_orbit, "Keringo: %.1f ev", pi->orbit_days / 365.25f);
        if (pi->dist_mkm == 0.f)
            snprintf(line_dist, sizeof line_dist, "Tavolsag: -");
        else
            snprintf(line_dist, sizeof line_dist, "Tavolsag: %.1f M km", pi->dist_mkm);
        snprintf(line_moons, sizeof line_moons, "Holdak : %d", pi->moons);
        snprintf(line_temp, sizeof line_temp, "Homerseklet: %.0f C", pi->temp_c);
        const char *cardlines[] = {
            line_type, line_diam, line_mass,
            line_orbit, line_dist, line_moons, line_temp,
            pi->extra
        };
        int nc = 8;
        int lsc = 1, clh = textH(lsc) + 4;

        int cpw = 0;
        for (int i = 0; i < nc; i++) {
            int w = textW(cardlines[i], lsc);
            if (w > cpw) cpw = w;
        }
        cpw += 28;

        int titleH = textH(SB) + 10;
        int cph = titleH + nc * clh + 10;
        int cpx = 8;
        int cpy = WIN_H - cph - 8;

        fillRect(cpx-3, cpy-3, cpw+6, cph+6, pr*0.6f, pg*0.6f, pb*0.6f, 0.92f);
        fillRect(cpx, cpy, cpw, cph, 0.f, 0.f, 0.08f, 0.95f);

        drawTextGL(cpx + (cpw - textW(pi->name, SB))/2, cpy + 6,
                   pi->name, pr, pg, pb, SB);

        int cy2 = cpy + titleH;
        for (int i = 0; i < nc; i++) {
            float lr = 0.82f, lg = 0.82f, lb = 0.82f;
            if (i == nc-1) { lr=pr*0.9f+0.1f; lg=pg*0.7f+0.3f; lb=pb*0.5f+0.3f; } /* extra sor kiemelt */
            drawTextGL(cpx + 10, cy2, cardlines[i], lr, lg, lb, lsc);
            cy2 += clh;
        }

        const char *nav = "< > valtas  |  le: bezar";
        drawTextGL(cpx + (cpw - textW(nav, lsc))/2, cpy + cph - clh + 2,
                   nav, 0.4f, 0.4f, 0.4f, lsc);
    }

    if (bhActive) {
        float pulse=0.5f+0.5f*sinf(bhPulse);
        const char *bt="FEKETE LYUK AKTIV";
        int tw=textW(bt,S);
        int bx=(WIN_W-tw)/2-8, by=WIN_H-textH(S)-14;
        fillRect(bx,by,tw+16,textH(S)+6, 0.15f*pulse,0.f,0.25f*pulse,0.9f);
        strokeRect(bx,by,tw+16,textH(S)+6, 0.7f,0.2f,1.f,2.f);
        drawTextGL(bx+8, by+3, bt, 0.9f,0.4f,1.f, S);
    }

    if (showInfo) {
        int pw=400, lh2=LH;
        int ph=textH(SB)+16 + 6*lh2 + 12;
        int px=(WIN_W-pw)/2, py=(WIN_H-ph)/2;
        fillRect(px-3,py-3,pw+6,ph+6, 0.6f,0.5f,0.1f,0.92f);
        fillRect(px,py,pw,ph, 0.f,0.f,0.08f,0.97f);
        int iy=py+8;
        const char *ttl="=== INFO PANEL ===";
        drawTextGL(px+(pw-textW(ttl,SB))/2, iy, ttl, 1.f,0.85f,0.2f, SB);
        iy+=textH(SB)+12;

        snprintf(buf,sizeof buf,"Animacio: %s", paused?"SZUNET":"FUTO");
        drawTextGL(px+14, iy, buf, 0.8f,0.8f,0.8f, S); iy+=lh2;

        snprintf(buf,sizeof buf,"Feny: %s", lightOn?"BE":"KI");
        drawTextGL(px+14, iy, buf, 0.9f,0.85f,0.5f, S); iy+=lh2;

        snprintf(buf,sizeof buf,"Sebesseg: %.2fx  (J/K)", animSpeed);
        drawTextGL(px+14, iy, buf, 0.5f,1.f,0.5f, S); iy+=lh2;

        snprintf(buf,sizeof buf,"Palya: %s   Nevek: %s",
                 showOrbits?"BE":"KI", showNames?"BE":"KI");
        drawTextGL(px+14, iy, buf, 0.8f,0.8f,0.8f, S); iy+=lh2;

        snprintf(buf,sizeof buf,"Fekete lyuk: %s", bhActive?"AKTIV":"KI");
        drawTextGL(px+14, iy, buf, 0.8f,0.5f,1.f, S); iy+=lh2;

        const char *cls="[ F2 = bezaras ]";
        drawTextGL(px+(pw-textW(cls,S))/2, iy, cls, 0.5f,0.5f,0.5f, S);
    }

    if (showHelp) {
        const char *keys[]={
            "Eger bal + huzas",
            "Eger gorgo",
            "W/S  A/D  Q/E",
            "Bal katt bolygon",
            "Bal nyomva + huzas",
            "Jobb klikk tartva",
            "P",
            "L",
            "J / K",
            "O",
            "N",
            "Bal / Jobb nyil",
            "Fel / Le nyil",
            "R",
            "1 / 2",
            "F2",
            "ESC",
        };
        const char *vals[]={
            "kamera forgatas",
            "zoom",
            "kamera mozgatas",
            "kivalaszt",
            "bolygot mozgat",
            "fekete lyuk aktiv",
            "szunet / folytat",
            "feny be/ki",
            "sebesseg -/+ ",
            "palyak be/ki",
            "nevek be/ki",
            "bolygo valtas (Nap is)",
            "infokartya elrejt / elohoz",
            "teljes reset",
            "felul / oldal nezet",
            "info panel",
            "kilepes",
        };
    
        int nl=(int)(sizeof keys/sizeof*keys);
        int lsc=1, lh2=textH(lsc)+5;
        int col1w = 21*6*lsc;  
        int sep   = 14;      
        int col2w = 25*6*lsc;   
        int pw = 14 + col1w + sep + col2w + 14;
        int ph = textH(SB)+16 + nl*lh2 + 8;
        if(ph > WIN_H-20) ph=WIN_H-20;
        int px=(WIN_W-pw)/2, py=(WIN_H-ph)/2;
        fillRect(px-3,py-3,pw+6,ph+6, 0.8f,0.6f,0.1f,0.92f);
        fillRect(px,py,pw,ph, 0.f,0.f,0.08f,0.97f);
        int hy=py+8;
        const char *ttl="=== VEZERLES  (F1=bezar) ===";
        drawTextGL(px+(pw-textW(ttl,SB))/2, hy, ttl, 1.f,0.85f,0.2f, SB);
        hy+=textH(SB)+12;
        for(int i=0;i<nl && hy+lh2 < py+ph-4; i++){
            float kr=0.85f,kg=0.85f,kb=0.85f;
            float vr=0.85f,vg=0.85f,vb=0.85f;
            if(i==5)  {kr=0.9f;kg=0.4f;kb=1.f;  vr=0.9f;vg=0.4f;vb=1.f;}
            if(i==7)  {kr=1.f;kg=0.9f;kb=0.3f;  vr=1.f;vg=0.9f;vb=0.3f;}
            if(i==8)  {kr=0.4f;kg=1.f;kb=0.4f;  vr=0.4f;vg=1.f;vb=0.4f;}
            if(i==11||i==14){kr=0.4f;kg=0.9f;kb=0.4f; vr=0.4f;vg=0.9f;vb=0.4f;}
            drawTextGL(px+14,          hy, keys[i], kr,kg,kb, lsc);
            drawTextGL(px+14+col1w,    hy, ":",     0.5f,0.5f,0.5f, lsc);
            drawTextGL(px+14+col1w+sep,hy, vals[i], vr,vg,vb, lsc);
            hy+=lh2;
        }
    }

    endOrtho();
}

/* ════════════════════════════════════════════════════════════
   RESET
   ════════════════════════════════════════════════════════════ */
static void resetSim(void)
{
    camXRot=CAM_XROT0; camYRot=CAM_YROT0;
    camTx=CAM_TX0; camTy=CAM_TY0; camTz=CAM_TZ0;
    paused=0; lightOn=1; lightIntensity=1.6f; animSpeed=1.0f;
    showOrbits=1; showNames=1; showHelp=0; showInfo=0;
    sunRot=0.f; selectedPlanet=-1; draggedPlanet=-1;
    bhActive=0; bhPulse=0.f;
    for(int i=0;i<NUM_PLANETS;i++){
        planets[i].orbitAngle     = initOrbitAngle[i];
        planets[i].rotAngle       = 0.f;
        planets[i].dragged        = 0;
        planets[i].returning      = 0;
        planets[i].returnProgress = 0.f;
        planets[i].waitFrames     = 0;
        planets[i].velX = planets[i].velY = planets[i].velZ = 0.f;
        planets[i].absorbed       = 0;
        planets[i].absorbScale    = 1.f;
        planets[i].bhPulled       = 0;
        for(int m=0;m<moonGroups[i].count;m++)
            moonGroups[i].moons[m].orbitAngle = initMoonAngles[i][m];
    }
}

/* ════════════════════════════════════════════════════════════
   CSILLAGMEZO
   ════════════════════════════════════════════════════════════ */
static void initStars(void)
{

    srand(42);
    for (int i = 0; i < NUM_STARS; i++) {
        float theta = ((float)rand()/RAND_MAX) * 2.f * PI;
        float phi   = acosf(1.f - 2.f * ((float)rand()/RAND_MAX));
        float r     = 300.f + ((float)rand()/RAND_MAX) * 200.f;
        stars[i].x  = r * sinf(phi) * cosf(theta);
        stars[i].y  = r * sinf(phi) * sinf(theta);
        stars[i].z  = r * cosf(phi);
        float warm  = ((float)rand()/RAND_MAX);
        if (warm > 0.6f) {
            stars[i].r = 1.0f;
            stars[i].g = 0.95f + ((float)rand()/RAND_MAX) * 0.05f;
            stars[i].b = 0.6f  + ((float)rand()/RAND_MAX) * 0.2f;
        } else {
            float t = ((float)rand()/RAND_MAX);
            stars[i].r = 0.85f + t * 0.15f;
            stars[i].g = 0.85f + t * 0.15f;
            stars[i].b = 0.90f + ((float)rand()/RAND_MAX) * 0.10f;
        }
        float rnd = (float)rand()/RAND_MAX;
        stars[i].size = (rnd < 0.75f) ? 1.0f : (rnd < 0.93f) ? 1.5f : 2.0f;
    }
}

static void drawStars(void)
{
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);


    glPointSize(1.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        if (stars[i].size > 1.0f) continue;
        float alpha = 0.5f + ((float)(i % 17) / 17.f) * 0.5f;
        glColor4f(stars[i].r, stars[i].g, stars[i].b, alpha);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();

    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        if (stars[i].size != 1.5f) continue;
        glColor4f(stars[i].r, stars[i].g, stars[i].b, 0.85f);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();


    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        if (stars[i].size < 2.0f) continue;
        glColor4f(stars[i].r, stars[i].g, stars[i].b, 1.0f);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();

    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    if (lightOn) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
}

/* ════════════════════════════════════════════════════════════
   RENDER
   ════════════════════════════════════════════════════════════ */
static void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(camTx, camTy, camTz);

glRotatef(camXRot, 1.f,0.f,0.f);
glRotatef(camYRot, 0.f,1.f,0.f);


if(selectedPlanet >= 0){

    Planet *p = &planets[selectedPlanet];

    float px, py, pz;

    if(p->dragged || p->waitFrames > 0 || p->returning){
        px = p->worldX;
        py = p->worldY;
        pz = p->worldZ;
    } else {
        planetWorldPos(p, &px, &py, &pz);
    }

    glTranslatef(-px, -py, -pz);
}
    drawStars();
    configLight();
    if(showOrbits)
        for(int i=0;i<NUM_PLANETS;i++)
            drawOrbit(planets[i].orbitRadius, planets[i].orbitTilt);
    drawSun();
    for(int i=0;i<NUM_PLANETS;i++) drawPlanet(&planets[i]);
    drawBlackHole();
    drawHUD();
}

/* ════════════════════════════════════════════════════════════
   UPDATE
   ════════════════════════════════════════════════════════════ */
#define WAIT_FRAMES 150   

static void update(void)
{
if(introAnim){

    introTime += 0.0035f;

    float t = introTime;
    if(t > 1.f) t = 1.f;

    float s = t * t * (3.f - 2.f * t); 
    camTz = -520.f + (-45.f + 520.f) * s;
    camXRot = 5.f + (18.f - 5.f) * s;
    camYRot = 18.f * (1.f - s);
    camTx = sinf(t * PI) * 1.2f;
    camTy = -15.f * (1.f - s);
    if(introTime >= 1.f){

        introAnim = 0;

        camTz   = -45.f;
        camXRot = 18.f;
        camYRot = 0.f;
        camTx   = 0.f;
        camTy   = 0.f;
    }
}
    bhPulse += 0.05f;
    if (!paused) {
        float sp = animSpeed;
        sunRot += 0.08f * sp;

        for(int i=0;i<NUM_PLANETS;i++){

            if(planets[i].absorbed){
                if(planets[i].absorbScale > 0.f){
                    planets[i].absorbScale -= 0.025f;
                    if(planets[i].absorbScale < 0.f) planets[i].absorbScale = 0.f;
                }
                continue;
            }

            if(planets[i].returning && !planets[i].dragged){
                planets[i].returnProgress += 0.007f;
                if(planets[i].returnProgress >= 1.f){
                    planets[i].returnProgress = 1.f;
                    planets[i].returning = 0;
                    planets[i].bhPulled  = 0;
                }
                planets[i].orbitAngle += planets[i].orbitSpeed * sp;
                if(planets[i].orbitAngle >= 360.f) planets[i].orbitAngle -= 360.f;
                planets[i].rotAngle += planets[i].rotSpeed * sp;
                if(planets[i].rotAngle >= 360.f)   planets[i].rotAngle -= 360.f;
                continue;
            }

            if(planets[i].waitFrames > 0 && !planets[i].dragged){
                planets[i].waitFrames--;
                if(planets[i].waitFrames == 0){

                    planets[i].returning      = 1;
                    planets[i].returnProgress = 0.f;
                    planets[i].returnFromX    = planets[i].worldX;
                    planets[i].returnFromY    = planets[i].worldY;
                    planets[i].returnFromZ    = planets[i].worldZ;
                }

                planets[i].orbitAngle += planets[i].orbitSpeed * sp;
                if(planets[i].orbitAngle >= 360.f) planets[i].orbitAngle -= 360.f;
                planets[i].rotAngle += planets[i].rotSpeed * sp;
                if(planets[i].rotAngle >= 360.f)   planets[i].rotAngle -= 360.f;
                continue;
            }


            if(!planets[i].dragged){
                planets[i].orbitAngle += planets[i].orbitSpeed * sp;
                if(planets[i].orbitAngle >= 360.f) planets[i].orbitAngle -= 360.f;
                planets[i].rotAngle += planets[i].rotSpeed * sp;
                if(planets[i].rotAngle >= 360.f)   planets[i].rotAngle -= 360.f;
            }
            if(bhActive && draggedPlanet!=i && !planets[i].returning && !planets[i].waitFrames){
                float wx,wy,wz;
                if(planets[i].dragged){
                    wx=planets[i].worldX; wy=planets[i].worldY; wz=planets[i].worldZ;
                } else {
                    planetWorldPos(&planets[i],&wx,&wy,&wz);
                }

                float fx=bhX-wx, fy=bhY-wy, fz=bhZ-wz;
                float dist2=fx*fx+fy*fy+fz*fz;
                if(dist2<0.1f) dist2=0.1f;
                float dist=sqrtf(dist2);

                if(dist < 1.2f){
                    planets[i].absorbed    = 1;
                    planets[i].absorbScale = 1.f;
                    planets[i].dragged     = 0;
                    planets[i].returning   = 0;
                    planets[i].waitFrames  = 0;
                    planets[i].bhPulled    = 0;
                    planets[i].velX = planets[i].velY = planets[i].velZ = 0.f;
                    printf("%s elnyelt (csak R hozza vissza)\n",planets[i].name);
                    continue;
                }


                float str = 12.f / (dist2 + 1.f);
                if(!planets[i].dragged){
                    planets[i].dragged  = 1;
                    planets[i].bhPulled = 1;
                    planets[i].worldX   = wx;
                    planets[i].worldY   = wy;
                    planets[i].worldZ   = wz;
                }
                planets[i].velX += (fx/dist)*str*0.016f;
                planets[i].velY += (fy/dist)*str*0.016f;
                planets[i].velZ += (fz/dist)*str*0.016f;
                planets[i].velX *= 0.98f;
                planets[i].velY *= 0.98f;
                planets[i].velZ *= 0.98f;
                planets[i].worldX += planets[i].velX;
                planets[i].worldY += planets[i].velY;
                planets[i].worldZ += planets[i].velZ;
            }
        }

        for(int i=0;i<NUM_PLANETS;i++){
            if(planets[i].absorbed) continue;
            MoonGroup *mg = &moonGroups[i];
            for(int m=0;m<mg->count;m++){
                mg->moons[m].orbitAngle += mg->moons[m].orbitSpeed * sp;
                if(mg->moons[m].orbitAngle >= 360.f) mg->moons[m].orbitAngle -= 360.f;
                if(mg->moons[m].orbitAngle <    0.f) mg->moons[m].orbitAngle += 360.f;
            }
        }
    }
}

/* ════════════════════════════════════════════════════════════
   ESEMÉNYKEZELÉS
   ════════════════════════════════════════════════════════════ */
static void handleEvent(SDL_Event *e)
{
    if(e->type==SDL_QUIT){ running=0; return; }

    if(e->type==SDL_KEYDOWN){
        switch(e->key.keysym.sym){
            case SDLK_ESCAPE: running=0; break;
            case SDLK_p:  paused=!paused; break;
            case SDLK_l:  lightOn=!lightOn; break;
            case SDLK_o:  showOrbits=!showOrbits; break;
            case SDLK_n:  showNames=!showNames; break;
            case SDLK_r:  resetSim(); break;
            case SDLK_F1: showHelp=!showHelp; if(showHelp) showInfo=0; break;
            case SDLK_F2: showInfo=!showInfo; if(showInfo) showHelp=0; break;
            case SDLK_j:
                animSpeed-=ANIM_SPEED_STEP;
                if(animSpeed<ANIM_SPEED_MIN) animSpeed=ANIM_SPEED_MIN; break;
            case SDLK_k:
                animSpeed+=ANIM_SPEED_STEP;
                if(animSpeed>ANIM_SPEED_MAX) animSpeed=ANIM_SPEED_MAX; break;
            case SDLK_w: camTz+=1.f; break;
            case SDLK_s: camTz-=1.f; break;
            case SDLK_a: camTx+=1.f; break;
            case SDLK_d: camTx-=1.f; break;
            case SDLK_q: camTy-=1.f; break;
            case SDLK_e: camTy+=1.f; break;
            case SDLK_1: camXRot=90.f; camYRot=0.f; camTx=0.f; camTy=-5.f; camTz=-55.f; break;
            case SDLK_2: camXRot=20.f; camYRot=0.f; camTx=0.f; camTy=0.f;  camTz=-35.f; break;
            case SDLK_RIGHT:
                selectedPlanet++;
                if(selectedPlanet >= NUM_PLANETS) selectedPlanet = -1;
                showCard = 1;
                break;
            case SDLK_LEFT:
                selectedPlanet--;
                if(selectedPlanet < -1) selectedPlanet = NUM_PLANETS - 1;
                showCard = 1;
                break;
            case SDLK_UP:   showCard = 1; break;
            case SDLK_DOWN: showCard = 0; break;
        }
    }


    if(e->type==SDL_MOUSEBUTTONDOWN && e->button.button==SDL_BUTTON_RIGHT){
        bhActive=1;
        float wx,wy,wz;
        if(mouseToWorld(e->button.x,e->button.y,&wx,&wy,&wz)){
            bhX=wx; bhY=wy; bhZ=wz;
        }
    }
    if(e->type==SDL_MOUSEBUTTONUP && e->button.button==SDL_BUTTON_RIGHT){
        bhActive=0;
        for(int i=0;i<NUM_PLANETS;i++){
            if(!planets[i].absorbed && planets[i].bhPulled && planets[i].dragged){
                planets[i].dragged    = 0;
                planets[i].bhPulled   = 0;
                planets[i].waitFrames = WAIT_FRAMES;
                planets[i].velX = planets[i].velY = planets[i].velZ = 0.f;
            }
        }
    }


    if(e->type==SDL_MOUSEBUTTONDOWN && e->button.button==SDL_BUTTON_LEFT){
        mouseDown=1;
        lastMX=e->button.x; lastMY=e->button.y;
        clickMX=e->button.x; clickMY=e->button.y;
        didDrag=0;

        draggedPlanet = pickPlanet(e->button.x, e->button.y);
        if(draggedPlanet >= 0){
            Planet *p = &planets[draggedPlanet];
            float ox,oy,oz;
            if(!p->dragged && !p->returning && !p->waitFrames)
                planetWorldPos(p,&ox,&oy,&oz);
            else {
                ox=p->worldX; oy=p->worldY; oz=p->worldZ;
            }
            p->worldX=ox; p->worldY=oy; p->worldZ=oz;
            p->returning=0; p->returnProgress=0.f;
            p->waitFrames=0;
            p->velX=p->velY=p->velZ=0.f;
            p->bhPulled=0;
        }
    }

    if(e->type==SDL_MOUSEBUTTONUP && e->button.button==SDL_BUTTON_LEFT){
        int ddx=e->button.x-clickMX, ddy=e->button.y-clickMY;

        if(ddx*ddx+ddy*ddy < 25 && !didDrag){
            int hit=pickPlanet(e->button.x,e->button.y);
            if(hit>=0) selectedPlanet=hit;
            else selectedPlanet=-1;
        }

        if(draggedPlanet >= 0 && didDrag){
            Planet *p = &planets[draggedPlanet];
            p->dragged    = 0;
            p->waitFrames = WAIT_FRAMES;
        }

        draggedPlanet=-1; mouseDown=0; didDrag=0;
    }

    if(e->type==SDL_MOUSEMOTION){
        int ddx=e->motion.x-clickMX, ddy=e->motion.y-clickMY;
        if(ddx*ddx+ddy*ddy > 25) didDrag=1;

        if(draggedPlanet>=0 && didDrag){
            float wx,wy,wz;
            if(mouseToWorld(e->motion.x,e->motion.y,&wx,&wy,&wz)){
                planets[draggedPlanet].dragged=1;
                planets[draggedPlanet].worldX=wx;
                planets[draggedPlanet].worldY=wy;
                planets[draggedPlanet].worldZ=wz;
            }
        } else if(mouseDown){
            camYRot += (e->motion.x - lastMX) * 0.4f;
            camXRot += (e->motion.y - lastMY) * 0.4f;
        }
        lastMX=e->motion.x; lastMY=e->motion.y;

        if(bhActive){
            float wx,wy,wz;
            if(mouseToWorld(e->motion.x,e->motion.y,&wx,&wy,&wz)){
                bhX=wx; bhY=wy; bhZ=wz;
            }
        }
    }

    if(e->type==SDL_MOUSEWHEEL) camTz += e->wheel.y * 1.5f;
}

/* ════════════════════════════════════════════════════════════
   OPENGL INIT
   ════════════════════════════════════════════════════════════ */
static void initGL(void)
{
    glClearColor(0.f,0.f,0.04f,1.f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(60.0,(double)WIN_W/WIN_H,0.1,600.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

/* ════════════════════════════════════════════════════════════
   MAIN
   ════════════════════════════════════════════════════════════ */
int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    for(int i=0;i<NUM_PLANETS;i++){
        initOrbitAngle[i]=planets[i].orbitAngle;
        for(int m=0;m<moonGroups[i].count;m++)
            initMoonAngles[i][m]=moonGroups[i].moons[m].orbitAngle;
    }

    if(SDL_Init(SDL_INIT_VIDEO)<0){
        fprintf(stderr,"SDL: %s\n",SDL_GetError()); return 1;
    }
    if(!(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG)&(IMG_INIT_PNG|IMG_INIT_JPG))){
        fprintf(stderr,"IMG: %s\n",IMG_GetError()); SDL_Quit(); return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

    SDL_Window *win = SDL_CreateWindow("Naprendszer Szimulacio",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    if(!win){ fprintf(stderr,"Ablak: %s\n",SDL_GetError()); IMG_Quit(); SDL_Quit(); return 1; }

    SDL_GLContext glCtx = SDL_GL_CreateContext(win);
    if(!glCtx){ fprintf(stderr,"GL: %s\n",SDL_GetError()); SDL_DestroyWindow(win); IMG_Quit(); SDL_Quit(); return 1; }
    SDL_GL_SetSwapInterval(1);

    initGL();
    initStars();
    sunTexID = loadTexture("img/sol.png");
    moonTexID = loadTexture("img/hold.png");
    for(int i=0;i<NUM_PLANETS;i++) planets[i].texID = loadTexture(planets[i].texPath);

    Uint32 prev = SDL_GetTicks();
    while(running){
        SDL_Event ev;
        while(SDL_PollEvent(&ev)) handleEvent(&ev);
        Uint32 now = SDL_GetTicks();
        if(now - prev >= 16){ update(); prev=now; }
        render();
        SDL_GL_SwapWindow(win);
    }

    SDL_GL_DeleteContext(glCtx);
    SDL_DestroyWindow(win);
    IMG_Quit(); SDL_Quit();
    return 0;
}