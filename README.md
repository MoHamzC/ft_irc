# ft_irc - Internet Relay Chat Server

## 📋 Description

Implémentation complète d'un serveur IRC (Internet Relay Chat) en C++98 conforme au RFC 2812.  
Ce projet fait partie du cursus 42 et reproduit fidèlement le fonctionnement d'un serveur IRC professionnel.

## ✨ Fonctionnalités implémentées

### 🔐 Authentification complète
- **PASS** : Authentification par mot de passe
- **NICK** : Gestion des pseudonymes (validation RFC)
- **USER** : Enregistrement des informations utilisateur
- Validation complète des nicknames selon RFC 2812
- Messages de bienvenue (RPL_WELCOME, RPL_YOURHOST, etc.)

### 💬 Gestion des canaux
- **JOIN** : Création et rejoindre des canaux
- **PART** : Quitter des canaux
- **PRIVMSG** : Messages vers canaux et utilisateurs privés
- Création automatique des canaux
- Notification des membres lors des JOIN/PART

### 👑 Opérateurs de canaux
- **KICK** : Éjecter un utilisateur (opérateurs uniquement)
- **INVITE** : Inviter un utilisateur 
- **TOPIC** : Gérer le sujet du canal
- **MODE** : Modes de canal complets (i,t,k,o,l)
- Gestion des permissions et restrictions

### 🔧 Modes de canal supportés
- **+i** : Canal sur invitation uniquement
- **+t** : Restriction du TOPIC aux opérateurs
- **+k** : Clé de canal (mot de passe)
- **+o** : Privilèges d'opérateur
- **+l** : Limite du nombre d'utilisateurs

### 🌐 Fonctionnalités techniques
- **Multi-clients** : Support de connexions simultanées
- **poll()** : I/O non-bloquant avec poll()
- **C++98** : Code strictement conforme C++98
- **RFC compliant** : Respect du protocole IRC standard
- **Gestion des timeouts** : Déconnexion automatique
- **Parsing robuste** : Messages IRC avec trailing parameters

## 🚀 Compilation et utilisation

### Compilation
```bash
make           # Compilation standard
make debug     # Compilation avec debug
make release   # Compilation optimisée
make clean     # Nettoyer les objets
make fclean    # Nettoyer tout
```

### Utilisation
```bash
./ft_irc <port> <password>

# Exemple :
./ft_irc 6667 mypassword
```

### Paramètres
- **port** : Port d'écoute (1024-65535)
- **password** : Mot de passe du serveur

## 🧪 Tests

### Test rapide avec netcat
```bash
echo -e "PASS password\\r\\nNICK alice\\r\\nUSER alice 0 * :Alice\\r\\nJOIN #general\\r\\nPRIVMSG #general :Hello!\\r\\n" | nc localhost 6667
```

### Test complet automatisé
```bash
./test_complete.sh
```

### Test avec clients IRC réels
Compatible avec : **irssi**, **HexChat**, **WeeChat**, **Textual**

Exemple avec irssi :
```bash
irssi
/connect localhost 6667 password alice
/join #general
/msg #general Hello world!
```

## 📁 Structure du projet

```
ft_irc/
├── README.md               # Documentation
├── .gitignore             # Fichiers à ignorer
├── Makefile               # Build system
├── main.cpp               # Point d'entrée
├── Server.hpp/cpp         # Serveur principal
├── Client.hpp/cpp         # Gestion des clients
├── ClientManager.hpp/cpp  # Gestionnaire de clients
├── AuthHandler.hpp/cpp    # Authentification IRC
├── CommandParser.hpp/cpp  # Parsing des commandes
├── Channel.hpp/cpp        # Gestion des canaux
├── ChannelManager.hpp/cpp # Gestionnaire de canaux
└── test_complete.sh       # Script de test
```

## 🎯 Conformité au sujet

### ✅ Exigences respectées
- [x] Pas de forking (single-threaded)
- [x] I/O non-bloquant avec poll()
- [x] C++98 strict (-std=c++98)
- [x] Gestion multi-clients
- [x] Authentification complète
- [x] Canaux et messages
- [x] Opérateurs et modes
- [x] Commandes obligatoires (KICK, INVITE, TOPIC, MODE)
- [x] Modes obligatoires (i,t,k,o,l)

## 🔍 Commandes IRC supportées

### Authentification
- `PASS <password>` - Authentification
- `NICK <nickname>` - Définir/changer le pseudonyme  
- `USER <username> <hostname> <servername> <realname>` - Enregistrement

### Canaux
- `JOIN <#channel> [key]` - Rejoindre un canal
- `PART <#channel> [reason]` - Quitter un canal
- `PRIVMSG <target> <message>` - Envoyer un message
- `TOPIC <#channel> [topic]` - Voir/définir le sujet

### Opérateurs
- `KICK <#channel> <nick> [reason]` - Éjecter un utilisateur
- `INVITE <nick> <#channel>` - Inviter un utilisateur
- `MODE <#channel> <modes> [params]` - Gérer les modes

### Utilitaires
- `PING [server]` - Test de connexion
- `QUIT [reason]` - Déconnexion

## 🏆 Qualité du code

- **Architecture modulaire** : Séparation claire des responsabilités
- **Gestion d'erreurs robuste** : Validation et codes de retour
- **Memory management** : Pas de fuites mémoire
- **Thread-safe** : Design single-threaded sécurisé
- **RFC compliant** : Respect strict du protocole IRC
- **Makefile professionnel** : Build system complet

## 📊 Progression : 100% ✅

**🎉 PROJET TERMINÉ - PRÊT POUR L'ÉVALUATION**

---

**Auteur** : Équipe ft_irc  
**École** : 42  
**Date** : Juin 2025
