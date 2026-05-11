#ifndef SOLAR_H
#define SOLAR_H

/*
 * solar.h -- Naprendszer Szimulacio
 * Tipusdefiniciok, konstansok es fuggvenydeklaraciok.
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <math.h>

/* ════════════════════════════════════════════════════════════
   KONSTANSOK
   ════════════════════════════════════════════════════════════ */

#define PI              3.14159265358979323846f
#define DEG2RAD(x)      ((x) * PI / 180.f)

/** Gomb felbontasa (szelet- es gyuruszam). */
#define QUALITY         48

/** Bolygok szama (Merkur..Neptun). */
#define NUM_PLANETS     8

/** Hatterben levo csillagok szama. */
#define NUM_STARS       500

/** Egy bolygohoz tartozo maximalis holdak szama. */
#define MAX_MOONS       8

/** Hanyi framen var a bolygo mielott visszater a palyara. */
#define WAIT_FRAMES     150

/** Ablak alapertelmezett szelessege pixelben. */
#define WIN_W_DEFAULT   1100

/** Ablak alapertelmezett magassaga pixelben. */
#define WIN_H_DEFAULT   650

/** Kamera kezdeti X-tengelyu forgasszoge (fokban). */
#define CAM_XROT0  20.f

/** Kamera kezdeti Y-tengelyu forgasszoge (fokban). */
#define CAM_YROT0   0.f

/** Kamera kezdeti X eltolasa. */
#define CAM_TX0     0.f

/** Kamera kezdeti Y eltolasa. */
#define CAM_TY0     0.f

/** Kamera kezdeti Z eltolasa (tavolsag). */
#define CAM_TZ0   -35.f

/** Animacios sebesseg minimuma. */
#define ANIM_SPEED_MIN   0.1f

/** Animacios sebesseg maximuma. */
#define ANIM_SPEED_MAX  20.0f

/** Animacios sebesseg lepeskoje. */
#define ANIM_SPEED_STEP  0.1f

/* ════════════════════════════════════════════════════════════
   ADATSTRUKTURAK
   ════════════════════════════════════════════════════════════ */

/**
 * Egy hatterben levo csillag adatai.
 * @field x,y,z   Vilagpozicio.
 * @field r,g,b   Szin (0..1).
 * @field size    Pontmeret (1.0, 1.5 vagy 2.0).
 */
typedef struct {
    float x, y, z;
    float r, g, b;
    float size;
} Star;

/**
 * Egy bolygo osszes adata es futasidei allapota.
 * @field name         Nev (pl. "Fold").
 * @field radius       Vizualis sugar (OpenGL egysegben).
 * @field orbitRadius  Palyasugara a Naptol.
 * @field orbitSpeed   Keringesi szogsebesség (fok/frame).
 * @field rotSpeed     Sajat tengely koruli forgassebesség.
 * @field orbitAngle   Jelenlegi szoghelyzet a palyan (fok).
 * @field rotAngle     Sajat forgasszog (fok).
 * @field tilt         Tengelydoles szoge (fok).
 * @field orbitTilt    Palya hajlasszoge (fok).
 * @field texPath      Texturafajl eleresi utja.
 * @field texID        OpenGL textura azonosito.
 * @field r,g,b        Anyagszin diffuz komponense.
 * @field dragged      1 ha az eger huzza.
 * @field worldX/Y/Z   Huzcas / fizika alatti vilagpozicio.
 * @field velX/Y/Z     Sebessegvektor (fekete lyuk fizika).
 * @field returning    1 ha visszater a palyara.
 * @field returnProgress Visszateres interpolacios paramétere (0..1).
 * @field returnFromX/Y/Z Visszateres kiindulo pozicioja.
 * @field waitFrames   Hatralevo varakozasi framek szama.
 * @field absorbed     1 ha a fekete lyuk elnyelte.
 * @field absorbScale  Elnyeleskor zsugorodas merteke (1..0).
 * @field bhPulled     1 ha a fekete lyuk vonzza.
 */
typedef struct {
    char   name[32];
    float  radius, orbitRadius, orbitSpeed, rotSpeed;
    float  orbitAngle, rotAngle, tilt, orbitTilt;
    char   texPath[64];
    GLuint texID;
    float  r, g, b;
    int    dragged;
    float  worldX, worldY, worldZ;
    float  velX, velY, velZ;
    int    returning;
    float  returnProgress;
    float  returnFromX, returnFromY, returnFromZ;
    int    waitFrames;
    int    absorbed;
    float  absorbScale;
    int    bhPulled;
} Planet;

/**
 * Egy hold keringesi adatai.
 * @field name        Nev (pl. "Hold", "Titan").
 * @field orbitRadius Palyasugár a bolygotol.
 * @field orbitSpeed  Keringesi szogsebesség.
 * @field orbitAngle  Jelenlegi szoghelyzet (fok).
 * @field orbitTilt   Palya hajlasszoge (fok).
 * @field radius      Vizualis sugar.
 */
typedef struct {
    char  name[32];
    float orbitRadius;
    float orbitSpeed;
    float orbitAngle;
    float orbitTilt;
    float radius;
} Moon;

/**
 * Egy bolygohoz tartozo holdak csoportja.
 * @field count  Holdak szama (0..MAX_MOONS).
 * @field moons  Hold tomb.
 */
typedef struct {
    int  count;
    Moon moons[MAX_MOONS];
} MoonGroup;

/**
 * Egy egitest enciklopedikus adatai (infokartyan jelenik meg).
 * @field name         Nev.
 * @field type         Tipus leirasa (pl. "Ko bolygo").
 * @field diameter_km  Atmerő kilometerben.
 * @field mass_1024kg  Tomeg 10^24 kg egysegben.
 * @field orbit_days   Keringesi ido napban.
 * @field dist_mkm     Tavolsag a Naptol millo km-ben.
 * @field moons        Holdak szama.
 * @field temp_c       Atlaghomerseklet Celsius-ban.
 * @field extra        Rovid erdekesseg szoveg.
 */
typedef struct {
    char  name[32];
    char  type[48];
    float diameter_km;
    float mass_1024kg;
    int   orbit_days;
    float dist_mkm;
    int   moons;
    float temp_c;
    char  extra[128];
} PlanetInfo;

/**
 * Az alkalmazas teljes allapota egyetlen strukturaban.
 * Ez az egyetlen globalis valtozo (g_state) tipusa.
 */
typedef struct {
    /* --- kamera --- */
    float camXRot, camYRot;        /**< Forgasszogek (fok). */
    float camTx, camTy, camTz;     /**< Eltolasvektor. */

    /* --- eger allapot --- */
    int   mouseDown;               /**< 1 ha bal gomb lenyomva. */
    int   lastMX, lastMY;          /**< Előző egerpozcio. */
    int   clickMX, clickMY;        /**< Kattintas pozicioja. */
    int   didDrag;                 /**< 1 ha volt huzas kattintas ota. */
    int   draggedPlanet;           /**< Huzott bolygo indexe, -1 ha nincs. */

    /* --- megjelenites --- */
    int   showOrbits;              /**< Palyagorbek lathatosaga. */
    int   showNames;               /**< Bolygonev lista lathatosaga. */
    int   showHelp;                /**< F1 sugo panel lathatosaga. */
    int   showInfo;                /**< F2 info panel lathatosaga. */
    int   showCard;                /**< Infokartya lathatosaga. */

    /* --- vilagitas --- */
    int   lightOn;                 /**< Fenyforrás be/ki. */
    float lightIntensity;          /**< Fenyerosseg (0.1..3.0). */

    /* --- animacio --- */
    int   paused;                  /**< 1 ha az animacio szunetel. */
    float animSpeed;               /**< Animacios sebesseg szorzoja. */

    /* --- fekete lyuk --- */
    int   bhActive;                /**< 1 ha aktiv a fekete lyuk. */
    float bhX, bhY, bhZ;          /**< Fekete lyuk vilagpozicioja. */
    float bhPulse;                 /**< Pulzacios animacios parameter. */

    /* --- intro --- */
    int   introAnim;               /**< 1 ha fut a nyito kameraanimacio. */
    float introTime;               /**< Intro elorehaladasa (0..1). */

    /* --- kivalasztas --- */
    int   selectedPlanet;          /**< Kivalasztott bolygo indexe, -1 ha Nap/semmi. */

    /* --- nap --- */
    float sunRot;                  /**< Nap forgasszoge (fok). */

    /* --- texturak --- */
    GLuint sunTexID;               /**< Nap textura azonosito. */
    GLuint moonTexID;              /**< Hold textura azonosito. */

    /* --- ablak --- */
    int   winW, winH;              /**< Ablak aktualis merete pixelben. */

    /* --- futasjelzo --- */
    int   running;                 /**< 0 eseten a folyok leallt. */

    /* --- vilagegyetem adatai --- */
    Planet     planets[NUM_PLANETS];           /**< Bolygok tombje. */
    MoonGroup  moonGroups[NUM_PLANETS];        /**< Holdcsoportok tombje. */
    PlanetInfo planetInfos[NUM_PLANETS + 1];  /**< Infokartya adatok (+1 a Napnak). */
    int        numPlanetInfos;                 /**< Feltoltott infok szama. */
    float      initOrbitAngle[NUM_PLANETS];    /**< Kezdeti palyaszogek (resethez). */
    float      initMoonAngles[NUM_PLANETS][MAX_MOONS]; /**< Kezdeti holdszogek (resethez). */
    Star       stars[NUM_STARS];               /**< Csillagmezo adatai. */
} AppState;

/* ════════════════════════════════════════════════════════════
   FUGGVENYDEKLARACIOK
   ════════════════════════════════════════════════════════════ */

/* --- CSV betoltes --- */

/**
 * Betolti a bolygoadatokat a megadott CSV fajlbol.
 * @param path  A fajl eleresi utja.
 * @return      A sikeresen beolvasott bolygok szama, vagy -1 hiba eseten.
 */
int loadPlanetsCSV(const char *path);

/**
 * Betolti a holdadatokat a megadott CSV fajlbol.
 * @param path  A fajl eleresi utja.
 * @return      A sikeresen beolvasott holdak szama, vagy -1 hiba eseten.
 */
int loadMoonsCSV(const char *path);

/* --- OpenGL init --- */

/** Inicializalja az OpenGL allapotot (melysegi teszt, normalizalas stb.). */
void initGL(void);

/**
 * Beallitja a perspektiv projekciot az ablak meretei alapjan.
 * @param w  Ablak szelessege pixelben.
 * @param h  Ablak magassaga pixelben.
 */
void setProjection(int w, int h);

/* --- Textura --- */

/**
 * Betolt egy PNG/JPG kepet es feltolti OpenGL texturakent.
 * @param path  A kepfajl eleresi utja.
 * @return      OpenGL textura azonosito, vagy 0 hiba eseten.
 */
GLuint loadTexture(const char *path);

/* --- Csillagmezo --- */

/** Veletlen pozicioju es szinu csillagokat general a stars[] tombbe. */
void initStars(void);

/** Kirajzolja a csillagmezot. */
void drawStars(void);

/* --- Rajzolo fuggvenyek --- */

/**
 * Textúrázott gömböt rajzol.
 * @param radius  Sugar.
 * @param texID   OpenGL textura azonosito (0 = nincs textura).
 * @param sl      Felbontas (szelet- es gyuruszam).
 */
void drawTexturedSphere(float radius, GLuint texID, int sl);

/**
 * Keringesi palyat rajzol ellipszisszeru vonalkerettel.
 * @param r     Palyasugár.
 * @param tilt  Palya hajlasszoge fokban.
 */
void drawOrbit(float r, float tilt);

/** Kiszamolja egy bolygo vilagpoziciojat a jelenlegi palyaszoge alapjan. */
void planetWorldPos(Planet *p, float *ox, float *oy, float *oz);

/** Kirajzolja a Napot. */
void drawSun(void);

/**
 * Kirajzol egy bolygot holdjaival es esetleges gyuruivel egyutt.
 * @param p  A kirajzolandó bolygo mutatoja.
 */
void drawPlanet(Planet *p);

/** Kirajzolja a fekete lyuk effektet (ha aktiv). */
void drawBlackHole(void);

/* --- Feny --- */

/** Beallitja az OpenGL fenyforrast a g_state aktualis ertekei alapjan. */
void configLight(void);

/* --- HUD --- */

/** Kirajzolja a teljes 2D kepernyofelulet-retget (HUD). */
void drawHUD(void);

/* --- Kivalasztas --- */

/**
 * Ray picking: megkeresi melyik bolygot kattintottak meg.
 * @param mx,my  Egerkoordinatak kepernyon.
 * @return       Kivalasztott bolygo indexe (0..NUM_PLANETS-1), vagy -1.
 */
int pickPlanet(int mx, int my);

/* --- Foesemenyek --- */

/** Visszaallitja a szimulaciot a kezdeti allapotra. */
void resetSim(void);

/**
 * Frissiti az animaciot es a fizikai allapotokat.
 * @param dt  Delta time: 1.0 = egy 60fps-es frame ideje (~16.67ms).
 *            Erteke aranyos az eltelt idovel, igy az animacio
 *            idofuggetlen es frame-rate-tol independent lesz.
 */
void update(float dt);

/** Kirajzolja a teljes jelenetet. */
void render(void);

/**
 * Feldolgoz egy SDL esemenyt.
 * @param e  A feldolgozando esemeny mutatoja.
 */
void handleEvent(SDL_Event *e);

#endif /* SOLAR_H */