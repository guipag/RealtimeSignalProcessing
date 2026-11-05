# Installation de l'environnement

À réaliser **avant** la séance (compter 20–30 min avec le téléchargement). Installer **les versions
indiquées dans [`LOGICIELS.md`](LOGICIELS.md)**, pour que tout le monde travaille avec les mêmes outils. En cas de doute,
suivez cet ordre : Arduino IDE → Teensyduino → test d'un blink → REW (optionnel).

---

## 1. Arduino IDE 2.x

Télécharger depuis <https://www.arduino.cc/en/software> et installer la version 2.x pour votre
système.

- **Windows** : exécuter l'installeur, accepter l'installation des pilotes.
- **macOS** : glisser l'app dans Applications.
- **Linux** : décompresser l'AppImage / le paquet ; ajouter votre utilisateur au groupe
  `dialout` pour l'accès au port série :
  ```bash
  sudo usermod -a -G dialout $USER   # puis se déconnecter/reconnecter
  ```

## 2. Teensyduino (support Teensy)

Deux méthodes, au choix.

**Méthode recommandée — Boards Manager (Arduino IDE 2.x)**

1. Ouvrir *Fichier → Préférences*.
2. Dans **« URLs de gestionnaire de cartes supplémentaires »**, ajouter :
   ```
   https://www.pjrc.com/teensy/package_teensy_index.json
   ```
3. Ouvrir *Outils → Type de carte → Gestionnaire de cartes*, chercher **« Teensy »**,
   installer le paquet **Teensy (for Arduino IDE 2.x)**.

**Méthode alternative — installeur PJRC**

Depuis <https://www.pjrc.com/teensy/td_download.html>, installer Teensyduino qui embarque
déjà l'Audio Library.

> ℹ️ L'**Audio Library** et le **Serial Plotter** sont installés avec Teensyduino : rien de
> plus à ajouter pour les TP.

## 3. Vérification (Blink)

1. Brancher la Teensy en USB.
2. *Outils → Type de carte* → **Teensy 4.0** (ou 4.1).
3. *Outils → USB Type* → **Serial**.
4. *Outils → Port* → sélectionner le port Teensy (la fenêtre **Teensy Loader** s'ouvre au
   premier téléversement).
5. Ouvrir *Fichier → Exemples → 01.Basics → Blink*, téléverser. La LED doit clignoter.

> Le bouton **PROGRAM** de la Teensy force le mode programmation si le téléversement
> automatique échoue.

## 4. REW — Room EQ Wizard (optionnel, postes « mesure »)

- Télécharger depuis <https://www.roomeqwizard.com/> (nécessite un **JRE** ; l'installeur le
  propose).
- Utilisé au TP3 et lors de la mesure de latence pour visualiser un spectre / une réponse en
  fréquence via l'entrée ligne/micro de la carte son du PC.

## 5. Pupitre MIDI (aucune installation)

Les paramètres des effets se règlent depuis le PC, en MIDI, via une page web :

1. Dans l'IDE : *Outils → USB Type →* **« Serial + MIDI + Audio »** (conserve le Moniteur série).
2. Ouvrir [`pupitre_midi.html`](pupitre_midi.html) dans **Chrome** ou **Edge**
   (le Web MIDI n'est pas disponible sur Safari ni Firefox par défaut).
3. Autoriser l'accès MIDI, choisir la sortie **« Teensy MIDI/Audio »** et le preset du TP.

Rien à installer : la Teensy est *class-compliant*, elle apparaît toute seule comme périphérique
MIDI **et** carte son. Détails et table des CC : [`branchements.md`](../materiel/branchements.md).

## 6. Design Tool (aucune installation)

Le générateur de graphe de l'Audio Library est **en ligne** :
<https://www.pjrc.com/teensy/gui/>. On y glisse les objets, on relie les patch cords, on
exporte le code (bouton **Export**).

---

## Dépannage express

| Symptôme | Piste |
|---|---|
| Port Teensy absent | Câble USB « charge seule » → utiliser un câble **data** |
| « error opening serial port » (Linux) | groupe `dialout` non appliqué → se reconnecter |
| Téléversement bloqué | appuyer sur le bouton **PROGRAM** de la carte |
| Pas de son | vérifier `sgtl5000.enable()`, volume, sélection L/R du casque |
| Compilation : *Audio.h not found* | Teensyduino non installé / carte non-Teensy sélectionnée |
| Pas de port « Teensy MIDI » | USB Type ≠ « Serial + MIDI + Audio » → rechanger et retéléverser |
| Faders du pupitre sans effet | `usbMIDI.read()` manquant dans `loop()` |
