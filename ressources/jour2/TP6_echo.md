# TP6 — Écho sur ligne à retard

> **Fiche PDF : [`TP6_echo.pdf`](TP6_echo.pdf)** (version imprimable, avec les schémas).
> Ce Markdown en reprend le contenu pour une lecture directe sur GitHub.

**Jour 2 · Durée : 50 min · Travail individuel**

---

## Problématique

L’écho d’une paroi se répète : sa réponse impulsionnelle est *infinie*. Par convolution (TP4, TP5), il faudrait des dizaines de milliers de coefficients. Comment produire un écho qui se répète avec une seule multiplication par échantillon ?

## Objectifs

- Réaliser une **ligne à retard** sur un buffer circulaire ;

- Passer d’un écho simple (RIF) à un écho répété (**récursion**) en changeant *ce qu’on écrit* dans le buffer ;

- Relever les deux réponses impulsionnelles à l’oscilloscope ; relier le gain $g$ à la décroissance et à la stabilité ;

- Comparer le coût d’une récursion et d’une convolution, et expliquer pourquoi une boucle de rétroaction ne traverse pas un traitement par blocs.

## Prérequis

- TP4 terminé (buffer circulaire, index d’écriture, masque binaire).

> **Projet Arduino.** `code/jour2/TP6_echo`. On complète `echo.cpp`.

> **Montage et réglages.** Oscilloscope : voie 1 sur `LINE OUT` gauche (signal traité), voie 2 sur `LINE OUT` droite (source non traitée), déclenchement sur la voie 2. Tous les réglages se font dans `setup()`, dans le fichier `.ino`, puis on téléverse à nouveau (une vingtaine de secondes) :
>
> - **source** : gain 1 sur une seule voie du mélangeur `source` : *impulsions* (une toutes les 100 ms : la voie 1 montre directement $h[n]$), *sinus* de test, ou *musique* du PC par USB (*Outils $\rightarrow$ USB Type $\rightarrow$ « Serial + MIDI + Audio »*) ;
>
> - **écho** : retard et gain réglés dans `setup()` par `echo.setDelay(ms)` et `echo.setGain(g)` ;

## Cours éclair : ligne à retard et réinjection

#### La ligne à retard.

Comme au TP4, on range le signal dans un buffer circulaire `buf_` de `BUF_N` $= 32\,768 = 2^{15}$ cases. À chaque échantillon, on *écrit* dans la case $w$, puis $w$ avance d’une case. Ce qu’on a écrit dans une case ressort $D$ échantillons plus tard : on le *lit* $D$ cases avant $w$, à l’indice $(w - D)\ \&\ (\text{\texttt{BUF\_N}} - 1)$. *(Schéma : voir la fiche PDF.)*

#### Écho simple ou répété : ce qu’on écrit dans le buffer.

La sortie est toujours le signal direct plus la valeur lue, multipliée par $g$ : $y[n] = x[n] + g \times (\text{valeur lue})$. Tout dépend de ce qu’on a écrit dans le buffer :

- on écrit l’**entrée** $x$ : la valeur lue est $x[n-D]$, et $y[n] = x[n] + g\,x[n-D]$. L’écho ressort une fois : c’est un RIF à deux coefficients (le noyau « écho » du TP4) ;

- on écrit la **sortie** $y$ : la valeur lue est $y[n-D]$, et $y[n] = x[n] + g\,y[n-D]$. La sortie contient l’écho ; elle est réécrite dans le buffer, ressort $D$ échantillons plus tard, multipliée à nouveau par $g$, est réécrite, et ainsi de suite : c’est une **récursion**.

*(Schéma : voir la fiche PDF.)* Pour une impulsion $x = \delta[n]$, dans un buffer de 8 cases avec $D = 4$ : *(Schéma : voir la fiche PDF.)* *(Schéma : voir la fiche PDF.)* La réponse impulsionnelle de l’écho répété est $\delta[n] + g\,\delta[n-D] + g^2\,\delta[n-2D] +
\cdots$ : une infinité de répétitions décroissantes pour *une* multiplication par échantillon.

> **Attention.** Si $g \geq 1$, les répétitions ne décroissent plus : la boucle diverge.

## Écho simple

- **Q1.** Dans `echo.cpp`, compléter les quatre TODO (Q1) en écrivant l’*entrée* $x$ dans le buffer. Source « impulsions », `setDelay(20.0f)`, `setGain(0.5f)` : relever à l’oscilloscope la réponse (nombre d’impulsions, écart, amplitudes) et la comparer à la théorie. Écouter sur la musique avec un retard de 250 ms.

*Extrait du squelette : `code/jour2/TP6_echo/echo.cpp`*

```cpp
// TODO (Q1) 1) lire ce qui a été écrit D_ échantillons plus tôt :
//              case (w_ - D_) & BUF_MASK
const float retarde = 0.0f;                    // provisoire

// TODO (Q1) 2) sortie : signal direct + g_ × valeur retardée
const float y = x;                             // provisoire

// TODO (Q1) 3) écrire l'entrée x dans le buffer, case w_
// TODO (Q2)    puis : écrire la sortie y à la place de x

// TODO (Q1) 4) avancer w_ d'une case, avec bouclage

block->data[n] = versInt16(y);
```

## Écho répété : la réinjection

- **Q2.** Modifier *une seule ligne* : écrire la *sortie* $y$ dans le buffer au lieu de $x$. Même relevé : écart et rapport d’amplitude de deux impulsions successives. Combien de répétitions faut-il pour que l’écho descende de 60 dB ? En quelle durée ? Essayer $g = 0{,}9$ puis $g = 1$ : que se passe-t-il ?

- **Q3.** Le buffer fait `BUF_N` $= 32\,768$ `float`. Taille en octets ? Retard maximal ? Combien de coefficients faudrait-il à un RIF équivalent pour $D = 250$ ms et $g = 0{,}4$ (répétitions jusqu’à $-60$ dB) ?

- **Q4.** Pourquoi ne peut-on pas placer la réinjection autour d’un traitement par blocs (la convolution FFT du TP5, par exemple) pour un retard $D = 50$ échantillons ?

## Livrables

- Réponses impulsionnelles de l’écho simple et de l’écho répété relevées à l’oscilloscope ;

- Réponses argumentées à Q2, Q3 et Q4 (décroissance et stabilité, coût RIF / récursion, boucle et traitement par blocs).

> **Pour aller plus loin.** Rendre le retard de l’écho continûment variable (lecture entre deux cases du buffer, par interpolation linéaire) : c’est le principe du chorus et du flanger. Écouter l’effet d’une modulation lente du retard.
