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

### 🎯 Commandes de démonstration pour évaluation

#### 🔐 Authentification et connexion
```bash
# Connexion basique
echo -e "PASS password\r\nNICK evaluator\r\nUSER evaluator 0 * :Evaluation User\r\n" | nc localhost 6667

# Test nickname déjà pris
echo -e "PASS password\r\nNICK evaluator\r\nUSER test 0 * :Test\r\n" | nc localhost 6667

# Test mauvais mot de passe
echo -e "PASS wrongpass\r\nNICK testuser\r\nUSER test 0 * :Test\r\n" | nc localhost 6667
```

#### 💬 Gestion des canaux avancée
```bash
# Créer et rejoindre plusieurs canaux
echo -e "PASS password\r\nNICK admin\r\nUSER admin 0 * :Admin\r\nJOIN #general\r\nJOIN #42\r\nJOIN #coding\r\n" | nc localhost 6667

# Rejoindre canal avec clé
echo -e "PASS password\r\nNICK user1\r\nUSER user1 0 * :User1\r\nJOIN #private secret123\r\n" | nc localhost 6667

# Quitter canal avec raison
echo -e "PASS password\r\nNICK leaver\r\nUSER leaver 0 * :Leaver\r\nJOIN #test\r\nPART #test :Going to lunch\r\n" | nc localhost 6667
```

#### 🔧 Modes de canal - Démonstration complète
```bash
# Configurer tous les modes
echo -e "PASS password\r\nNICK operator\r\nUSER operator 0 * :Op\r\nJOIN #demo\r\nMODE #demo +t\r\nMODE #demo +i\r\nMODE #demo +k secretkey\r\nMODE #demo +l 50\r\nMODE #demo +o operator\r\n" | nc localhost 6667

# Vérifier les modes actifs
echo -e "PASS password\r\nNICK checker\r\nUSER checker 0 * :Checker\r\nJOIN #demo\r\nMODE #demo\r\n" | nc localhost 6667

# Retirer des modes
echo -e "PASS password\r\nNICK remover\r\nUSER remover 0 * :Remover\r\nJOIN #demo\r\nMODE #demo -t\r\nMODE #demo -i\r\nMODE #demo -k\r\nMODE #demo -l\r\n" | nc localhost 6667
```

#### 👑 Fonctions d'opérateur
```bash
# Donner privilèges opérateur
echo -e "PASS password\r\nNICK owner\r\nUSER owner 0 * :Owner\r\nJOIN #ops\r\nMODE #ops +o target_user\r\n" | nc localhost 6667

# Kick avec raison
echo -e "PASS password\r\nNICK kicker\r\nUSER kicker 0 * :Kicker\r\nJOIN #discipline\r\nKICK #discipline baduser :Inappropriate behavior\r\n" | nc localhost 6667

# Invitation sur canal privé
echo -e "PASS password\r\nNICK inviter\r\nUSER inviter 0 * :Inviter\r\nJOIN #private\r\nMODE #private +i\r\nINVITE friend #private\r\n" | nc localhost 6667
```

#### 🏷️ Gestion des topics
```bash
# Définir topic
echo -e "PASS password\r\nNICK topic_setter\r\nUSER setter 0 * :Setter\r\nJOIN #discussion\r\nTOPIC #discussion :Welcome to our discussion channel!\r\n" | nc localhost 6667

# Voir topic actuel
echo -e "PASS password\r\nNICK topic_viewer\r\nUSER viewer 0 * :Viewer\r\nJOIN #discussion\r\nTOPIC #discussion\r\n" | nc localhost 6667

# Topic sur canal +t (opérateurs seulement)
echo -e "PASS password\r\nNICK restricted\r\nUSER restricted 0 * :Restricted\r\nJOIN #moderated\r\nMODE #moderated +t\r\nTOPIC #moderated :Ops only topic\r\n" | nc localhost 6667
```

#### 💬 Messages privés et de canal
```bash
# Messages de canal
echo -e "PASS password\r\nNICK chatter\r\nUSER chatter 0 * :Chatter\r\nJOIN #chat\r\nPRIVMSG #chat :Hello everyone in the channel!\r\n" | nc localhost 6667

# Message privé utilisateur
echo -e "PASS password\r\nNICK messenger\r\nUSER messenger 0 * :Messenger\r\nPRIVMSG target_user :Private message for you\r\n" | nc localhost 6667

# Message avec caractères spéciaux
echo -e "PASS password\r\nNICK special\r\nUSER special 0 * :Special\r\nJOIN #test\r\nPRIVMSG #test :Message with émojis and spéciål chars! 🚀\r\n" | nc localhost 6667
```

#### 🚪 Déconnexion propre
```bash
# QUIT avec raison
echo -e "PASS password\r\nNICK quitter\r\nUSER quitter 0 * :Quitter\r\nJOIN #goodbye\r\nPRIVMSG #goodbye :I'm leaving now\r\nQUIT :Thanks for the good time!\r\n" | nc localhost 6667

# QUIT sans raison
echo -e "PASS password\r\nNICK simple_quit\r\nUSER simple 0 * :Simple\r\nJOIN #test\r\nQUIT\r\n" | nc localhost 6667
```

#### 🧪 Tests de robustesse
```bash
# Test limites nickname
echo -e "PASS password\r\nNICK a\r\nUSER a 0 * :Single char\r\n" | nc localhost 6667
echo -e "PASS password\r\nNICK verylongnickname123\r\nUSER long 0 * :Long nick\r\n" | nc localhost 6667

# Test canal inexistant
echo -e "PASS password\r\nNICK tester\r\nUSER tester 0 * :Tester\r\nJOIN #nonexistent\r\nPRIVMSG #nonexistent :Testing\r\n" | nc localhost 6667

# Test commandes invalides
echo -e "PASS password\r\nNICK invalid\r\nUSER invalid 0 * :Invalid\r\nINVALIDCOMMAND param\r\nJOIN\r\nMODE\r\n" | nc localhost 6667

# Test permissions insuffisantes
echo -e "PASS password\r\nNICK noob\r\nUSER noob 0 * :Noob\r\nJOIN #restricted\r\nKICK #restricted someone :No permission\r\nMODE #restricted +o noob\r\n" | nc localhost 6667
```

#### 🔄 Tests multi-clients simultanés
```bash
# Terminal 1 - Admin
echo -e "PASS password\r\nNICK admin\r\nUSER admin 0 * :Admin\r\nJOIN #multi\r\nMODE #multi +t\r\nTOPIC #multi :Multi-client test\r\n" | nc localhost 6667 &

# Terminal 2 - User1  
echo -e "PASS password\r\nNICK user1\r\nUSER user1 0 * :User1\r\nJOIN #multi\r\nPRIVMSG #multi :User1 here!\r\n" | nc localhost 6667 &

# Terminal 3 - User2
echo -e "PASS password\r\nNICK user2\r\nUSER user2 0 * :User2\r\nJOIN #multi\r\nPRIVMSG #multi :User2 joining the party\r\n" | nc localhost 6667 &
```

#### 📊 Vérification d'état
```bash
# Voir membres d'un canal (automatique avec JOIN)
echo -e "PASS password\r\nNICK observer\r\nUSER observer 0 * :Observer\r\nJOIN #status\r\n" | nc localhost 6667

# Test case sensitivity nicknames
echo -e "PASS password\r\nNICK TestUser\r\nUSER test 0 * :Test\r\n" | nc localhost 6667
echo -e "PASS password\r\nNICK testuser\r\nUSER test2 0 * :Test2\r\n" | nc localhost 6667  # Doit échouer
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

### 🎭 Scénarios complets pour démonstration

#### Scénario 1 : Configuration d'un canal modéré
```bash
# Étape 1 : Admin crée et configure le canal
echo -e "PASS password\r\nNICK admin\r\nUSER admin 0 * :Channel Admin\r\nJOIN #moderated\r\nMODE #moderated +t\r\nMODE #moderated +i\r\nTOPIC #moderated :Canal modéré - Sur invitation uniquement\r\n" | nc localhost 6667 &

# Étape 2 : User normal essaie de rejoindre (échoue)
echo -e "PASS password\r\nNICK normaluser\r\nUSER normal 0 * :Normal User\r\nJOIN #moderated\r\n" | nc localhost 6667 &

# Étape 3 : Admin invite l'utilisateur
echo -e "PASS password\r\nNICK admin\r\nUSER admin 0 * :Admin\r\nINVITE normaluser #moderated\r\n" | nc localhost 6667 &

# Étape 4 : User rejoint après invitation
echo -e "PASS password\r\nNICK normaluser\r\nUSER normal 0 * :Normal User\r\nJOIN #moderated\r\n" | nc localhost 6667 &
```

#### Scénario 2 : Gestion disciplinaire
```bash
# Étape 1 : Plusieurs users rejoignent
echo -e "PASS password\r\nNICK moderator\r\nUSER mod 0 * :Moderator\r\nJOIN #discipline\r\n" | nc localhost 6667 &
echo -e "PASS password\r\nNICK gooduser\r\nUSER good 0 * :Good User\r\nJOIN #discipline\r\nPRIVMSG #discipline :Hello everyone!\r\n" | nc localhost 6667 &
echo -e "PASS password\r\nNICK baduser\r\nUSER bad 0 * :Bad User\r\nJOIN #discipline\r\nPRIVMSG #discipline :SPAM SPAM SPAM\r\n" | nc localhost 6667 &

# Étape 2 : Modérateur kick le spammeur
echo -e "PASS password\r\nNICK moderator\r\nUSER mod 0 * :Moderator\r\nKICK #discipline baduser :Spam is not allowed\r\n" | nc localhost 6667 &
```

#### Scénario 3 : Canal avec mot de passe
```bash
# Étape 1 : Admin crée canal privé
echo -e "PASS password\r\nNICK owner\r\nUSER owner 0 * :Owner\r\nJOIN #private\r\nMODE #private +k secretpass\r\nTOPIC #private :Canal privé sécurisé\r\n" | nc localhost 6667 &

# Étape 2 : User essaie sans mot de passe (échoue)
echo -e "PASS password\r\nNICK visitor\r\nUSER visitor 0 * :Visitor\r\nJOIN #private\r\n" | nc localhost 6667 &

# Étape 3 : User rejoint avec bon mot de passe
echo -e "PASS password\r\nNICK visitor\r\nUSER visitor 0 * :Visitor\r\nJOIN #private secretpass\r\n" | nc localhost 6667 &
```

#### Scénario 4 : Transfert de pouvoirs
```bash
# Étape 1 : Fondateur crée canal
echo -e "PASS password\r\nNICK founder\r\nUSER founder 0 * :Founder\r\nJOIN #transfer\r\n" | nc localhost 6667 &

# Étape 2 : Utilisateur de confiance rejoint
echo -e "PASS password\r\nNICK trusted\r\nUSER trusted 0 * :Trusted User\r\nJOIN #transfer\r\n" | nc localhost 6667 &

# Étape 3 : Transfert des privilèges
echo -e "PASS password\r\nNICK founder\r\nUSER founder 0 * :Founder\r\nMODE #transfer +o trusted\r\nPRIVMSG #transfer :Vous êtes maintenant opérateur!\r\n" | nc localhost 6667 &

# Étape 4 : Nouveau op teste ses pouvoirs
echo -e "PASS password\r\nNICK trusted\r\nUSER trusted 0 * :Trusted\r\nTOPIC #transfer :New management in place\r\n" | nc localhost 6667 &
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

## 🛠️ Commandes de debugging et monitoring

### Surveillance des connexions
```bash
# Vérifier les connexions actives
netstat -an | grep :6667

# Monitorer les connexions en temps réel
watch "netstat -an | grep :6667 | wc -l"

# Logs du serveur avec timestamps
./ft_irc 6667 password | while IFS= read -r line; do echo "$(date '+%H:%M:%S') $line"; done
```

### Tests de charge
```bash
# Connexions multiples simultanées
for i in {1..10}; do
    echo -e "PASS password\r\nNICK user$i\r\nUSER user$i 0 * :User $i\r\nJOIN #stress\r\nPRIVMSG #stress :User $i connected\r\nQUIT :Test done\r\n" | nc localhost 6667 &
done

# Test de déconnexions brutales
for i in {1..5}; do
    (echo -e "PASS password\r\nNICK brutal$i\r\nUSER brutal$i 0 * :Brutal $i\r\nJOIN #disconnect\r\n"; sleep 1; pkill -f "nc localhost 6667") &
done
```

### Validation RFC 2812
```bash
# Test trailing parameters avec espaces
echo -e "PASS password\r\nNICK rfc_tester\r\nUSER rfc 0 * :RFC 2812 Compliance Tester\r\nJOIN #rfc\r\nPRIVMSG #rfc :Message with trailing spaces and special chars: @#$%^&*()\r\n" | nc localhost 6667

# Test paramètres vides et mal formés
echo -e "PASS password\r\nNICK edge_case\r\nUSER edge 0 * :Edge Case\r\nJOIN\r\nJOIN #\r\nJOIN #toolongchannelnamethatexceedslimits\r\nMODE #test +xyz\r\n" | nc localhost 6667

# Test nicknames limites
echo -e "PASS password\r\nNICK 123starts_with_number\r\nUSER test 0 * :Test\r\n" | nc localhost 6667
echo -e "PASS password\r\nNICK valid_nick123\r\nUSER test 0 * :Test\r\n" | nc localhost 6667
```

### Tests d'erreurs communes
```bash
# Commandes avant authentification
echo -e "JOIN #test\r\nNICK testuser\r\nPASS password\r\nUSER test 0 * :Test\r\n" | nc localhost 6667

# Paramètres manquants
echo -e "PASS password\r\nNICK param_test\r\nUSER param_test 0 * :Param Test\r\nJOIN\r\nKICK\r\nINVITE\r\nMODE\r\nTOPIC\r\n" | nc localhost 6667

# Messages trop longs (test buffer overflow protection)
echo -e "PASS password\r\nNICK buffer_test\r\nUSER buffer 0 * :Buffer Test\r\nJOIN #buffer\r\nPRIVMSG #buffer :$(python3 -c 'print("A" * 1000)')\r\n" | nc localhost 6667
```

### Performance et stabilité
```bash
# Test ping-pong
echo -e "PASS password\r\nNICK pinger\r\nUSER pinger 0 * :Pinger\r\nPING :test\r\n" | nc localhost 6667

# Reconnexions rapides
for i in {1..20}; do
    echo -e "PASS password\r\nNICK rapid$i\r\nUSER rapid$i 0 * :Rapid $i\r\nQUIT :Rapid disconnect\r\n" | nc localhost 6667
    sleep 0.1
done

# Test memory leaks (longue session)
(echo -e "PASS password\r\nNICK memory_test\r\nUSER memory 0 * :Memory Test\r\nJOIN #memory\r\n"; 
 for i in {1..1000}; do 
     echo -e "PRIVMSG #memory :Message $i for memory test\r\n"; 
     sleep 0.01; 
 done; 
 echo -e "QUIT :Memory test complete\r\n") | nc localhost 6667
```

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
