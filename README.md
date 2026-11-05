# Traitement numérique du signal en temps réel — E5e

Fiches de TP, code à compléter et ressources du module **TNS temps réel** (ESEO, E5e), sur
**Teensy 4.0/4.1 + Audio Shield** (codec SGTL5000) avec la Teensy Audio Library.

> Prérequis : bases de TNS hors-ligne (échantillonnage, convolution, transformée en z, filtres
> RIF/RII), C/C++ élémentaire, Arduino IDE.

---

## Avant la première séance

1. Installer les logiciels **dans les versions indiquées** :
   [`ressources_generales/logiciels/LOGICIELS.md`](ressources_generales/logiciels/LOGICIELS.md),
   puis suivre [`INSTALLATION.md`](ressources_generales/logiciels/INSTALLATION.md).
2. Vérifier le matériel et le câblage :
   [`ressources_generales/materiel/branchements.md`](ressources_generales/materiel/branchements.md).
3. Télécharger ce dépôt : bouton **Code → Download ZIP**, ou
   ```bash
   git clone <url-de-ce-depot>
   ```

## Organisation

```
├── code/                        Projets Arduino à compléter (// TODO), rangés par jour
│   ├── jour1/                   TP1_passthrough, TP2_gain, TP3_biquad
│   ├── jour2/                   TP4a, TP5a, TP5b, TP6_echo, Projet_compresseur, Projet_surechantillonnage
│   └── avance/                  A2_sigma_delta (TP avancé)
├── ressources/                  Fiches de TP : PDF imprimable + Markdown, et diapositives
│   ├── jour1/
│   ├── jour2/                   + planning.md (déroulé de la journée)
│   └── avance/
└── ressources_generales/
    ├── livres/                  Références (DAFx, cookbook RBJ, doc PJRC)
    ├── logiciels/               Versions, installation, pupitre MIDI
    └── materiel/                Câblage, photos de la carte
```

Chaque dossier de `code/` est un projet Arduino complet : ouvrir le `.ino` dans l'IDE, les
autres fichiers s'ouvrent en onglets. Le squelette affiché dans une fiche est exactement le
contenu du fichier correspondant.

> Les fiches PDF citent les projets sous la forme `jour2/TP6_echo` : ils se trouvent dans
> `code/jour2/TP6_echo`.

---

## Jour 1 — Fondations temps réel

| TP | Fiche | Code |
|---|---|---|
| TP1 — Passthrough et prise en main | [PDF](ressources/jour1/TP1_passthrough.pdf) · [Markdown](ressources/jour1/TP1_passthrough.md) | [`code/jour1/TP1_passthrough`](code/jour1/TP1_passthrough) |
| TP2 — Objet « gain » | [PDF](ressources/jour1/TP2_gain.pdf) · [Markdown](ressources/jour1/TP2_gain.md) | [`code/jour1/TP2_gain`](code/jour1/TP2_gain) |
| TP3 — Filtre biquad | [PDF](ressources/jour1/TP3_biquad.pdf) · [Markdown](ressources/jour1/TP3_biquad.md) | [`code/jour1/TP3_biquad`](code/jour1/TP3_biquad) |

Diapositives : [`ressources/jour1/Diapositives_jour1.pdf`](ressources/jour1/Diapositives_jour1.pdf).

## Jour 2 — Convolution en temps réel

Déroulé détaillé : [`ressources/jour2/planning.md`](ressources/jour2/planning.md).

| TP | Fiche | Code |
|---|---|---|
| TP4 — Convolution directe par blocs | [PDF](ressources/jour2/TP4_convolution_directe.pdf) · [Markdown](ressources/jour2/TP4_convolution_directe.md) | [`code/jour2/TP4a_convolution_directe`](code/jour2/TP4a_convolution_directe) |
| TP5 — Convolution rapide par FFT | [PDF](ressources/jour2/TP5_convolution_fft.pdf) · [Markdown](ressources/jour2/TP5_convolution_fft.md) | [`TP5a_fft_overlap_save`](code/jour2/TP5a_fft_overlap_save) · [`TP5b_fft_overlap_add`](code/jour2/TP5b_fft_overlap_add) |
| TP6 — Écho sur ligne à retard | [PDF](ressources/jour2/TP6_echo.pdf) · [Markdown](ressources/jour2/TP6_echo.md) | [`code/jour2/TP6_echo`](code/jour2/TP6_echo) |
| **Projet** — compresseur, limiteur, suréchantillonnage (à rendre en décembre) | [PDF](ressources/jour2/Projet_compresseur_limiteur.pdf) · [Markdown](ressources/jour2/Projet_compresseur_limiteur.md) | [`Projet_compresseur`](code/jour2/Projet_compresseur) (parties A, B) · [`Projet_surechantillonnage`](code/jour2/Projet_surechantillonnage) (partie C) |

Diapositives : [`ressources/jour2/Diapositives_jour2.pdf`](ressources/jour2/Diapositives_jour2.pdf).

## TP avancé — Quantification et sigma-delta

| TP | Fiche | Code |
|---|---|---|
| A2 — Sigma-delta 1 bit | [PDF](ressources/avance/A2_sigma_delta.pdf) | [`code/avance/A2_sigma_delta`](code/avance/A2_sigma_delta) |

---

## Réglages pendant les TP

- **Jour 1** : paramètres réglés en MIDI depuis le PC avec le
  [pupitre](ressources_generales/logiciels/pupitre_midi.html) (Chrome ou Edge), sur le même câble
  USB que l'audio. *Outils → USB Type →* **Serial + MIDI + Audio**.
- **Jour 2** : réglages dans `setup()` ou au moniteur série (projet) ; observation à
  l'oscilloscope ; charge processeur affichée au moniteur série.

## Conventions de code

- `Fs = 44,1 kHz`, blocs de `AUDIO_BLOCK_SAMPLES = 128` échantillons (≈ 2,9 ms).
- Échantillons en `int16` dans les `audio_block_t` ; calcul interne en `float`, avec
  **saturation** avant de réécrire en `int16`.
- **Jamais** d'allocation dynamique ni d'attente bloquante dans `update()` ; MIDI et
  affichages dans `loop()`.

## Licence

Support pédagogique © Guilhem PAGES, sous **CC BY-NC-SA 4.0** — voir [`LICENSE.md`](LICENSE.md).
La Teensy Audio Library est © Paul Stoffregen / PJRC (MIT).
