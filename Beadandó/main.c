/*
 * Naprendszer Szimulacio v9
 * SDL2 + SDL2_image + OpenGL 1.x / GLU
 *
 * Forditas (Windows MinGW):
 *   gcc main.c -o solar.exe -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm
 *
 * Forditas (Linux):
 *   gcc main.c -o solar $(sdl2-config --cflags --libs) -lSDL2_image -lGL -lGLU -lm
 *
 * Adatfajlok (assets/ mappaban):
 *   assets/planets.csv  - bolygoadatok
 *   assets/moons.csv    - holdadatok
 */

#include "solar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static AppState g_state;

/* ════════════════════════════════════════════════════════════
   BITMAP FONT (5x8)
   ════════════════════════════════════════════════════════════ */
static const unsigned char g_font5x8[][5] = {
/* 32  ' ' */ {0x00,0x00,0x00,0x00,0x00},
/* 33  '!' */ {0x00,0x00,0x5F,0x00,0x00},
/* 34  '"' */ {0x00,0x07,0x00,0x07,0x00},
/* 35  '#' */ {0x14,0x7F,0x14,0x7F,0x14},
/* 36  '$' */ {0x24,0x2A,0x7F,0x2A,0x12},
/* 37  '%' */ {0x23,0x13,0x08,0x64,0x62},
/* 38  '&' */ {0x36,0x49,0x55,0x22,0x50},
/* 39  '\''*/ {0x00,0x05,0x03,0x00,0x00},
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
/* 92 '\\'*/ {0x02,0x04,0x08,0x10,0x20},
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

/* ════════════════════════════════════════════════════════════
   CSV BETOLTO FUGGVENYEK
   ════════════════════════════════════════════════════════════ */

static int csvSplit(char *line, char **fields, int maxFields)
{
    int n = 0;
    char *p = line;
    while (n < maxFields) {
        fields[n++] = p;
        p = strchr(p, ',');
        if (!p) break;
        *p = '\0';
        p++;
    }
    return n;
}

static char *csvTrim(char *s)
{
    while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n'))
        *end-- = '\0';
    return s;
}

int loadPlanetsCSV(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "CSV hiba: nem nyithato: %s\n", path); return -1; }

    char line[512];
    int count = 0;

    strncpy(g_state.planetInfos[0].name,  "Nap",                   sizeof g_state.planetInfos[0].name  - 1);
    strncpy(g_state.planetInfos[0].type,  "Csillag (G-tipus)",     sizeof g_state.planetInfos[0].type  - 1);
    strncpy(g_state.planetInfos[0].extra, "Tomege a Naprendszer 99.86%-a", sizeof g_state.planetInfos[0].extra - 1);
    g_state.planetInfos[0].diameter_km = 1392700.f;
    g_state.planetInfos[0].mass_1024kg = 1989000.f;
    g_state.planetInfos[0].orbit_days  = 0;
    g_state.planetInfos[0].dist_mkm    = 0.f;
    g_state.planetInfos[0].moons       = 0;
    g_state.planetInfos[0].temp_c      = 5505.f;
    g_state.numPlanetInfos = 1;

    while (fgets(line, sizeof line, f) && count < NUM_PLANETS) {
        char *trimmed = csvTrim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue; /* komment / ures sor */

        char *fields[22];
        char linecopy[512];
        strncpy(linecopy, trimmed, sizeof linecopy - 1);
        int n = csvSplit(linecopy, fields, 22);
        if (n < 20) { fprintf(stderr, "CSV figyelmeztes: rövid sor (%d mezo): %s\n", n, trimmed); continue; }

        Planet *p = &g_state.planets[count];
        memset(p, 0, sizeof *p);

        strncpy(p->name,    csvTrim(fields[0]),  sizeof p->name    - 1);
        p->radius      = (float)atof(csvTrim(fields[1]));
        p->orbitRadius = (float)atof(csvTrim(fields[2]));
        p->orbitSpeed  = (float)atof(csvTrim(fields[3]));
        p->rotSpeed    = (float)atof(csvTrim(fields[4]));
        p->orbitAngle  = (float)atof(csvTrim(fields[5]));
        p->rotAngle    = 0.f;
        p->tilt        = (float)atof(csvTrim(fields[6]));
        p->orbitTilt   = (float)atof(csvTrim(fields[7]));
        strncpy(p->texPath, csvTrim(fields[8]), sizeof p->texPath - 1);
        p->r           = (float)atof(csvTrim(fields[9]));
        p->g           = (float)atof(csvTrim(fields[10]));
        p->b           = (float)atof(csvTrim(fields[11]));
        p->absorbScale = 1.f;

        PlanetInfo *pi = &g_state.planetInfos[g_state.numPlanetInfos];
        memset(pi, 0, sizeof *pi);
        strncpy(pi->name,  csvTrim(fields[0]),  sizeof pi->name  - 1);
        strncpy(pi->type,  csvTrim(fields[12]), sizeof pi->type  - 1);
        pi->diameter_km  = (float)atof(csvTrim(fields[13]));
        pi->mass_1024kg  = (float)atof(csvTrim(fields[14]));
        pi->orbit_days   = atoi(csvTrim(fields[15]));
        pi->dist_mkm     = (float)atof(csvTrim(fields[16]));
        pi->moons        = atoi(csvTrim(fields[17]));
        pi->temp_c       = (float)atof(csvTrim(fields[18]));
        strncpy(pi->extra, csvTrim(fields[19]), sizeof pi->extra - 1);
        g_state.numPlanetInfos++;

        count++;
    }
    fclose(f);
    printf("Betoltve %d bolygo a %s fajlbol.\n", count, path);
    return count;
}

int loadMoonsCSV(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "CSV hiba: nem nyithato: %s\n", path); return -1; }

    memset(g_state.moonGroups, 0, sizeof g_state.moonGroups);

    char line[256];
    int total = 0;

    while (fgets(line, sizeof line, f)) {
        char *trimmed = csvTrim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;

        char *fields[8];
        char linecopy[256];
        strncpy(linecopy, trimmed, sizeof linecopy - 1);
        int n = csvSplit(linecopy, fields, 8);
        if (n < 7) continue;

        int pi = atoi(csvTrim(fields[0]));
        if (pi < 0 || pi >= NUM_PLANETS) continue;

        MoonGroup *mg = &g_state.moonGroups[pi];
        if (mg->count >= MAX_MOONS) continue;

        Moon *mo = &mg->moons[mg->count];
        strncpy(mo->name,  csvTrim(fields[1]), sizeof mo->name - 1);
        mo->orbitRadius = (float)atof(csvTrim(fields[2]));
        mo->orbitSpeed  = (float)atof(csvTrim(fields[3]));
        mo->orbitAngle  = (float)atof(csvTrim(fields[4]));
        mo->orbitTilt   = (float)atof(csvTrim(fields[5]));
        mo->radius      = (float)atof(csvTrim(fields[6]));
        mg->count++;
        total++;
    }
    fclose(f);
    printf("Betoltve %d hold a %s fajlbol.\n", total, path);
    return total;
}

/* ════════════════════════════════════════════════════════════
   FONT RENDERING
   ════════════════════════════════════════════════════════════ */
void fontDrawCharToSurface(SDL_Surface *surf, int cx, int cy,
                                   unsigned char ch,
                                   Uint8 r, Uint8 g, Uint8 b)
{
    int idx = (int)ch - 32;
    int maxidx = (int)(sizeof g_font5x8 / sizeof g_font5x8[0]);
    if (idx < 0 || idx >= maxidx) return;
    Uint32 color = SDL_MapRGBA(surf->format, r, g, b, 255);
    Uint32 *pixels = (Uint32*)surf->pixels;
    int pitch = surf->pitch / 4;
    for (int col = 0; col < 5; col++) {
        unsigned char coldata = g_font5x8[idx][col];
        for (int row = 0; row < 8; row++) {
            if (coldata & (0x80 >> row)) {
                int px = cx + col, py = cy + row;
                if (px >= 0 && px < surf->w && py >= 0 && py < surf->h)
                    pixels[py * pitch + px] = color;
            }
        }
    }
}

void fontDrawStringToSurface(SDL_Surface *surf, int x, int y,
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
                for (int row = 0; row < 8; row++)
                    for (int col = 0; col < 6; col++) {
                        Uint32 px = src[row * (tmp->pitch/4) + col];
                        if (px != blank)
                            for (int sy = 0; sy < scale; sy++)
                                for (int sx = 0; sx < scale; sx++) {
                                    int fx = cx + col*scale + sx;
                                    int fy = y  + row*scale + sy;
                                    if (fx>=0&&fx<surf->w&&fy>=0&&fy<surf->h)
                                        dst[fy*dstpitch+fx] = px;
                                }
                    }
                SDL_FreeSurface(tmp);
            }
        }
        cx += 6 * scale;
        str++;
    }
}

void drawTextGL(float x, float y, const char *str,
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
    fontDrawStringToSurface(surf, 0, 0, str,
                            (Uint8)(cr*255), (Uint8)(cg*255), (Uint8)(cb*255), scale);
    GLuint texid;
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);
    SDL_FreeSurface(surf);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D); glColor4f(1.f,1.f,1.f,1.f);
    glBegin(GL_QUADS);
        glTexCoord2f(0,1); glVertex2f(x,    y);
        glTexCoord2f(1,1); glVertex2f(x+tw, y);
        glTexCoord2f(1,0); glVertex2f(x+tw, y+th);
        glTexCoord2f(0,0); glVertex2f(x,    y+th);
    glEnd();
    glDisable(GL_TEXTURE_2D); glDisable(GL_BLEND);
    glDeleteTextures(1, &texid);
}

int textW(const char *s, int scale) { return (int)strlen(s) * 6 * scale; }
int textH(int scale)                { return 8 * scale; }

/* ════════════════════════════════════════════════════════════
   TEXTURA BETOLTES
   ════════════════════════════════════════════════════════════ */
GLuint loadTexture(const char *path)
{
    SDL_Surface *surf = IMG_Load(path);
    if (!surf) { fprintf(stderr, "Tex hiba (%s): %s\n", path, IMG_GetError()); return 0; }
    SDL_Surface *conv = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surf);
    if (!conv) return 0;
    GLuint id; glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,conv->w,conv->h,0,GL_RGBA,GL_UNSIGNED_BYTE,conv->pixels);
    SDL_FreeSurface(conv);
    return id;
}

/* ════════════════════════════════════════════════════════════
   CSILLAGMEZO
   ════════════════════════════════════════════════════════════ */
void initStars(void)
{
    srand(42);
    for (int i = 0; i < NUM_STARS; i++) {
        float theta = ((float)rand()/RAND_MAX) * 2.f * PI;
        float phi   = acosf(1.f - 2.f * ((float)rand()/RAND_MAX));
        float r     = 300.f + ((float)rand()/RAND_MAX) * 200.f;
        g_state.stars[i].x  = r * sinf(phi) * cosf(theta);
        g_state.stars[i].y  = r * sinf(phi) * sinf(theta);
        g_state.stars[i].z  = r * cosf(phi);
        float warm  = (float)rand()/RAND_MAX;
        if (warm > 0.6f) {
            g_state.stars[i].r = 1.0f;
            g_state.stars[i].g = 0.95f + (float)rand()/RAND_MAX * 0.05f;
            g_state.stars[i].b = 0.6f  + (float)rand()/RAND_MAX * 0.2f;
        } else {
            float t = (float)rand()/RAND_MAX;
            g_state.stars[i].r = 0.85f + t * 0.15f;
            g_state.stars[i].g = 0.85f + t * 0.15f;
            g_state.stars[i].b = 0.90f + (float)rand()/RAND_MAX * 0.10f;
        }
        float rnd = (float)rand()/RAND_MAX;
        g_state.stars[i].size = (rnd < 0.75f) ? 1.0f : (rnd < 0.93f) ? 1.5f : 2.0f;
    }
}

void drawStars(void)
{
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);

    glPointSize(1.0f); glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        if (g_state.stars[i].size > 1.0f) continue;
        float alpha = 0.5f + ((float)(i%17)/17.f)*0.5f;
        glColor4f(g_state.stars[i].r, g_state.stars[i].g, g_state.stars[i].b, alpha);
        glVertex3f(g_state.stars[i].x, g_state.stars[i].y, g_state.stars[i].z);
    }
    glEnd();

    glPointSize(1.5f); glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        if (g_state.stars[i].size != 1.5f) continue;
        glColor4f(g_state.stars[i].r, g_state.stars[i].g, g_state.stars[i].b, 0.85f);
        glVertex3f(g_state.stars[i].x, g_state.stars[i].y, g_state.stars[i].z);
    }
    glEnd();

    glPointSize(2.0f); glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; i++) {
        if (g_state.stars[i].size < 2.0f) continue;
        glColor4f(g_state.stars[i].r, g_state.stars[i].g, g_state.stars[i].b, 1.0f);
        glVertex3f(g_state.stars[i].x, g_state.stars[i].y, g_state.stars[i].z);
    }
    glEnd();

    glDisable(GL_POINT_SMOOTH); glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST);
    if (g_state.lightOn) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
}

/* ════════════════════════════════════════════════════════════
   BOLYGOPOZICIO
   ════════════════════════════════════════════════════════════ */
void planetWorldPos(Planet *p, float *ox, float *oy, float *oz)
{
    float ang  = DEG2RAD(p->orbitAngle);
    float tilt = DEG2RAD(p->orbitTilt);
    float x    =  p->orbitRadius * cosf(ang);
    float z    =  p->orbitRadius * sinf(ang);
    *ox = x;
    *oy = -z * sinf(tilt);
    *oz =  z * cosf(tilt);
}

/* ════════════════════════════════════════════════════════════
   2D OVERLAY
   ════════════════════════════════════════════════════════════ */
void beginOrtho(void)
{
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, g_state.winW, g_state.winH, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST); glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
}

void endOrtho(void)
{
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);  glPopMatrix();
    if (g_state.lightOn) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
}

void fillRect(float x, float y, float w, float h,
                     float r, float g, float b, float a)
{
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r,g,b,a);
    glBegin(GL_QUADS);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd(); glDisable(GL_BLEND);
}

void strokeRect(float x, float y, float w, float h,
                        float r, float g, float b, float lw)
{
    glLineWidth(lw); glColor3f(r,g,b);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x,y); glVertex2f(x+w,y); glVertex2f(x+w,y+h); glVertex2f(x,y+h);
    glEnd(); glLineWidth(1.f);
}

/* ════════════════════════════════════════════════════════════
   3D RAJZOLAS
   ════════════════════════════════════════════════════════════ */
void drawTexturedSphere(float radius, GLuint texID, int sl)
{
    GLUquadric *q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    if (texID) { gluQuadricTexture(q,GL_TRUE); glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texID); }
    else glDisable(GL_TEXTURE_2D);
    gluSphere(q, radius, sl, sl/2);
    gluDeleteQuadric(q);
    glDisable(GL_TEXTURE_2D);
}

void drawOrbit(float r, float tilt)
{
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.f,1.f,1.f,0.15f);
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<360; i++) {
        float a  = DEG2RAD((float)i);
        float x  = r * cosf(a);
        float z  = r * sinf(a);
        float tr = DEG2RAD(tilt);
        glVertex3f(x, -z*sinf(tr), z*cosf(tr));
    }
    glEnd(); glDisable(GL_BLEND);
}

void drawRings(float inner, float outer)
{
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i=0; i<=360; i++) {
        float a  = DEG2RAD((float)i);
        float al = (i%2==0) ? 0.55f : 0.35f;
        glColor4f(0.90f,0.82f,0.60f,al);
        glVertex3f(inner*cosf(a), 0.f, inner*sinf(a));
        glColor4f(0.80f,0.70f,0.45f,al*0.6f);
        glVertex3f(outer*cosf(a), 0.f, outer*sinf(a));
    }
    glEnd(); glDisable(GL_BLEND);
}

void configLight(void)
{
    if (!g_state.lightOn) { glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); return; }
    float iv = g_state.lightIntensity;

    GLfloat zero[]      = {0.f, 0.f, 0.f, 1.f};
    GLfloat dif[]       = {iv, iv, iv, 1.f};
    GLfloat spe[]       = {iv*0.4f, iv*0.4f, iv*0.4f, 1.f};
    GLfloat pos[]       = {0.f, 0.f, 0.f, 1.f};
    float ga = 0.015f * iv;
    GLfloat globalAmb[] = {ga, ga, ga, 1.f};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  zero);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spe);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.003f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
}

void drawSun(void)
{
    glDisable(GL_LIGHTING);
    glPushMatrix(); glRotatef(g_state.sunRot, 0.f,1.f,0.f);
    glColor3f(1.f,1.f,1.f);
    drawTexturedSphere(2.0f, g_state.sunTexID, QUALITY);
    glPopMatrix();
    if (g_state.lightOn) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
}

void drawPlanet(Planet *p)
{
    if (p->absorbed && p->absorbScale <= 0.01f) return;
    glPushMatrix();

    if (p->returning) {
        float t  = p->returnProgress;
        float ft = 1.f - (1.f-t)*(1.f-t)*(1.f-t);
        float tx,ty,tz; planetWorldPos(p,&tx,&ty,&tz);
        glTranslatef(p->returnFromX+(tx-p->returnFromX)*ft,
                     p->returnFromY+(ty-p->returnFromY)*ft,
                     p->returnFromZ+(tz-p->returnFromZ)*ft);
    } else if (p->absorbed || p->dragged || p->waitFrames > 0) {
        glTranslatef(p->worldX, p->worldY, p->worldZ);
    } else {
        float px,py,pz; planetWorldPos(p,&px,&py,&pz);
        glTranslatef(px,py,pz);
    }

    if (p->absorbed) glScalef(p->absorbScale, p->absorbScale, p->absorbScale);
    glRotatef(p->tilt,     0.f, 0.f, 1.f);
    glRotatef(p->rotAngle, 0.f, 1.f, 0.f);

    if (g_state.lightOn) {
        float iv = g_state.lightIntensity;
        GLfloat matAmb[]  = {p->r, p->g, p->b, 1.f};
        GLfloat matDiff[] = {p->r, p->g, p->b, 1.f};
        GLfloat matSpec[] = {.15f*.15f*iv,.15f*.15f*iv,.15f*.15f*iv,1.f};
        GLfloat matShin[] = {8.f};
        GLfloat matEmit[] = {0.f,0.f,0.f,1.f};
        glMaterialfv(GL_FRONT,GL_AMBIENT,   matAmb);
        glMaterialfv(GL_FRONT,GL_DIFFUSE,   matDiff);
        glMaterialfv(GL_FRONT,GL_SPECULAR,  matSpec);
        glMaterialfv(GL_FRONT,GL_SHININESS, matShin);
        glMaterialfv(GL_FRONT,GL_EMISSION,  matEmit);
        glColor3f(1.f,1.f,1.f);
    } else {
        glColor3f(p->r, p->g, p->b);
    }
    drawTexturedSphere(p->radius, p->texID, QUALITY);

    if (strcmp(p->name,"Szaturnusz") == 0)
        drawRings(p->radius*1.35f, p->radius*2.4f);

    int pi = (int)(p - g_state.planets);
    MoonGroup *mg = &g_state.moonGroups[pi];
    for (int m = 0; m < mg->count; m++) {
        Moon *mo = &mg->moons[m];
        glPushMatrix();
        float moonAng = DEG2RAD(mo->orbitAngle);
        float moonTilt = DEG2RAD(mo->orbitTilt);
        float mx =  mo->orbitRadius * cosf(moonAng);
        float mz =  mo->orbitRadius * sinf(moonAng);
        glTranslatef(mx, -mz*sinf(moonTilt), mz*cosf(moonTilt));
        if (g_state.lightOn) {
            float iv = g_state.lightIntensity;
            GLfloat moonAmb[]  = {0.8f, 0.8f, 0.8f, 1.f};
            GLfloat moonDiff[] = {0.75f,0.75f,0.75f,1.f};
            GLfloat moonSpec[] = {0.05f*iv,0.05f*iv,0.05f*iv,1.f};
            GLfloat moonShin[] = {4.f};
            GLfloat moonEmit[] = {0.f,0.f,0.f,1.f};
            glMaterialfv(GL_FRONT,GL_AMBIENT,   moonAmb);
            glMaterialfv(GL_FRONT,GL_DIFFUSE,   moonDiff);
            glMaterialfv(GL_FRONT,GL_SPECULAR,  moonSpec);
            glMaterialfv(GL_FRONT,GL_SHININESS, moonShin);
            glMaterialfv(GL_FRONT,GL_EMISSION,  moonEmit);
            glColor3f(1.f,1.f,1.f);
        } else {
            glColor3f(0.75f,0.75f,0.75f);
        }
        drawTexturedSphere(mo->radius, g_state.moonTexID, 16);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawBlackHole(void)
{
    if (!g_state.bhActive) return;
    glPushMatrix(); glTranslatef(g_state.bhX, g_state.bhY, g_state.bhZ);
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glColor3f(0.f,0.f,0.f);
    GLUquadric *q = gluNewQuadric(); gluQuadricNormals(q,GLU_SMOOTH);
    gluSphere(q,0.8f,32,16); gluDeleteQuadric(q);
    float pulse = 0.5f+0.5f*sinf(g_state.bhPulse);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    for (int i=0; i<5; i++) {
        float ri = 0.9f+i*0.35f+pulse*0.1f;
        float al = (0.7f-i*0.13f)*(1.f-(float)i/7.f);
        glColor4f(0.6f+pulse*0.3f,0.1f,0.9f,al);
        glBegin(GL_LINE_LOOP);
        for (int j=0; j<64; j++) { float a=DEG2RAD((float)j*360.f/64.f); glVertex3f(ri*cosf(a),0.f,ri*sinf(a)); }
        glEnd();
    }
    glPushMatrix(); glRotatef(g_state.bhPulse*10.f,0.f,1.f,0.f);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i=0; i<=72; i++) {
        float a=DEG2RAD((float)i*360.f/72.f), ac=(i%2==0)?.55f:.25f;
        glColor4f(.8f,.3f,1.f,ac); glVertex3f(1.1f*cosf(a),0.f,1.1f*sinf(a));
        glColor4f(.4f,0.f,.6f,ac*.4f); glVertex3f(2.2f*cosf(a),0.f,2.2f*sinf(a));
    }
    glEnd(); glPopMatrix();
    glDisable(GL_BLEND);
    if (g_state.lightOn) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
    glPopMatrix();
}

/* ════════════════════════════════════════════════════════════
   RAY PICKING
   ════════════════════════════════════════════════════════════ */
int rayPick(int mx, int my,
                   double *nx, double *ny, double *nz,
                   double *dx, double *dy, double *dz)
{
    GLdouble mv[16], proj[16]; GLint vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, mv);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, vp);
    double wy = (double)(g_state.winH - my);
    GLdouble fx,fy,fz;
    gluUnProject((double)mx,wy,0.0,mv,proj,vp,nx,ny,nz);
    gluUnProject((double)mx,wy,1.0,mv,proj,vp,&fx,&fy,&fz);
    *dx=fx-*nx; *dy=fy-*ny; *dz=fz-*nz;
    double len = sqrt((*dx)*(*dx)+(*dy)*(*dy)+(*dz)*(*dz));
    if (len<1e-9) return 0;
    *dx/=len; *dy/=len; *dz/=len;
    return 1;
}

static double sphereRayT(double ox,double oy,double oz,
                          double dx,double dy,double dz,
                          double cx,double cy,double cz,double r)
{
    double ex=ox-cx, ey=oy-cy, ez=oz-cz;
    double B=2.*(ex*dx+ey*dy+ez*dz);
    double C=ex*ex+ey*ey+ez*ez-r*r;
    double D=B*B-4.*C;
    if (D<0.) return -1.;
    double t=(-B-sqrt(D))/2.;
    if (t<0.01) t=(-B+sqrt(D))/2.;
    return t;
}

int pickPlanet(int mx, int my)
{
    double nx,ny,nz,dx,dy,dz;
    if (!rayPick(mx,my,&nx,&ny,&nz,&dx,&dy,&dz)) return -1;
    int best=-1; double bestT=1e18;
    for (int i=0; i<NUM_PLANETS; i++) {
        if (g_state.planets[i].absorbed && g_state.planets[i].absorbScale<0.1f) continue;
        double cx,cy,cz;
        if (g_state.planets[i].dragged || g_state.planets[i].waitFrames>0 || g_state.planets[i].returning) {
            cx=g_state.planets[i].worldX; cy=g_state.planets[i].worldY; cz=g_state.planets[i].worldZ;
        } else {
            float ox,oy,oz; planetWorldPos(&g_state.planets[i],&ox,&oy,&oz);
            cx=ox; cy=oy; cz=oz;
        }
        double t = sphereRayT(nx,ny,nz,dx,dy,dz,cx,cy,cz,(double)(g_state.planets[i].radius*1.4));
        if (t>0.01 && t<bestT) { bestT=t; best=i; }
    }
    return best;
}

int mouseToWorld(int mx, int my, float *wx, float *wy2, float *wz)
{
    double nx,ny,nz,dx,dy,dz;
    if (!rayPick(mx,my,&nx,&ny,&nz,&dx,&dy,&dz)) return 0;
    if (fabs(dy)<1e-6) { *wx=(float)nx; *wy2=0.f; *wz=(float)nz; return 1; }
    double t = -ny/dy;
    *wx=(float)(nx+t*dx); *wy2=0.f; *wz=(float)(nz+t*dz);
    return 1;
}

/* ════════════════════════════════════════════════════════════
   HUD
   ════════════════════════════════════════════════════════════ */
void drawHUD(void)
{
    char buf[256];
    int S  = 2;
    int SB = 2;
    int LH = textH(S) + 5;

    beginOrtho();

    int topH = 5 + textH(SB) + 6;
    fillRect(0, 0, g_state.winW, topH, 0.f,0.f,0.f, 0.68f);
    strokeRect(0, topH-1, g_state.winW, 1, 0.6f,0.5f,0.1f, 1.f);
    drawTextGL(10, 5, "NAPRENDSZER SZIMULACIO", 1.f,0.78f,0.2f, SB);
    drawTextGL(g_state.winW - textW("F1=Sugo  F2=Info", SB) - 10, 5,
               "F1=Sugo  F2=Info", 0.39f,0.9f,0.39f, SB);

    if (g_state.selectedPlanet >= 0) {
        snprintf(buf,sizeof buf,">> %s <<", g_state.planets[g_state.selectedPlanet].name);
        int tw = textW(buf, SB);
        int bx = (g_state.winW - tw)/2 - 8;
        fillRect(bx, 3, tw+16, textH(SB)+4, 0.f,0.f,0.f,0.85f);
        strokeRect(bx, 3, tw+16, textH(SB)+4,
                   g_state.planets[g_state.selectedPlanet].r,
                   g_state.planets[g_state.selectedPlanet].g,
                   g_state.planets[g_state.selectedPlanet].b, 2.f);
        drawTextGL(bx+8, 5, buf,
                   g_state.planets[g_state.selectedPlanet].r,
                   g_state.planets[g_state.selectedPlanet].g,
                   g_state.planets[g_state.selectedPlanet].b, SB);
    }

    if (g_state.showNames) {
        int pw = 220, startY = topH + 6;
        int ph = textH(S)+8 + NUM_PLANETS*(textH(S)+4) + 6;
        fillRect(g_state.winW-pw-4, startY, pw+4, ph, 0.f,0.f,0.f,0.6f);
        drawTextGL(g_state.winW-pw, startY+4, "Bolygok:", 0.7f,0.7f,1.f, S);
        for (int i=0; i<NUM_PLANETS; i++) {
            float nr=g_state.planets[i].r, ng=g_state.planets[i].g, nb=g_state.planets[i].b;
            if (i==g_state.selectedPlanet) { nr=1.f; ng=1.f; nb=0.3f; }
            if      (g_state.planets[i].absorbed)       snprintf(buf,sizeof buf,"%s [ELNYELT]",g_state.planets[i].name);
            else if (g_state.planets[i].returning)       snprintf(buf,sizeof buf,"%s [VISSZA]", g_state.planets[i].name);
            else if (g_state.planets[i].waitFrames > 0)  snprintf(buf,sizeof buf,"%s [VAR]",    g_state.planets[i].name);
            else if (g_state.planets[i].dragged)         snprintf(buf,sizeof buf,"%s [MOZOG]",  g_state.planets[i].name);
            else                                  snprintf(buf,sizeof buf,"%s",          g_state.planets[i].name);
            drawTextGL(g_state.winW-pw,
                       startY+4+textH(S)+6+i*(textH(S)+4),
                       buf, nr,ng,nb, S);
        }
    }

    if (g_state.showCard && g_state.selectedPlanet >= -1) {
        const PlanetInfo *pi = &g_state.planetInfos[g_state.selectedPlanet + 1];
        float pr=1.f, pg=1.f, pb=1.f;
        if (g_state.selectedPlanet >= 0) {
            pr=g_state.planets[g_state.selectedPlanet].r;
            pg=g_state.planets[g_state.selectedPlanet].g;
            pb=g_state.planets[g_state.selectedPlanet].b;
        } else { pr=1.f; pg=0.78f; pb=0.2f; }

        char l_type[64], l_diam[64], l_mass[64], l_orbit[64], l_dist[64], l_moons[64], l_temp[64];
        snprintf(l_type,  sizeof l_type,  "Tipus  : %s", pi->type);
        snprintf(l_diam,  sizeof l_diam,  "Atmero : %.0f km", pi->diameter_km);
        if (pi->mass_1024kg >= 1000.f)
            snprintf(l_mass, sizeof l_mass, "Tomeg  : %.0f * 10^24 kg", pi->mass_1024kg);
        else
            snprintf(l_mass, sizeof l_mass, "Tomeg  : %.3f * 10^24 kg", pi->mass_1024kg);
        if (pi->orbit_days == 0)
            snprintf(l_orbit, sizeof l_orbit, "Keringo: -");
        else if (pi->orbit_days < 365)
            snprintf(l_orbit, sizeof l_orbit, "Keringo: %d nap", pi->orbit_days);
        else
            snprintf(l_orbit, sizeof l_orbit, "Keringo: %.1f ev", pi->orbit_days/365.25f);
        if (pi->dist_mkm == 0.f)
            snprintf(l_dist,  sizeof l_dist,  "Tavolsag: -");
        else
            snprintf(l_dist,  sizeof l_dist,  "Tavolsag: %.1f M km", pi->dist_mkm);
        snprintf(l_moons, sizeof l_moons, "Holdak : %d", pi->moons);
        snprintf(l_temp,  sizeof l_temp,  "Homerseklet: %.0f C", pi->temp_c);

        const char *cardlines[] = { l_type,l_diam,l_mass,l_orbit,l_dist,l_moons,l_temp,pi->extra };
        int nc=8, lsc=1, clh=textH(lsc)+4;
        int cpw=0;
        for (int i=0; i<nc; i++) { int w=textW(cardlines[i],lsc); if (w>cpw) cpw=w; }
        cpw += 28;
        int titleH=textH(SB)+10, cph=titleH+nc*clh+10;
        int cpx=8, cpy=g_state.winH-cph-8;
        fillRect(cpx-3, cpy-3, cpw+6, cph+6, pr*.6f,pg*.6f,pb*.6f, 0.92f);
        fillRect(cpx, cpy, cpw, cph, 0.f,0.f,0.08f, 0.95f);
        drawTextGL(cpx+(cpw-textW(pi->name,SB))/2, cpy+6, pi->name, pr,pg,pb, SB);
        int cy2=cpy+titleH;
        for (int i=0; i<nc; i++) {
            float lr=0.82f,lg=0.82f,lb=0.82f;
            if (i==nc-1) { lr=pr*.9f+.1f; lg=pg*.7f+.3f; lb=pb*.5f+.3f; }
            drawTextGL(cpx+10, cy2, cardlines[i], lr,lg,lb, lsc);
            cy2 += clh;
        }
        const char *nav="< > valtas  |  le: bezar";
        drawTextGL(cpx+(cpw-textW(nav,lsc))/2, cpy+cph-clh+2, nav, 0.4f,0.4f,0.4f, lsc);
    }

    if (g_state.bhActive) {
        float pulse=0.5f+0.5f*sinf(g_state.bhPulse);
        const char *bt="FEKETE LYUK AKTIV";
        int tw=textW(bt,S), bx=(g_state.winW-tw)/2-8, by=g_state.winH-textH(S)-14;
        fillRect(bx,by,tw+16,textH(S)+6, 0.15f*pulse,0.f,0.25f*pulse,0.9f);
        strokeRect(bx,by,tw+16,textH(S)+6, 0.7f,0.2f,1.f,2.f);
        drawTextGL(bx+8, by+3, bt, 0.9f,0.4f,1.f, S);
    }

    if (g_state.showInfo) {
        int pw=400; int ph=textH(SB)+16+7*LH+12;
        int px=(g_state.winW-pw)/2, py=(g_state.winH-ph)/2;
        fillRect(px-3,py-3,pw+6,ph+6, 0.6f,0.5f,0.1f,0.92f);
        fillRect(px,py,pw,ph, 0.f,0.f,0.08f,0.97f);
        int iy=py+8;
        const char *ttl="=== INFO PANEL ===";
        drawTextGL(px+(pw-textW(ttl,SB))/2, iy, ttl, 1.f,0.85f,0.2f, SB); iy+=textH(SB)+12;
        snprintf(buf,sizeof buf,"Animacio: %s", g_state.paused?"SZUNET":"FUTO");
        drawTextGL(px+14, iy, buf, 0.8f,0.8f,0.8f, S); iy+=LH;
        snprintf(buf,sizeof buf,"Feny: %s  (L)", g_state.lightOn?"BE":"KI");
        drawTextGL(px+14, iy, buf, 0.9f,0.85f,0.5f, S); iy+=LH;
        snprintf(buf,sizeof buf,"Fenyerosseg: %.1fx  (+/-)", g_state.lightIntensity);
        drawTextGL(px+14, iy, buf, 0.9f,0.85f,0.5f, S); iy+=LH;
        snprintf(buf,sizeof buf,"Sebesseg: %.2fx  (J/K)", g_state.animSpeed);
        drawTextGL(px+14, iy, buf, 0.5f,1.f,0.5f, S); iy+=LH;
        snprintf(buf,sizeof buf,"Palya: %s   Nevek: %s", g_state.showOrbits?"BE":"KI", g_state.showNames?"BE":"KI");
        drawTextGL(px+14, iy, buf, 0.8f,0.8f,0.8f, S); iy+=LH;
        snprintf(buf,sizeof buf,"Fekete lyuk: %s", g_state.bhActive?"AKTIV":"KI");
        drawTextGL(px+14, iy, buf, 0.8f,0.5f,1.f, S); iy+=LH;
        const char *cls="[ F2 = bezaras ]";
        drawTextGL(px+(pw-textW(cls,S))/2, iy, cls, 0.5f,0.5f,0.5f, S);
    }

    if (g_state.showHelp) {
        const char *keys[]={
            "Eger bal + huzas","Eger gorgo","W/S  A/D  Q/E",
            "Bal katt bolygon","Bal nyomva + huzas","Jobb klikk tartva",
            "P","L","+ / -","J / K","O","N",
            "Bal / Jobb nyil","Fel / Le nyil","R","1 / 2","F2","ESC",
        };
        const char *vals[]={
            "kamera forgatas","zoom","kamera mozgatas",
            "kivalaszt","bolygot mozgat","fekete lyuk aktiv",
            "szunet / folytat","feny be/ki","fenyerosseg +/-","sebesseg -/+",
            "palyak be/ki","nevek be/ki",
            "bolygo valtas (Nap is)","infokartya elrejt / elohoz",
            "teljes reset","felul / oldal nezet","info panel","kilepes",
        };
        int nl=(int)(sizeof keys/sizeof*keys);
        int lsc=1, lh2=textH(lsc)+5;
        int col1w=21*6*lsc, sep=14, col2w=25*6*lsc;
        int pw=14+col1w+sep+col2w+14;
        int ph=textH(SB)+16+nl*lh2+8;
        if (ph > g_state.winH-20) ph=g_state.winH-20;
        int px=(g_state.winW-pw)/2, py=(g_state.winH-ph)/2;
        fillRect(px-3,py-3,pw+6,ph+6, 0.8f,0.6f,0.1f,0.92f);
        fillRect(px,py,pw,ph, 0.f,0.f,0.08f,0.97f);
        int hy=py+8;
        const char *ttl="=== VEZERLES  (F1=bezar) ===";
        drawTextGL(px+(pw-textW(ttl,SB))/2, hy, ttl, 1.f,0.85f,0.2f, SB); hy+=textH(SB)+12;
        for (int i=0; i<nl && hy+lh2<py+ph-4; i++) {
            float kr=0.85f,kg=0.85f,kb=0.85f,vr=0.85f,vg=0.85f,vb=0.85f;
            if (i==5) {kr=0.9f;kg=0.4f;kb=1.f;  vr=0.9f;vg=0.4f;vb=1.f;}
            if (i==7) {kr=1.f; kg=0.9f;kb=0.3f; vr=1.f; vg=0.9f;vb=0.3f;}
            if (i==8) {kr=1.f; kg=0.9f;kb=0.3f; vr=1.f; vg=0.9f;vb=0.3f;}
            if (i==9) {kr=0.4f;kg=1.f; kb=0.4f; vr=0.4f;vg=1.f; vb=0.4f;}
            if (i==12||i==15){kr=0.4f;kg=0.9f;kb=0.4f;vr=0.4f;vg=0.9f;vb=0.4f;}
            drawTextGL(px+14,       hy, keys[i], kr,kg,kb, lsc);
            drawTextGL(px+14+col1w, hy, ":",     0.5f,0.5f,0.5f, lsc);
            drawTextGL(px+14+col1w+sep, hy, vals[i], vr,vg,vb, lsc);
            hy += lh2;
        }
    }

    endOrtho();
}

/* ════════════════════════════════════════════════════════════
   RESET
   ════════════════════════════════════════════════════════════ */
void resetSim(void)
{
    g_state.camXRot = CAM_XROT0; g_state.camYRot = CAM_YROT0;
    g_state.camTx   = CAM_TX0;   g_state.camTy   = CAM_TY0;   g_state.camTz = CAM_TZ0;
    g_state.paused  = 0;         g_state.lightOn = 1;          g_state.lightIntensity = 1.6f;
    g_state.animSpeed  = 1.0f;
    g_state.showOrbits = 1; g_state.showNames = 1; g_state.showHelp = 0; g_state.showInfo = 0;
    g_state.sunRot     = 0.f;
    g_state.selectedPlanet = -1; g_state.draggedPlanet = -1;
    g_state.bhActive = 0; g_state.bhPulse = 0.f;
    for (int i=0; i<NUM_PLANETS; i++) {
        g_state.planets[i].orbitAngle     = g_state.initOrbitAngle[i];
        g_state.planets[i].rotAngle       = 0.f;
        g_state.planets[i].dragged        = 0;
        g_state.planets[i].returning      = 0; g_state.planets[i].returnProgress = 0.f;
        g_state.planets[i].waitFrames     = 0;
        g_state.planets[i].velX = g_state.planets[i].velY = g_state.planets[i].velZ = 0.f;
        g_state.planets[i].absorbed       = 0; g_state.planets[i].absorbScale = 1.f;
        g_state.planets[i].bhPulled       = 0;
        for (int m=0; m<g_state.moonGroups[i].count; m++)
            g_state.moonGroups[i].moons[m].orbitAngle = g_state.initMoonAngles[i][m];
    }
}

/* ════════════════════════════════════════════════════════════
   OPENGL INIT ES RESIZE
   ════════════════════════════════════════════════════════════ */
void initGL(void)
{
    glClearColor(0.f,0.f,0.04f,1.f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
}

void setProjection(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(60.0, (double)w/h, 0.1, 600.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
}

/* ════════════════════════════════════════════════════════════
   RENDER
   ════════════════════════════════════════════════════════════ */
void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(g_state.camTx, g_state.camTy, g_state.camTz);
    glRotatef(g_state.camXRot, 1.f,0.f,0.f);
    glRotatef(g_state.camYRot, 0.f,1.f,0.f);

    if (g_state.selectedPlanet >= 0) {
        Planet *p = &g_state.planets[g_state.selectedPlanet];
        float px,py,pz;
        if (p->dragged || p->waitFrames>0 || p->returning)
            { px=p->worldX; py=p->worldY; pz=p->worldZ; }
        else
            planetWorldPos(p,&px,&py,&pz);
        glTranslatef(-px,-py,-pz);
    }

    drawStars();
    configLight();
    if (g_state.showOrbits)
        for (int i=0; i<NUM_PLANETS; i++) drawOrbit(g_state.planets[i].orbitRadius, g_state.planets[i].orbitTilt);
    drawSun();
    for (int i=0; i<NUM_PLANETS; i++) drawPlanet(&g_state.planets[i]);
    drawBlackHole();
    drawHUD();
}

/* ════════════════════════════════════════════════════════════
   UPDATE
   ════════════════════════════════════════════════════════════ */
void update(float dt)
{
    if (g_state.introAnim) {
        g_state.introTime += 0.0035f * dt;
        float t = g_state.introTime; if (t>1.f) t=1.f;
        float s = t*t*(3.f-2.f*t);
        g_state.camTz   = -520.f + (CAM_TZ0  + 520.f) * s;
        g_state.camXRot =    5.f + (CAM_XROT0 - 5.f)  * s;
        g_state.camYRot = 18.f  * (1.f-s);
        g_state.camTx   = sinf(t*PI)*1.2f;
        g_state.camTy   = -15.f*(1.f-s);
        if (g_state.introTime >= 1.f) {
            g_state.introAnim = 0;
            g_state.camTz=CAM_TZ0; g_state.camXRot=CAM_XROT0;
            g_state.camYRot=0.f; g_state.camTx=0.f; g_state.camTy=0.f;
        }
    }

    g_state.bhPulse += 0.05f * dt;

    if (!g_state.paused) {
        float sp = g_state.animSpeed * dt;
        g_state.sunRot += 0.08f * sp;

        for (int i=0; i<NUM_PLANETS; i++) {
            if (g_state.planets[i].absorbed) {
                if (g_state.planets[i].absorbScale > 0.f) {
                    g_state.planets[i].absorbScale -= 0.025f * dt;
                    if (g_state.planets[i].absorbScale < 0.f) g_state.planets[i].absorbScale = 0.f;
                }
                continue;
            }
            if (g_state.planets[i].returning && !g_state.planets[i].dragged) {
                g_state.planets[i].returnProgress += 0.007f * dt;
                if (g_state.planets[i].returnProgress >= 1.f) {
                    g_state.planets[i].returnProgress=1.f; g_state.planets[i].returning=0; g_state.planets[i].bhPulled=0;
                }
                g_state.planets[i].orbitAngle += g_state.planets[i].orbitSpeed * sp;
                if (g_state.planets[i].orbitAngle >= 360.f) g_state.planets[i].orbitAngle -= 360.f;
                g_state.planets[i].rotAngle += g_state.planets[i].rotSpeed * sp;
                if (g_state.planets[i].rotAngle >= 360.f) g_state.planets[i].rotAngle -= 360.f;
                continue;
            }
            if (g_state.planets[i].waitFrames > 0 && !g_state.planets[i].dragged) {
                g_state.planets[i].waitFrames -= (int)(dt + 0.5f);
                if (g_state.planets[i].waitFrames < 0) g_state.planets[i].waitFrames = 0;
                if (g_state.planets[i].waitFrames == 0) {
                    g_state.planets[i].returning=1; g_state.planets[i].returnProgress=0.f;
                    g_state.planets[i].returnFromX=g_state.planets[i].worldX;
                    g_state.planets[i].returnFromY=g_state.planets[i].worldY;
                    g_state.planets[i].returnFromZ=g_state.planets[i].worldZ;
                }
                g_state.planets[i].orbitAngle += g_state.planets[i].orbitSpeed * sp;
                if (g_state.planets[i].orbitAngle >= 360.f) g_state.planets[i].orbitAngle -= 360.f;
                g_state.planets[i].rotAngle += g_state.planets[i].rotSpeed * sp;
                if (g_state.planets[i].rotAngle >= 360.f) g_state.planets[i].rotAngle -= 360.f;
                continue;
            }
            if (!g_state.planets[i].dragged) {
                g_state.planets[i].orbitAngle += g_state.planets[i].orbitSpeed * sp;
                if (g_state.planets[i].orbitAngle >= 360.f) g_state.planets[i].orbitAngle -= 360.f;
                g_state.planets[i].rotAngle += g_state.planets[i].rotSpeed * sp;
                if (g_state.planets[i].rotAngle >= 360.f) g_state.planets[i].rotAngle -= 360.f;
            }
            if (g_state.bhActive && g_state.draggedPlanet!=i &&
                !g_state.planets[i].returning && !g_state.planets[i].waitFrames) {
                float wx,wy,wz;
                if (g_state.planets[i].dragged)
                    { wx=g_state.planets[i].worldX; wy=g_state.planets[i].worldY; wz=g_state.planets[i].worldZ; }
                else
                    planetWorldPos(&g_state.planets[i],&wx,&wy,&wz);
                float fx=g_state.bhX-wx, fy=g_state.bhY-wy, fz=g_state.bhZ-wz;
                float dist2=fx*fx+fy*fy+fz*fz; if(dist2<0.1f) dist2=0.1f;
                float dist=sqrtf(dist2);
                if (dist < 1.2f) {
                    g_state.planets[i].absorbed=1; g_state.planets[i].absorbScale=1.f;
                    g_state.planets[i].dragged=0; g_state.planets[i].returning=0;
                    g_state.planets[i].waitFrames=0; g_state.planets[i].bhPulled=0;
                    g_state.planets[i].velX=g_state.planets[i].velY=g_state.planets[i].velZ=0.f;
                    printf("%s elnyelt (R hozza vissza)\n", g_state.planets[i].name);
                    continue;
                }
                float str=12.f/(dist2+1.f);
                if (!g_state.planets[i].dragged) {
                    g_state.planets[i].dragged=1; g_state.planets[i].bhPulled=1;
                    g_state.planets[i].worldX=wx; g_state.planets[i].worldY=wy; g_state.planets[i].worldZ=wz;
                }
                g_state.planets[i].velX += (fx/dist)*str*0.016f * dt;
                g_state.planets[i].velY += (fy/dist)*str*0.016f * dt;
                g_state.planets[i].velZ += (fz/dist)*str*0.016f * dt;
                float drag = 1.f - (1.f - 0.98f) * dt;
                g_state.planets[i].velX *= drag;
                g_state.planets[i].velY *= drag;
                g_state.planets[i].velZ *= drag;
                g_state.planets[i].worldX+=g_state.planets[i].velX;
                g_state.planets[i].worldY+=g_state.planets[i].velY;
                g_state.planets[i].worldZ+=g_state.planets[i].velZ;
            }
        }

        for (int i=0; i<NUM_PLANETS; i++) {
            if (g_state.planets[i].absorbed) continue;
            MoonGroup *mg = &g_state.moonGroups[i];
            for (int m=0; m<mg->count; m++) {
                mg->moons[m].orbitAngle += mg->moons[m].orbitSpeed * sp;
                if (mg->moons[m].orbitAngle >= 360.f) mg->moons[m].orbitAngle -= 360.f;
                if (mg->moons[m].orbitAngle <    0.f) mg->moons[m].orbitAngle += 360.f;
            }
        }
    }
}

/* ════════════════════════════════════════════════════════════
   ESEMENYKEZELES
   ════════════════════════════════════════════════════════════ */
void handleEvent(SDL_Event *e)
{
    if (e->type == SDL_QUIT) { g_state.running=0; return; }

    if (e->type == SDL_WINDOWEVENT &&
        e->window.event == SDL_WINDOWEVENT_RESIZED) {
        g_state.winW = e->window.data1;
        g_state.winH = e->window.data2;
        setProjection(g_state.winW, g_state.winH);
    }

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_ESCAPE: g_state.running=0; break;
            case SDLK_p:  g_state.paused=!g_state.paused; break;
            case SDLK_l:  g_state.lightOn=!g_state.lightOn; break;
            case SDLK_o:  g_state.showOrbits=!g_state.showOrbits; break;
            case SDLK_n:  g_state.showNames=!g_state.showNames; break;
            case SDLK_r:  resetSim(); break;
            case SDLK_F1: g_state.showHelp=!g_state.showHelp; if(g_state.showHelp) g_state.showInfo=0; break;
            case SDLK_F2: g_state.showInfo=!g_state.showInfo; if(g_state.showInfo) g_state.showHelp=0; break;
            case SDLK_PLUS: case SDLK_EQUALS: case SDLK_KP_PLUS:
                g_state.lightIntensity+=0.1f; if(g_state.lightIntensity>3.f) g_state.lightIntensity=3.f; break;
            case SDLK_MINUS: case SDLK_KP_MINUS:
                g_state.lightIntensity-=0.1f; if(g_state.lightIntensity<0.0f) g_state.lightIntensity=0.0f; break;
            case SDLK_j:
                g_state.animSpeed-=ANIM_SPEED_STEP;
                if(g_state.animSpeed<ANIM_SPEED_MIN) g_state.animSpeed=ANIM_SPEED_MIN; break;
            case SDLK_k:
                g_state.animSpeed+=ANIM_SPEED_STEP;
                if(g_state.animSpeed>ANIM_SPEED_MAX) g_state.animSpeed=ANIM_SPEED_MAX; break;
            case SDLK_w: g_state.camTz+=1.f; break;
            case SDLK_s: g_state.camTz-=1.f; break;
            case SDLK_a: g_state.camTx+=1.f; break;
            case SDLK_d: g_state.camTx-=1.f; break;
            case SDLK_q: g_state.camTy-=1.f; break;
            case SDLK_e: g_state.camTy+=1.f; break;
            case SDLK_1: g_state.camXRot=90.f; g_state.camYRot=0.f;
                         g_state.camTx=0.f; g_state.camTy=-5.f; g_state.camTz=-55.f; break;
            case SDLK_2: g_state.camXRot=20.f; g_state.camYRot=0.f;
                         g_state.camTx=0.f; g_state.camTy=0.f;  g_state.camTz=-35.f; break;
            case SDLK_RIGHT:
                g_state.selectedPlanet++;
                if (g_state.selectedPlanet >= NUM_PLANETS) g_state.selectedPlanet=-1;
                g_state.showCard=1; break;
            case SDLK_LEFT:
                g_state.selectedPlanet--;
                if (g_state.selectedPlanet < -1) g_state.selectedPlanet=NUM_PLANETS-1;
                g_state.showCard=1; break;
            case SDLK_UP:   g_state.showCard=1; break;
            case SDLK_DOWN: g_state.showCard=0; break;
        }
    }

    if (e->type==SDL_MOUSEBUTTONDOWN && e->button.button==SDL_BUTTON_RIGHT) {
        g_state.bhActive=1;
        float wx,wy,wz;
        if (mouseToWorld(e->button.x,e->button.y,&wx,&wy,&wz))
            { g_state.bhX=wx; g_state.bhY=wy; g_state.bhZ=wz; }
    }
    if (e->type==SDL_MOUSEBUTTONUP && e->button.button==SDL_BUTTON_RIGHT) {
        g_state.bhActive=0;
        for (int i=0; i<NUM_PLANETS; i++)
            if (!g_state.planets[i].absorbed && g_state.planets[i].bhPulled && g_state.planets[i].dragged) {
                g_state.planets[i].dragged=0; g_state.planets[i].bhPulled=0;
                g_state.planets[i].waitFrames=WAIT_FRAMES;
                g_state.planets[i].velX=g_state.planets[i].velY=g_state.planets[i].velZ=0.f;
            }
    }

    if (e->type==SDL_MOUSEBUTTONDOWN && e->button.button==SDL_BUTTON_LEFT) {
        g_state.mouseDown=1;
        g_state.lastMX=e->button.x; g_state.lastMY=e->button.y;
        g_state.clickMX=e->button.x; g_state.clickMY=e->button.y;
        g_state.didDrag=0;
        g_state.draggedPlanet = pickPlanet(e->button.x, e->button.y);
        if (g_state.draggedPlanet >= 0) {
            Planet *p = &g_state.planets[g_state.draggedPlanet];
            float ox,oy,oz;
            if (!p->dragged && !p->returning && !p->waitFrames)
                planetWorldPos(p,&ox,&oy,&oz);
            else
                { ox=p->worldX; oy=p->worldY; oz=p->worldZ; }
            p->worldX=ox; p->worldY=oy; p->worldZ=oz;
            p->returning=0; p->returnProgress=0.f;
            p->waitFrames=0; p->velX=p->velY=p->velZ=0.f; p->bhPulled=0;
        }
    }
    if (e->type==SDL_MOUSEBUTTONUP && e->button.button==SDL_BUTTON_LEFT) {
        int ddx=e->button.x-g_state.clickMX, ddy=e->button.y-g_state.clickMY;
        if (ddx*ddx+ddy*ddy<25 && !g_state.didDrag) {
            int hit=pickPlanet(e->button.x, e->button.y);
            if (hit>=0) g_state.selectedPlanet=hit;
            else        g_state.selectedPlanet=-1;
        }
        if (g_state.draggedPlanet>=0 && g_state.didDrag) {
            g_state.planets[g_state.draggedPlanet].dragged=0;
            g_state.planets[g_state.draggedPlanet].waitFrames=WAIT_FRAMES;
        }
        g_state.draggedPlanet=-1; g_state.mouseDown=0; g_state.didDrag=0;
    }
    if (e->type==SDL_MOUSEMOTION) {
        int ddx=e->motion.x-g_state.clickMX, ddy=e->motion.y-g_state.clickMY;
        if (ddx*ddx+ddy*ddy>25) g_state.didDrag=1;
        if (g_state.draggedPlanet>=0 && g_state.didDrag) {
            float wx,wy,wz;
            if (mouseToWorld(e->motion.x,e->motion.y,&wx,&wy,&wz)) {
                g_state.planets[g_state.draggedPlanet].dragged=1;
                g_state.planets[g_state.draggedPlanet].worldX=wx;
                g_state.planets[g_state.draggedPlanet].worldY=wy;
                g_state.planets[g_state.draggedPlanet].worldZ=wz;
            }
        } else if (g_state.mouseDown) {
            g_state.camYRot += (e->motion.x-g_state.lastMX)*0.4f;
            g_state.camXRot += (e->motion.y-g_state.lastMY)*0.4f;
        }
        g_state.lastMX=e->motion.x; g_state.lastMY=e->motion.y;
        if (g_state.bhActive) {
            float wx,wy,wz;
            if (mouseToWorld(e->motion.x,e->motion.y,&wx,&wy,&wz))
                { g_state.bhX=wx; g_state.bhY=wy; g_state.bhZ=wz; }
        }
    }
    if (e->type==SDL_MOUSEWHEEL) g_state.camTz += e->wheel.y*1.5f;
}

/* ════════════════════════════════════════════════════════════
   MAIN
   ════════════════════════════════════════════════════════════ */
int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    memset(&g_state, 0, sizeof g_state);
    g_state.camXRot       = CAM_XROT0; g_state.camYRot    = CAM_YROT0;
    g_state.camTx         = CAM_TX0;   g_state.camTy      = CAM_TY0;   g_state.camTz = CAM_TZ0;
    g_state.showOrbits    = 1;         g_state.showNames  = 1;
    g_state.showCard      = 0;
    g_state.lightOn       = 1;         g_state.lightIntensity = 1.6f;
    g_state.animSpeed     = 1.0f;
    g_state.introAnim     = 1;
    g_state.selectedPlanet= -1;        g_state.draggedPlanet  = -1;
    g_state.winW          = WIN_W_DEFAULT;
    g_state.winH          = WIN_H_DEFAULT;
    g_state.running       = 1;

    if (loadPlanetsCSV("assets/planets.csv") != NUM_PLANETS) {
        fprintf(stderr, "Hiba: nem sikerult betolteni a bolygoadatokat!\n");
        return 1;
    }
    loadMoonsCSV("assets/moons.csv");

    for (int i=0; i<NUM_PLANETS; i++) {
        g_state.initOrbitAngle[i] = g_state.planets[i].orbitAngle;
        for (int m=0; m<g_state.moonGroups[i].count; m++)
            g_state.initMoonAngles[i][m] = g_state.moonGroups[i].moons[m].orbitAngle;
    }

    if (SDL_Init(SDL_INIT_VIDEO)<0)
        { fprintf(stderr,"SDL: %s\n",SDL_GetError()); return 1; }
    if (!(IMG_Init(IMG_INIT_PNG|IMG_INIT_JPG)&(IMG_INIT_PNG|IMG_INIT_JPG)))
        { fprintf(stderr,"IMG: %s\n",IMG_GetError()); SDL_Quit(); return 1; }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);

    SDL_Window *win = SDL_CreateWindow("Naprendszer Szimulacio",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        g_state.winW, g_state.winH,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!win) { fprintf(stderr,"Ablak: %s\n",SDL_GetError()); IMG_Quit(); SDL_Quit(); return 1; }

    SDL_GLContext glCtx = SDL_GL_CreateContext(win);
    if (!glCtx) { fprintf(stderr,"GL: %s\n",SDL_GetError()); SDL_DestroyWindow(win); IMG_Quit(); SDL_Quit(); return 1; }
    SDL_GL_SetSwapInterval(1);

    initGL();
    setProjection(g_state.winW, g_state.winH);
    initStars();

    g_state.sunTexID  = loadTexture("assets/img/sol.png");
    g_state.moonTexID = loadTexture("assets/img/hold.png");
    for (int i=0; i<NUM_PLANETS; i++)
        g_state.planets[i].texID = loadTexture(g_state.planets[i].texPath);

    printf("=== Naprendszer Szimulacio v9 ===\n");
    printf("F1=Sugo | F2=Info | P=Szunet | R=Reset | J/K=Sebesseg | ESC=Ki\n");

    Uint32 prev = SDL_GetTicks();
    while (g_state.running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) handleEvent(&ev);

        Uint32 now = SDL_GetTicks();
        float dt = (float)(now - prev) / 16.667f;
        if (dt > 4.f) dt = 4.f;                   
        prev = now;

        update(dt);
        render();
        SDL_GL_SwapWindow(win);
    }

    SDL_GL_DeleteContext(glCtx);
    SDL_DestroyWindow(win);
    IMG_Quit(); SDL_Quit();
    return 0;
}