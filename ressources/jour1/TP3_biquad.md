# TP3 — Filtre biquad

> **Fiche PDF : [`TP3_biquad.pdf`](TP3_biquad.pdf)** (version imprimable, avec les schémas).
> Ce Markdown en reprend le contenu pour une lecture directe sur GitHub.

**Jour 1 · Durée : 60 min · Binômes**

---

## Problématique

Comment implémenter une cellule de filtrage du second ordre (biquad) en temps réel, dont on
balaie la fréquence de coupure en MIDI sans introduire de clic ?

## Objectifs

- Écrire une cellule biquad en forme directe II transposée (DF2T) ;
- calculer les coefficients d'un passe-bas / passe-haut (cookbook RBJ / DAFx) ;
- mettre à jour les coefficients sans clic (mise à jour atomique) ;
- vérifier la réponse en fréquence à l'analyseur de spectre et discuter la quantification.

## Prérequis

- TP1 et TP2 validés (objet `AudioStream` custom) ;
- transformée en *z*, fonction de transfert, filtres RII du second ordre ;
- un PC avec Chrome (ou Edge) pour le pupitre MIDI.

---

## Pilotage en MIDI

Les paramètres ne sont pas réglés par des potentiomètres mais **en MIDI depuis le PC**, par le
même câble USB que l'audio. Régler *Outils → USB Type → « Serial + MIDI + Audio »*, ouvrir
[`pupitre_midi.html`](../../ressources_generales/logiciels/pupitre_midi.html) dans **Chrome**, choisir la sortie
« Teensy MIDI/Audio » et le preset **TP3 — Biquad**. Chaque fader envoie un *Control Change*
(0–127) que la Teensy convertit en valeur physique via
[`midi_map.h`](../../code/jour1/TP2_gain/midi_map.h).

| CC | Paramètre | Plage | Loi |
|---|---|---|---|
| 74 | fréquence de coupure | 40 – 20000 Hz | logarithmique |
| 71 | facteur de qualité *Q* | 0,5 – 10 | linéaire |
| 81 | mode | passe-bas / passe-haut | interrupteur |

Comme précédemment, la partie MIDI n'est pas à étudier : vous devez simplement conserver, telles
qu'elles sont fournies, les méthodes d'accès aux attributs de la classe
`AudioFilterBiquadManual`.

## 1. Éléments théoriques

### 1.1 Fonction de transfert du second ordre

Un filtre numérique du second ordre est décrit par une fonction de transfert de la forme :

```math
H(z) = \frac{Y(z)}{X(z)} = \frac{b_0 + b_1 z^{-1} + b_2 z^{-2}}{1 + a_1 z^{-1} + a_2 z^{-2}}
```

Les coefficients *b*₀, *b*₁, *b*₂ définissent la partie *numérateur* du filtre, tandis que *a*₁ et
*a*₂ définissent sa partie *dénominateur*. En développant puis en revenant dans le domaine
temporel, on obtient l'équation aux différences :

```math
y[n] = b_0 x[n] + b_1 x[n-1] + b_2 x[n-2] - a_1 y[n-1] - a_2 y[n-2]
```

Cette formulation est directement exploitable pour programmer le filtre. Elle nécessite cependant
de mémoriser les deux derniers échantillons d'entrée **et** les deux derniers échantillons de
sortie. Une autre réalisation, la forme directe II transposée, permet d'utiliser seulement deux
variables d'état.

### 1.2 Forme directe II transposée (DF2T)

La DF2T (*Direct Form II Transposed*) réalise exactement la même fonction de transfert, mais avec
une structure différente. Elle utilise deux variables d'état, notées *z*₁ et *z*₂ :

```math
\begin{aligned} y[n] &= b_0 x[n] + z_1[n]\\ z_1[n+1] &= b_1 x[n] - a_1 y[n] + z_2[n]\\ z_2[n+1] &= b_2 x[n] - a_2 y[n] \end{aligned}
```

Les deux variables *z*₁ et *z*₂ constituent la mémoire du filtre. Elles doivent donc être
conservées d'un appel à l'autre de `update()` : elles ne doivent **surtout pas** être
réinitialisées à chaque bloc audio.

Pour chaque nouvel échantillon *x*[*n*], le calcul s'effectue dans l'ordre suivant :

```
x[n] ──▶ y[n] ──▶ z1[n+1] ──▶ z2[n+1]
```

Il est important de mettre à jour les états **uniquement après** avoir calculé *y*[*n*]. Les
nouvelles valeurs de *z*₁ et *z*₂ seront utilisées lors du calcul de l'échantillon suivant.

L'intérêt de cette structure est notamment de limiter le nombre de variables d'état nécessaires
et de fournir une réalisation bien adaptée à l'implémentation temps réel. En code, le traitement
d'un échantillon se traduit directement par :

```cpp
float y = b0 * x + z1;
z1 = b1 * x - a1 * y + z2;
z2 = b2 * x - a2 * y;
```

Les variables *z*₁ et *z*₂ doivent être des **attributs de l'objet** et non des variables locales
à `update()`, car elles représentent l'état interne du filtre entre deux échantillons et entre
deux blocs audio.

### 1.3 Calcul des coefficients : filtres « cookbook » de RBJ

Pour construire un filtre passe-bas ou passe-haut à partir de sa fréquence de coupure et de son
facteur de qualité *Q*, on peut utiliser les équations dites « cookbook » proposées par Robert
Bristow-Johnson (RBJ). On commence par calculer la pulsation numérique :

```math
\omega_0 = 2\pi \frac{f_0}{F_s}, \qquad \alpha = \frac{\sin(\omega_0)}{2Q}
```

où *f*₀ est la fréquence de coupure souhaitée et *F*ₛ la fréquence d'échantillonnage.

**Passe-bas** — coefficients non normalisés :

| | | |
|---|---|---|
| $`b_0' = \dfrac{1-\cos\omega_0}{2}`$ | $`b_1' = 1-\cos\omega_0`$ | $`b_2' = \dfrac{1-\cos\omega_0}{2}`$ |
| $`a_0 = 1+\alpha`$ | $`a_1' = -2\cos\omega_0`$ | $`a_2' = 1-\alpha`$ |

La fonction de transfert obtenue est alors :

```math
H(z) = \frac{b_0' + b_1' z^{-1} + b_2' z^{-2}}{a_0 + a_1' z^{-1} + a_2' z^{-2}}
```

Or la forme utilisée précédemment suppose que le premier coefficient du dénominateur vaut 1. Il
faut donc **normaliser** tous les coefficients par *a*₀ :

| | |
|---|---|
| $`a_0 = 1`$ | $`b_0 = b_0'/a_0`$ |
| $`a_1 = a_1'/a_0`$ | $`b_1 = b_1'/a_0`$ |
| $`a_2 = a_2'/a_0`$ | $`b_2 = b_2'/a_0`$ |

Cette normalisation est importante : les coefficients utilisés dans les équations DF2T doivent
être ceux correspondant à une fonction de transfert dont le premier coefficient du dénominateur
vaut exactement 1.

**Passe-haut** — seuls les coefficients du numérateur changent :

```math
b_0' = \frac{1+\cos\omega_0}{2}, \qquad b_1' = -(1+\cos\omega_0), \qquad b_2' = \frac{1+\cos\omega_0}{2}
```

Les coefficients du dénominateur restent inchangés
($`a_0 = 1+\alpha`$, $`a_1' = -2\cos\omega_0`$, $`a_2' = 1-\alpha`$), puis les cinq coefficients *b*₀,
*b*₁, *b*₂, *a*₁ et *a*₂ sont à nouveau obtenus en les divisant par *a*₀.

> **Quantification.** La librairie de base travaille en **virgule fixe** : l'arrondi des
> coefficients est sensible surtout en **basses fréquences** (pôles proches du cercle unité). Ici
> on calcule en **float** (FPU du Cortex-M7) : plus simple et plus stable pour l'apprentissage.
> On discutera l'impact du fixe.

## 2. Boucle de traitement DF2T

> **Activité 1.** Dans
> [`effect_biquad.cpp`](../../code/jour1/TP3_biquad/effect_biquad.cpp), compléter
> `update()` : appliquer les trois équations DF2T échantillon par échantillon, **saturer**, et
> surtout **conserver l'état** *z*₁ / *z*₂ d'un bloc au suivant (variables membres).

```cpp
void AudioFilterBiquadManual::update(void) {
  audio_block_t *block = receiveWritable(0);
  if (!block) return;

  // Copies locales pour la boucle
  float b0 = b0_, b1 = b1_, b2 = b2_, a1 = a1_, a2 = a2_;
  float z1 = z1_, z2 = z2_;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
     // TODO : récupérer un échantillon du buffer et le caster en float

     // TODO : calculer le nouvel échantillon filtré puis mettre à jour l'état
     // float y = ...
     // z1 = 
     // z2 = 

     // TODO : saturation int16

    block->data[i] = (int16_t)y;
  }

  // Sauvegarder l'état pour le bloc suivant
  z1_ = z1; z2_ = z2;

  transmit(block);
  release(block);
}
```

## 3. Calcul des coefficients et mise à jour atomique

Dans un contexte temps réel, le traitement audio est exécuté périodiquement par interruption.
Lors de la modification des coefficients *aᵢ* / *bᵢ* dans
`AudioFilterBiquadManual::setLowpass()`, il est donc possible que le moteur audio déclenche un
`update()` **pendant** la mise à jour des coefficients.

On pourrait alors avoir une situation dans laquelle certains coefficients appartiennent à
l'ancien filtre tandis que les autres appartiennent au nouveau. Le traitement audio utiliserait
un jeu de coefficients **incohérent**, qui ne correspond à aucun des deux filtres calculés. Dans
le cas d'un filtre RII, cette situation peut provoquer une transitoire importante, voire conduire
à une instabilité selon les valeurs des coefficients.

Il faut donc empêcher le traitement audio d'observer un état intermédiaire des coefficients. On
cherche à rendre leur mise à jour **atomique** : le moteur audio doit observer soit l'ancien jeu
complet, soit le nouveau jeu complet, mais jamais un mélange des deux.

Pour cela, le calcul des coefficients est effectué normalement, **avant** la section critique.
Seule leur copie dans les variables utilisées par le traitement audio est protégée par
`AudioNoInterrupts()` :

```cpp
  // Les coefficients sont calcules avant la section critique
  // ...

  AudioNoInterrupts();
  b0_ = b0;
  b1_ = b1;
  b2_ = b2;
  a1_ = a1;
  a2_ = a2;
  AudioInterrupts();
```

La section critique doit rester **la plus courte possible**. Il serait inutile et pénalisant de
désactiver les interruptions pendant le calcul de *ω*₀, des fonctions trigonométriques ou des
coefficients RBJ : ces opérations sont relativement longues et pourraient perturber le traitement
audio. Seule la copie des coefficients doit être protégée.

> **Activité 2.** Compléter `setLowpass(freqHz, Q)` :
>
> 1. calculer *ω*₀, puis *α* ;
> 2. calculer les coefficients du filtre passe-bas selon les formules de Robert Bristow-Johnson
>    (RBJ) ;
> 3. normaliser les coefficients par *a*₀ afin d'obtenir un dénominateur de la forme
>    1 + *a*₁*z*⁻¹ + *a*₂*z*⁻² ;
> 4. copier les cinq coefficients dans les variables du filtre de façon atomique.
>
> **Pourquoi la copie des coefficients doit-elle être réalisée dans une section atomique ?**

Squelette :

```cpp
void AudioFilterBiquadManual::setLowpass(float freqHz, float Q) {
  // 1) Grandeurs intermédiaires
  float w0 = 2.0f * (float)M_PI * freqHz / FS;
  float cw = cosf(w0);
  float sw = sinf(w0);
  float alpha = sw / (2.0f * Q);

  // TODO 1 : calculer b0, b1, b2, a0, a1, a2

  // TODO 2 : diviser b0,b1,b2,a1,a2 par a0

  // TODO 3 : enregistrer les coefficients dans les attributs de la classe. Pensez à encadrer l'affectation des membres par AudioNoInterrupts()/AudioInterrupts()

}
```

> ⚠️ `AudioNoInterrupts()` doit être utilisé avec parcimonie : la section critique doit être très
> courte. Si les interruptions sont désactivées pendant une durée trop importante, le traitement
> audio peut prendre du retard et des données audio peuvent être perdues.

## 4. Vérification spectrale

> **Activité 3.** Envoyer un bruit ou un sweep en entrée, observer le spectre dans REW (ou
> l'analyseur FFT). Vérifier que la coupure suit le fader et mesurer la pente. Quelle pente
> attend-on pour un biquad du second ordre ?

---

## Pièges fréquents

| Symptôme | Cause probable | Correctif |
|---|---|---|
| Instabilité / explosion | signe de `a1`, `a2` inversé, ou pas de normalisation par `a0` | revoir RBJ + normalisation |
| Clic en bougeant le fader | coefficients mis à jour hors section atomique | encadrer par `AudioNoInterrupts()` |
| Filtre « figé » | `setLowpass` appelé dans `update()` ou état remis à 0 | calcul dans `loop()`, état membre |
| Bruit / souffle en HF | saturation manquante | borner `y` avant `int16` |
| Les faders n'ont aucun effet | `usbMIDI.read()` absent de `loop()` | l'ajouter dans `loop()` |

## Livrables

1. Objet biquad DF2T custom fonctionnel, passe-bas balayable en MIDI ;
2. capture du spectre à 2–3 fréquences de coupure ;
3. une phrase : pourquoi la DF2T, et pourquoi la section atomique de mise à jour ?

## Pour aller plus loin

Ajouter un passe-haut commutable ; chaîner deux biquads (filtre du 4ᵉ ordre, −24 dB/oct) ; passer
en stéréo.
