# Jour 2 — Convolution en temps réel

Journée 8h00–12h00 / 13h30–17h30, deux pauses de 15 min. Travail **individuel** : un poste par
étudiant (Teensy 4.x + Audio Shield, casque, oscilloscope deux voies, PC).
Un **QCM** court ouvre chaque demi-journée.

> **Entrée audio — USB.** Contrairement au Jour 1 (entrée `LINE IN` pour les mesures), le Jour 2
> utilise l'**entrée USB** pour l'écoute : la Teensy se déclare comme carte son du PC. Réglage une
> fois pour toutes : *Outils → USB Type → « Serial + MIDI + Audio »* (garde le Moniteur série),
> puis choisir « Teensy MIDI/Audio » comme sortie son du PC.

> **Pas de MIDI au Jour 2 : l'oscilloscope.** Chaque TP est un petit projet Arduino qui ne contient
> qu'un algorithme. Les réglages (source, réponse impulsionnelle, paramètres) sont des lignes de
> `setup()` : on les modifie et on téléverse à nouveau. Trois sources au choix : **impulsions**
> (une toutes les 100 ms : la voie traitée montre directement h[n]), **sinus** de test, **musique**
> USB. Voie gauche = signal traité, voie droite = source non traitée (déclenchement de
> l'oscilloscope). La charge processeur s'affiche chaque seconde dans le moniteur série.

## Fil rouge

Toute la journée porte sur **une seule opération : la convolution** `y = h * x`, calculée bloc par
bloc dans `update()`.

1. **TP4** — la calculer directement, comprendre la mémoire entre blocs et la ranger dans un
   **buffer circulaire**, mesurer son coût (M multiplications par
   échantillon) ;
2. **TP5** — la calculer par FFT : convolution circulaire vs linéaire, **overlap-save**
   (recouvrement des entrées, comme le buffer circulaire du TP4), validation à l'oscilloscope
   contre la directe et longueur de rentabilité ; puis **overlap-add** (zéro-padding, recouvrement
   des sorties) et comparaison des deux méthodes ;
3. **TP6** — l'**écho** : une ligne à retard (buffer circulaire) ; en y écrivant l'entrée, un
   écho simple (RIF) ; en y écrivant la sortie, un écho qui se répète (récursion). Sa réponse
   infinie montre la limite de la convolution ;
4. **Projet** (lancé en fin de journée, rendu en décembre) — **compresseur, limiteur, suréchantillonnage** :
   un même algorithme, effet dynamique quand il est lent, distorsion (harmoniques, repliement)
   quand il est rapide, puis calculé à 2 ou 4 × Fs pour supprimer le repliement ; caractérisé
   **par la mesure** à l'oscilloscope.

## Déroulé

### Matin

| Horaire | Séquence | Support |
|---|---|---|
| 08h00–08h15 | QCM (Jour 1), objectifs du Jour 2 | QCM |
| 08h15–09h00 | Cours : convolution par blocs, mémoire entre blocs (buffer circulaire), budget de calcul | slides |
| 09h00–10h35 | **TP4 — Convolution directe par blocs** | [`TP4`](TP4_convolution_directe.md) |
| 10h35–11h05 | Pause | — |
| 11h05–11h50 | Cours : TFD / FFT et nombres complexes, convolution circulaire, zéro-padding, overlap-add, overlap-save, coût | slides |
| 11h50–12h00 | **TP5 — Convolution rapide par FFT** (Q1 : choix de N) | [`TP5`](TP5_convolution_fft.md) |
| 12h00–13h30 | Déjeuner | — |

### Après-midi

| Horaire | Séquence | Support |
|---|---|---|
| 13h30–13h45 | QCM (convolution, FFT) | QCM |
| 13h45–15h15 | **TP5** (suite) — overlap-save, validation à l'oscilloscope, coût ; overlap-add | [`TP5`](TP5_convolution_fft.md) |
| 15h15–15h45 | Pause | — |
| 15h45–16h05 | Cours : ligne à retard, écho simple et répété (ce qu'on écrit dans le buffer), récursion | slides |
| 16h05–16h55 | **TP6 — Écho sur ligne à retard** | [`TP6`](TP6_echo.md) |
| 16h55–17h30 | **Lancement du projet** : compresseur, limiteur, suréchantillonnage ; synthèse des deux jours | [`Projet`](Projet_compresseur_limiteur.md) |

## Correspondance fiches ↔ code

| TP | Fiche | Objet custom | Dossier code |
|---|---|---|---|
| TP4 | [`TP4_convolution_directe.md`](TP4_convolution_directe.md) | `AudioFilterConvDirecte` | [`code/jour2/TP4a_convolution_directe`](../../code/jour2/TP4a_convolution_directe) |
| TP5 | [`TP5_convolution_fft.md`](TP5_convolution_fft.md) | `AudioFilterConvOLS` | [`code/jour2/TP5a_fft_overlap_save`](../../code/jour2/TP5a_fft_overlap_save) |
| TP5 (OLA) | idem | `AudioFilterConvFFT` | [`code/jour2/TP5b_fft_overlap_add`](../../code/jour2/TP5b_fft_overlap_add) |
| TP6 | [`TP6_echo.md`](TP6_echo.md) | `AudioEffectEcho` | [`code/jour2/TP6_echo`](../../code/jour2/TP6_echo) |
| Projet | [`Projet_compresseur_limiteur.md`](Projet_compresseur_limiteur.md) | `AudioEffectCompresseur` | [`code/jour2/Projet_compresseur`](../../code/jour2/Projet_compresseur) (parties A, B) · [`code/jour2/Projet_surechantillonnage`](../../code/jour2/Projet_surechantillonnage) (partie C) |

Chaque projet est indépendant. Fichiers communs
copiés dans les projets : `impulsions.h` (source d'impulsions), `conversion.h` (float → int16 avec
saturation), `noyaux.h/.cpp` (une fonction par réponse impulsionnelle : identité, retard, écho,
moyenne, passe-bas, salle), `fft_cmsis.h` (FFT `arm_cfft_f32` de la bibliothèque CMSIS-DSP d'ARM, jusqu'à 4096 points,
type `complex` = `std::complex<float>`).

Pour les étudiants en avance : le TP avancé **A2** (sigma-delta), dans [`avance/`](../../code/avance).

## Points de vigilance (Jour 2)

- **Mémoire entre blocs** : sans historique (TP4, TP5 overlap-save) ou sans queue (overlap-add), le son grésille à
  Fs/128 ≈ 345 Hz et les retards supérieurs à 128 échantillons disparaissent.
- **Buffer circulaire** (TP4, puis l'écho du TP6) : taille en puissance de 2 pour remplacer le
  modulo par un masque `& (N − 1)`, qui reste juste quand l'indice `w − k` est négatif.
- **Zéro-padding** : N ≥ L + M − 1, sinon repliement circulaire. M max = 3968 pour N = 4096.
- **Calculs lourds hors `update()`** : le noyau et sa FFT sont calculés dans `setup()`, **avant**
  `AudioMemory()` : tant qu'il n'y a pas de mémoire audio, `update()` ne reçoit aucun bloc.
- **Budget CPU** : ≈ 13 600 cycles par échantillon pour tout le traitement. Le moniteur série
  affiche `processorUsageMax()` de l'objet étudié.
- **Rétroaction** : un écho ne passe pas par un traitement par blocs (retard de boucle ≥ 128
  échantillons) : récursion échantillon par échantillon.

