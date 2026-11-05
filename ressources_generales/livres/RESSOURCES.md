# Ressources & références

## Ouvrages de référence

| Document | Fichier | Utilisé au |
|---|---|---|
| **DAFx — Digital Audio Effects**, U. Zölzer (éd.), Wiley, 2ᵉ éd. 2011 | ouvrage sous droits : à la bibliothèque, non fourni ici | TP3 (filtres), TP4–TP5 (convolution, FFT), TP6 (écho), projet (compresseur, non-linéarités, suréchantillonnage) |
| **RBJ Audio EQ Cookbook**, R. Bristow-Johnson | [`Audio-EQ-Cookbook.txt`](Audio-EQ-Cookbook.txt) | TP3 — coefficients du biquad |

Repères de lecture :

- **TP3 — Biquad** : cookbook RBJ, section *Low-pass* / *High-pass* (les formules de `alpha`,
  `b0..b2`, `a0..a2` reprises dans la fiche) ; DAFx, chapitre *Filters* pour la forme directe II
  transposée et la discussion virgule fixe / flottante.
- **TP4 — Convolution directe** et **TP5 — Convolution FFT** : DAFx, chapitre *Filters*
  (filtres RIF, convolution) et chapitre *Time-frequency processing* (TFD, FFT, convolution
  rapide par blocs).
- **TP6 — Écho** : DAFx, chapitre *Delay-based audio effects* (ligne à retard, feedback).
- **Projet — compresseur, limiteur, suréchantillonnage** : DAFx, chapitre *Nonlinear processing*
  (dynamique : compresseur, limiteur, détecteur d'enveloppe ; distorsion, aliasing, suréchantillonnage).

> ⚠️ Le PDF de DAFx est un ouvrage sous droits. Il est ici à usage pédagogique interne —
> **à ne pas publier** sur un dépôt public (voir la note en fin de fichier).

## Vous venez de Java ? Le pont vers C++

Si vous avez fait de la POO en Java, **vous avez déjà les notions** : classe, attribut, méthode,
constructeur, héritage, redéfinition. Ce qui change, c'est la syntaxe — et trois ou quatre choses
que Java vous cachait. Cette section suffit pour la plupart d'entre vous ; les cours
OpenClassrooms de la section suivante ne servent qu'en cas de doute sur un point précis.

### La même classe dans les deux langages

C'est exactement l'objet du TP2, écrit côte à côte.

**En Java** — un seul fichier, tout ensemble :

```java
public class AudioEffectGain extends AudioStream {
    private volatile float gain = 1.0f;

    public AudioEffectGain() {
        super(1);
    }

    public void setGain(float g) { this.gain = g; }

    @Override
    public void update() { /* ... */ }
}
```

**En C++** — la déclaration dans le `.h` :

```cpp
class AudioEffectGain : public AudioStream {
public:
  AudioEffectGain() : AudioStream(1, inputQueueArray) {}

  void setGain(float g) { gain_ = g; }

  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];
  volatile float gain_ = 1.0f;
};      // <-- le point-virgule est obligatoire
```

… et l'implémentation dans le `.cpp` :

```cpp
#include "effect_gain.h"

void AudioEffectGain::update(void) {   // Classe::methode
  // ...
}
```

### Table de correspondance

| Java | C++ | Remarque |
|---|---|---|
| `extends AudioStream` | `: public AudioStream` | le `public` n'est pas optionnel : sans lui, l'héritage est privé et la bibliothèque ne voit plus votre objet |
| `super(1);` en 1ʳᵉ ligne du constructeur | `: AudioStream(1, tab)` **avant** l'accolade | c'est la « liste d'initialisation », elle précède le corps |
| `private int x;` sur chaque membre | `private:` puis les membres | c'est une **section** : elle s'applique jusqu'au label suivant. Dans une `class`, tout est privé par défaut — d'où le `public:` en tête |
| `@Override public void update()` | `virtual void update(void);` | en Java tout est virtuel d'office ; en C++ il faut l'écrire, sinon la bibliothèque appellera la méthode de la classe mère |
| un fichier par classe | `.h` (déclaration) + `.cpp` (implémentation) | `#include "effect_gain.h"` remplace `import` |
| `new AudioEffectGain()` | `AudioEffectGain gain;` | voir ci-dessous, c'est le vrai changement |
| `null` | `nullptr` | |
| `obj.methode()` sur une référence | `ptr->methode()` sur un pointeur | `.` pour un objet, `->` pour un pointeur |
| `final` | `const` | |
| `static final int N = 128;` | `static const int N = 128;` | |
| `int[] t = new int[128];` puis `t.length` | `int16_t t[128];` | taille figée à la compilation, **pas** de `.length`, **pas** de vérification des bornes |
| `System.out.println(x)` | `Serial.println(x)` | |
| `public static void main` | `setup()` + `loop()` | le `main()` est fourni par Arduino |

### Les quatre pièges qui coûtent le plus de temps

**1. Un objet C++ n'est pas une référence.** `AudioEffectGain gain;` ne déclare pas une variable
vide en attente d'un `new` : cette ligne **crée réellement l'objet**, à cet endroit. Dans les
`.ino`, les objets sont déclarés en variables globales, donc construits avant même `setup()`.
Corollaire : `gain` ne peut jamais valoir `nullptr`, et il n'y a rien à libérer.

**2. Il n'y a pas de ramasse-miettes.** En Java, un objet devenu inutile disparaît tout seul. Ici
c'est vous qui rendez les blocs audio au pool : `release(block)` est exactement le travail que le
GC ferait à votre place. L'oublier sur **un seul** chemin d'exécution (y compris un `return`
anticipé) et le son se fige au bout de quelques secondes. C'est le bug numéro un de ces TP.

**3. `volatile` ne veut PAS dire la même chose qu'en Java.** En Java, `volatile` est une garantie
de synchronisation : visibilité entre threads et ordre des accès. En C++ embarqué, il dit
seulement au compilateur « ne garde pas cette variable dans un registre, relis-la en mémoire ».
**Ce n'est pas un verrou.** Il suffit pour un `float` ou un `bool` isolé (TP2), mais pas pour
mettre à jour cinq coefficients de façon cohérente — d'où `AudioNoInterrupts()` au TP3.

**4. `int` n'a pas de taille garantie.** Java garantit `int` = 32 bits partout. Pas C++ : la
taille dépend de la cible. D'où l'usage systématique de `int16_t`, `int32_t` (types à taille
explicite) dès qu'elle compte — et c'est tout le temps ici, puisque les échantillons sont des
`int16_t`. Notez aussi que C++ a des types **non signés** (`uint8_t` pour les valeurs MIDI
0–127), ce que Java n'a pas.

> **Détail de syntaxe qui pique :** écrivez `0.5f` et non `0.5`. Sans le suffixe `f`, la constante
> est un `double` et force tout le calcul en double précision — que le processeur du Teensy 4 sait
> faire, mais nettement plus lentement qu'en simple précision. Sur un traitement échantillon par
> échantillon, ça se voit dans `AudioProcessorUsage()`.

### Ce qui n'existe pas ici

Pas d'`interface` (on utiliserait une classe abstraite), pas de `toString()`, pas de `String`, pas
de packages. Et surtout : **pas de `new`, pas d'exceptions, pas de conteneurs** dans le code temps
réel — voir la fin de la section suivante.

## Remise à niveau C++ (OpenClassrooms, ex Site du Zéro)

Si vous venez de Java, la section précédente vous suffira probablement. Sinon, deux cours gratuits
couvrent tout le C++ utilisé dans ces TP :

- **[Apprenez à programmer en C++](https://openclassrooms.com/fr/courses/1894236-apprenez-a-programmer-en-c)**
  — les bases (noté **[C++]** ci-dessous) ;
- **[Programmez en orienté objet avec C++](https://openclassrooms.com/fr/courses/7137751-programmez-en-oriente-objet-avec-c)**
  — la POO (noté **[POO]**).

Inutile de les faire en entier : voici les chapitres réellement mobilisés, TP par TP.

| TP | Ce que vous écrivez | Chapitres à revoir |
|---|---|---|
| **TP1** | déclarations d'objets, appels de méthodes (`sgtl.enable()`), `setup()` / `loop()` | [C++] P1 *Déclarez une variable*, P2 *Structurez votre code avec les conditions*, P3 *Découpez votre programme en fonctions* |
| **TP2** | votre première **classe**, héritage de `AudioStream`, méthode `virtual`, pointeur `audio_block_t*`, test de pointeur nul, `for` sur un tableau, cast `float` ↔ `int16_t` | [C++] P3 *Manipulez des tableaux statiques*, *Déclarez les pointeurs*, *Utilisez plusieurs fichiers* — [POO] P1 *Créez une classe*, *Gérez l'accès et l'encapsulation*, *Initialisez un objet* ; P2 *Découvrez la notion d'héritage*, *Découvrez la notion de polymorphisme* |
| **TP3** | attributs d'état persistants, séparation `.h` / `.cpp`, syntaxe `Classe::methode()` | [C++] P3 *Utilisez plusieurs fichiers* — [POO] P1 *Créez une classe* |
| **TP4** | double boucle sur des tableaux, buffer circulaire (index qui boucle, masque binaire `&`) | [C++] P3 *Manipulez des tableaux statiques*, *Déclarez les pointeurs* |
| **TP5** | nombres complexes `std::complex<float>`, calcul lourd dans `setup()` (hors `update()`) | [C++] P3 *Manipulez des tableaux statiques*, *Déclarez les pointeurs* |
| **TP6** | tableau statique (buffer circulaire), arithmétique modulo, mémoire d'un état entre deux appels | [C++] P3 *Découpez votre programme en fonctions*, *Manipulez des tableaux statiques* |
| **Projet** | conditions, fonctions mathématiques (`expf`, `log10f`, `powf`), boucle sur un petit tableau | [C++] P2 *Structurez votre code avec les conditions* ; P3 *Découpez votre programme en fonctions* |

**Les trois notions vraiment indispensables**, si vous ne devez en revoir que trois :

1. **La classe** — attributs privés / méthodes publiques, constructeur, liste d'initialisation
   (`AudioEffectGain() : AudioStream(1, inputQueueArray) {}`).
2. **L'héritage et les méthodes virtuelles** — pourquoi écrire `virtual void update(void)` suffit
   à ce que la bibliothèque Audio appelle *votre* code.
3. **Les pointeurs** — `audio_block_t *block`, l'accès par `block->data[i]`, et le réflexe
   `if (!block) return;`.

**Ce qui n'est volontairement PAS utilisé ici**, et que vous pouvez sauter : la STL (`std::vector`,
conteneurs, itérateurs, algorithmes), les templates, les exceptions, et surtout `new` / `delete` —
l'allocation dynamique est proscrite dans `update()`, tout est dimensionné à la compilation. Les
parties 3 et 4 du cours POO ne servent donc pas pour ces TP.

> Le mot-clé **`volatile`**, central au TP2, n'est traité dans aucun des deux cours : il relève de
> la programmation concurrente. L'explication complète est dans la fiche du TP2.

## Documentation Teensy / Audio Library

- **Audio Library — accueil & liste des objets** : <https://www.pjrc.com/teensy/td_libs_Audio.html>
- **Design Tool (graphe de flux en ligne)** : <https://www.pjrc.com/teensy/gui/>
- **AudioProcessorUsage / AudioMemoryUsage** : <https://www.pjrc.com/teensy/td_libs_AudioProcessorUsage.html>
- **Teensy 4.0** (specs, brochage) : <https://www.pjrc.com/store/teensy40.html>
- **Audio Shield Rev D** : <https://www.pjrc.com/store/teensy3_audio.html>
- **Codec SGTL5000** (contrôles logiciels) : <https://www.pjrc.com/teensy/td_libs_AudioControlSGTL5000.html>
- **Écrire son propre objet** (forum PJRC, fil de référence) :
  <https://forum.pjrc.com/threads/28639-Writing-your-own-Audio-classes>
- **Code source de la lib** (modèles d'objets) :
  <https://github.com/PaulStoffregen/Audio> — voir `effect_multiply.cpp`, `filter_biquad.cpp`.

## Traitement du signal — filtres

- **DAFx — Digital Audio Effects**, U. Zölzer (éd.), Wiley — chapitre *Filters*.
- **RBJ Audio EQ Cookbook** (coefficients biquad passe-bas/haut/bande, etc.) :
  <https://www.w3.org/TR/audio-eq-cookbook/> (miroir officiel du cookbook de R. Bristow-Johnson),
  ou la [copie locale](Audio-EQ-Cookbook.txt).
- **Transposed Direct Form II** : rappel dans DAFx et dans la doc `filter_biquad` de la lib.

## Formats de blocs & temps réel

- `AUDIO_BLOCK_SAMPLES = 128` échantillons, `int16_t` par échantillon.
- À `F_s = 44,1 kHz` : un bloc = `128 / 44100 ≈ 2,90 ms`.
- Budget de calcul par bloc = durée d'un bloc ; `AudioProcessorUsage()` en donne le %.

## Aide-mémoire API (objets custom)

```cpp
class MonEffet : public AudioStream {
public:
  MonEffet() : AudioStream(1, inputQueueArray) {}   // 1 entrée
  virtual void update(void);
private:
  audio_block_t *inputQueueArray[1];
};
```

- `receiveReadOnly(n)` : bloc en lecture seule (partagé, ne pas écrire dedans).
- `receiveWritable(n)` : bloc modifiable (copie si nécessaire).
- `transmit(block, canal)` : émet vers la sortie `canal` (0 par défaut).
- `release(block)` : **obligatoire** pour rendre le bloc au pool mémoire.

## Pièges classiques (à afficher en salle)

1. `AudioMemory(n)` trop petit → blocs `NULL`, coupures. Surveiller `AudioMemoryUsageMax()`.
2. Oubli de `release()` → fuite mémoire, le son se fige après quelques secondes.
3. Écriture dans un bloc obtenu par `receiveReadOnly()` → corruption / effets fantômes.
4. Dépassement `int16` sans saturation → clics/craquements. **Saturer** avant réécriture.
5. `analogRead()`/`Serial.print()` **dans `update()`** → hors budget temps réel.

