Projekt címe: Solar System (Naprendszer szimuláció)
Projekt célja

A projekt célja egy interaktív, valós idejű 3D grafikus alkalmazás fejlesztése, amely a Naprendszer működésének vizuális és dinamikus szemléltetésére szolgál.
A megvalósítás során a hangsúly a grafikus megjelenítésen, az animációkon, valamint a felhasználói interakción van.

A program C nyelven készül, az OpenGL (fix pipeline) grafikus API és az SDL2 könyvtárak felhasználásával.

Felhasznált technológiák
Programozási nyelv: C
Grafikai API: OpenGL 1.x + GLU
Ablakkezelés és input: SDL2
Textúratöltés: SDL2_image
Matematikai műveletek: math.h

A program klasszikus OpenGL (immediate mode) technikákat alkalmaz, például:

glBegin/glEnd
fix pipeline világítás
GLU quadric objektumok (gömbök)
A rendszer fő komponensei
1. Nap (központi objektum és fényforrás)
A Nap a jelenet origójában helyezkedik el
Textúrázott gömbként jelenik meg
Fényforrásként működik (GL_LIGHT0)
Meghatározza a teljes jelenet megvilágítását:
ambient
diffuse
specular komponensek


2. Bolygók megjelenítése

A rendszer 8 bolygót kezel (Merkúr–Neptunusz).

Minden bolygóhoz tartozik:

sugár (méret)
keringési sugár
keringési sebesség
saját tengely körüli forgási sebesség
tengelydőlés
pályadőlés
textúra

A bolygók:

GLU gömbként kerülnek kirajzolásra
textúrázva vannak (PNG képekből)
anyagtulajdonságokkal rendelkeznek (OpenGL material)
3. Holdrendszer (kiterjesztett funkcionalitás)

A projekt egyik erősebb része, hogy több bolygóhoz holdak is tartoznak:

Föld → Hold
Mars → Phobos, Deimos
Jupiter → több hold
Szaturnusz, Uránusz, Neptunusz → több hold

A holdak:

a bolygó körül keringenek
saját orbit paraméterekkel rendelkeznek
hierarchikus transzformációval vannak megvalósítva
4. Animációs rendszer

A mozgások időfüggőek:

bolygók keringése (orbitAngle növelése)
saját tengely körüli forgás (rotAngle)
holdak mozgása

Az animáció:

megszakítható (pause)
sebessége állítható (animSpeed)
5. Kamera rendszer

Szabadon mozgatható kamera:

forgatás (egérrel)
zoom (Z tengely mentén)
eltolás

Paraméterek:

camXRot, camYRot
camTx, camTy, camTz

A kamera lehetővé teszi a jelenet teljes bejárását.

6. Világítás (Lighting)
A Nap mint pontszerű fényforrás
OpenGL fix pipeline világítás
Paraméterek:
ambient
diffuse
specular
attenuation

Hatás:

bolygók egyik oldala megvilágított
másik oldala árnyékos
7. Textúrázás
PNG/JPG képek betöltése (SDL_image)
OpenGL textúrák generálása
gömbök UV leképezése

Minden bolygó külön textúrát használ.

8. Felhasználói interakció

Billentyűzettel vezérelhető funkciók:

animáció ki/be
fény ki/be
fény erősség állítása
nézetek váltása

Egér:

kamera mozgatás
objektum kiválasztás (ray picking)
9. Objektum kiválasztás (Ray Picking)

A program implementál:

sugár–gömb metszést
egér pozíció → 3D sugár
kiválasztott bolygó meghatározása

Ez lehetővé teszi:

bolygók kijelölését
interaktív manipulációt (pl. húzás)
10. HUD (Head-Up Display)

2D overlay rendszer:

OpenGL ortografikus vetítéssel
saját bitmap font renderelő

Megjelenített elemek:

program címe
súgó (F1)
információk (F2)
kiválasztott bolygó neve
11. Kiegészítő vizuális elemek
pályagörbék kirajzolása
Szaturnusz gyűrűi
csillag háttér
fekete lyuk effekt (opcionális, látványos extra)
12. Speciális funkciók (kiemelkedő részek)

A projekt több extra megoldást is tartalmaz:

bolygók húzása egérrel
visszatérés az eredeti pályára (interpolációval)
fekete lyuk hatás (objektum elnyelés)
animált vizuális effektek
Program működése röviden

A program inicializálás után:

betölti a textúrákat
inicializálja a bolygókat és holdakat
beállítja a kamerát és világítást
folyamatos render ciklusban:
frissíti az animációt
kezeli a bemenetet
kirajzolja a jelenetet
megjeleníti a HUD-ot
Összegzés

A projekt egy komplex, interaktív 3D grafikai alkalmazás, amely:

modellezi a Naprendszer alapvető működését
valós idejű animációkat használ
fejlett grafikai technikákat alkalmaz (textúrázás, világítás)
felhasználói interakciót biztosít

A megvalósítás jól demonstrálja:

az OpenGL alapjait
3D transzformációkat
hierarchikus modellezést
és interaktív grafikai rendszerek működését