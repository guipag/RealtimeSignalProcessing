# Logiciels — versions de référence

Tout le monde installe **les mêmes versions** : un écart de version de Teensyduino suffit à
changer un message de compilation ou le comportement d'un objet de l'Audio Library. Si une
version plus récente sort pendant le module, garder celle-ci.

| Logiciel | Version | Téléchargement | Rôle |
|---|---|---|---|
| Arduino IDE | **2.3.10** | <https://www.arduino.cc/en/software> (anciennes versions : <https://github.com/arduino/arduino-ide/releases>) | éditer, compiler, téléverser |
| Teensyduino (paquet « Teensy » du gestionnaire de cartes) | **1.62** | URL à ajouter dans l'IDE : `https://www.pjrc.com/teensy/package_teensy_index.json` · installeur : <https://www.pjrc.com/teensy/td_download.html> | support Teensy, Audio Library, CMSIS-DSP, Teensy Loader (téléversement) |
| Navigateur | Chrome ou Edge récent | — | pupitre MIDI (Web MIDI), Design Tool |
| REW — Room EQ Wizard (optionnel) | dernière version stable | <https://www.roomeqwizard.com/> | spectres et réponses en fréquence via la carte son du PC |

Outils en ligne, sans installation :

- **Design Tool** de l'Audio Library : <https://www.pjrc.com/teensy/gui/>
- **Pupitre MIDI** du cours : [`pupitre_midi.html`](pupitre_midi.html) (à ouvrir dans Chrome ou Edge)

Procédure pas à pas : [`INSTALLATION.md`](INSTALLATION.md).
