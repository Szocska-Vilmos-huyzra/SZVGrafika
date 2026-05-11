# Solar System Simulation

Interaktív, valós idejű 3D naprendszer-szimuláció C nyelven, OpenGL és SDL2 könyvtárak felhasználásával.

## Assets letöltés

A textúrákat és egyéb adatfájlokat tartalmazó `assets` mappa az alábbi linken érhető el:

> (https://drive.google.com/drive/folders/1FqLYq_IqA2bSB6YvWfTHwFvc8ILBi6ZZ?usp=drive_link)

A letöltött `assets.zip` tartalmát csomagold ki a projekt gyökérkönyvtárába úgy, hogy az alábbi struktúra jöjjön létre:

```
Beadando/
  assets/
    planets.csv
    moons.csv
    img/
      sol.png
      mercurio.png
      venus.png
      terra.png
      marte.png
      jupiter.png
      saturno.png
      urano.png
      netuno.png
      hold.png
  main.c
  Makefile
```

---

## Fordítás és futtatás

### Linux

```bash
make
./solar
```

### Windows (MinGW)

```bash
gcc main.c -o solar.exe -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lglu32 -lm
solar.exe
```

**Fontos:** a programot mindig abból a könyvtárból futtasd, ahol az `assets/` mappa találhatók.

---

## A program leírása

A projekt egy interaktív 3D grafikai alkalmazás, amely a Naprendszer működését szimulálja. A program megjeleníti a Napot, a 8 bolygót (Merkúr–Neptunusz), azok holdjait, pályagörbéit, valamint számos vizuális és interaktív extra funkciót.

### Megvalósított funkciók

**Minimális követelmények:**

- **Kamerakezelés** – egérrel forgatható, billentyűzettel mozgatható és zoomolható kamera
- **Térbeli objektumok** – textúrázott gömbök (Nap, bolygók, holdak), Szaturnusz gyűrűi
- **Adatbetöltés fájlból** – bolygó- és holdadatok CSV fájlokból töltődnek be (`assets/planets.csv`, `assets/moons.csv`)
- **Animáció** – időfüggő keringés és tengelyforgás, szüneteltethető és sebességben állítható
- **Textúrák** – minden égitestnek saját PNG textúrája van
- **Fénykezelés** – OpenGL fix pipeline pont fényforrás (GL_LIGHT0), `+`/`-` billentyűkkel állítható intenzitással
- **F1 súgó** – billentyűzettel előhívható kezelési útmutató a képernyőn
- **Átméretezhető ablak** – az arányok megtartása mellett átméretezhető (`SDL_WINDOW_RESIZABLE`)

**Extra funkciók:**

- **Objektum kijelölése egérrel** – ray picking algoritmussal (sugár–gömb metszés), kijelölt bolygó infókártyával
- **Átlátszóság** – Szaturnusz gyűrűi, pályagörbék és UI elemek alpha blending-gel
- **Procedurális geometria** – véletlenszerűen generált csillagmező (500 csillag, háromféle méret és szín)
- **Fizika / speciális effekt** – fekete lyuk gravitációs hatás (jobb egérgombbal helyezhető el); bolygók vonzódnak felé, elnyelhetők, majd visszatérnek interpolált mozgással
- **Felhasználói felület** – saját bitmap font renderelővel megvalósított HUD: fejléc, bolygólista, infókártya, súgó, info panel
- **Intro animáció** – indításkor automatikus kamerabeúszás smooth interpolációval

---

## Kezelési útmutató

### Kamera

| Billentyű / Egér | Funkció |
|---|---|
| Egér bal gomb + húzás | Kamera forgatása |
| Egér görgő | Zoom (közelít / távolít) |
| W / S | Előre / hátra |
| A / D | Balra / jobbra |
| Q / E | Le / fel |
| 1 | Felülnézet |
| 2 | Alapnézet visszaállítása |

### Animáció

| Billentyű | Funkció |
|---|---|
| P | Szünet / folytatás |
| J | Animáció lassítása |
| K | Animáció gyorsítása |
| R | Szimuláció visszaállítása (reset) |

### Fények

| Billentyű | Funkció |
|---|---|
| + / = | Fényintenzitás növelése |
| - | Fényintenzitás csökkentése |
| L | Fény ki/be kapcsolása |

### Megjelenítés

| Billentyű | Funkció |
|---|---|
| F1 | Súgó megjelenítése / elrejtése |
| F2 | Részletes bolygóinformációk |
| O | Pályagörbék ki/be |
| N | Bolygónevek lista ki/be |
| ← / → | Bolygók között váltás (infókártya) |
| ↑ / ↓ | Infókártya megjelenítése / elrejtése |

### Egér interakció

| Művelet | Funkció |
|---|---|
| Bal kattintás bolygóra | Bolygó kijelölése (infókártya) |
| Bal gomb + húzás bolygón | Bolygó megfogása és mozgatása |
| Elengedés | Bolygó visszatér pályájára |
| Jobb gomb lenyomva tartás | Fekete lyuk elhelyezése (vonzza a bolygókat) |

---

## Technológiák

| Komponens | Technológia |
|---|---|
| Programozási nyelv | C (C11) |
| Grafikai API | OpenGL 1.x (fix pipeline) + GLU |
| Ablakkezelés, input | SDL2 |
| Textúratöltés | SDL2_image |
| Matematika | math.h |
| Adatformátum | CSV |

---

## Projekt struktúra

```
Beadando/
  main.c          - teljes forráskód (AppState, Planet, Moon, MoonGroup struktúrák)
  Makefile        - fordítási szkript
  assets/
    planets.csv   - bolygóadatok (név, sugár, pálya, textúra, infók)
    moons.csv     - holdadatok (melyik bolygóhoz, pályaparaméterek)
    img/          - textúrafájlok (PNG)
  README.md       - ez a fájl
```
