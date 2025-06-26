# 📋 Guide d'Évaluation - ft_irc

## 🎯 Introduction pour les Correcteurs

Ce guide fournit un protocole complet d'évaluation du projet **ft_irc**. Il est conçu pour permettre aux correcteurs de valider rapidement et efficacement toutes les fonctionnalités implémentées, tout en testant la robustesse et la conformité du serveur IRC.

---

## ⚡ Démarrage rapide (5 minutes)

### 1. Compilation
```bash
cd ft_irc/
make
```
**✅ Attendu** : Compilation sans erreurs ni warnings

### 2. Lancement du serveur
```bash
./ft_irc 6667 password123
```
**✅ Attendu** : 
- Message de démarrage affiché
- Serveur en écoute sur le port 6667
- Pas de crash immédiat

### 3. Test de connexion basique
```bash
echo -e "PASS password123\r\nNICK evaluator\r\nUSER evaluator 0 * :Evaluation User\r\n" | nc localhost 6667
```
**✅ Attendu** :
```
NOTICE AUTH :*** Looking up your hostname...
NOTICE AUTH :*** Found your hostname
:ft_irc.42.fr 001 evaluator :Welcome to the Internet Relay Network...
:ft_irc.42.fr 002 evaluator :Your host is ft_irc.42.fr...
:ft_irc.42.fr 003 evaluator :This server was created...
:ft_irc.42.fr 004 evaluator :ft_irc.42.fr ft_irc-1.0 o o
```

---

## 🔍 Tests d'Authentification (10 minutes)

### Test 1 : Authentification correcte
```bash
echo -e "PASS password123\r\nNICK alice\r\nUSER alice 0 * :Alice User\r\n" | nc localhost 6667
```
**✅ Critères de validation** :
- Messages de bienvenue (001-004) reçus
- Pas d'erreurs d'authentification
- Client authentifié avec succès

### Test 2 : Mot de passe incorrect
```bash
echo -e "PASS wrongpassword\r\nNICK bob\r\nUSER bob 0 * :Bob User\r\n" | nc localhost 6667
```
**✅ Attendu** : Erreur 464 (Password incorrect)

### Test 3 : Nickname déjà pris
```bash
# Terminal 1
echo -e "PASS password123\r\nNICK charlie\r\nUSER charlie 0 * :Charlie\r\n" | nc localhost 6667 &

# Terminal 2 (après 2 secondes)
echo -e "PASS password123\r\nNICK charlie\r\nUSER charlie2 0 * :Charlie2\r\n" | nc localhost 6667
```
**✅ Attendu** : Erreur 433 (Nickname is already in use)

### Test 4 : Case sensitivity des nicknames
```bash
# Terminal 1
echo -e "PASS password123\r\nNICK TestUser\r\nUSER test1 0 * :Test1\r\n" | nc localhost 6667 &

# Terminal 2
echo -e "PASS password123\r\nNICK testuser\r\nUSER test2 0 * :Test2\r\n" | nc localhost 6667
```
**✅ Attendu** : Erreur 433 (les nicknames Bob et bob doivent être considérés identiques)

---

## 💬 Tests des Canaux (15 minutes)

### Test 5 : Création et JOIN de canal
```bash
echo -e "PASS password123\r\nNICK creator\r\nUSER creator 0 * :Creator\r\nJOIN #general\r\n" | nc localhost 6667
```
**✅ Attendu** :
```
:creator!creator@localhost JOIN :#general
:ft_irc.42.fr 353 creator = #general :@creator
:ft_irc.42.fr 366 creator #general :End of NAMES list
```

### Test 6 : Multiple clients dans un canal
```bash
# Terminal 1 - Premier client
echo -e "PASS password123\r\nNICK user1\r\nUSER user1 0 * :User1\r\nJOIN #multi\r\n" | nc localhost 6667 &

# Terminal 2 - Deuxième client
echo -e "PASS password123\r\nNICK user2\r\nUSER user2 0 * :User2\r\nJOIN #multi\r\n" | nc localhost 6667 &
```
**✅ Attendu** : user1 doit voir user2 rejoindre le canal

### Test 7 : Messages dans un canal
```bash
echo -e "PASS password123\r\nNICK chatter\r\nUSER chatter 0 * :Chatter\r\nJOIN #test\r\nPRIVMSG #test :Hello everyone!\r\n" | nc localhost 6667
```
**✅ Attendu** : Message diffusé à tous les membres du canal

### Test 8 : Messages privés
```bash
# Terminal 1 - Destinataire
echo -e "PASS password123\r\nNICK receiver\r\nUSER receiver 0 * :Receiver\r\n" | nc localhost 6667 &

# Terminal 2 - Expéditeur
echo -e "PASS password123\r\nNICK sender\r\nUSER sender 0 * :Sender\r\nPRIVMSG receiver :Private message!\r\n" | nc localhost 6667
```
**✅ Attendu** : receiver reçoit le message privé

---

## 🔧 Tests des Modes de Canal (15 minutes)

### Test 9 : Mode +t (Topic restriction)
```bash
# Créer canal et activer +t
echo -e "PASS password123\r\nNICK op\r\nUSER op 0 * :Operator\r\nJOIN #restricted\r\nMODE #restricted +t\r\n" | nc localhost 6667 &

# User normal essaie de changer le topic (doit échouer)
echo -e "PASS password123\r\nNICK normal\r\nUSER normal 0 * :Normal\r\nJOIN #restricted\r\nTOPIC #restricted :New topic\r\n" | nc localhost 6667
```
**✅ Attendu** : Erreur de permission pour l'utilisateur normal

### Test 10 : Mode +i (Invite only)
```bash
# Admin crée canal +i
echo -e "PASS password123\r\nNICK admin\r\nUSER admin 0 * :Admin\r\nJOIN #private\r\nMODE #private +i\r\n" | nc localhost 6667 &

# User essaie de rejoindre (doit échouer)
echo -e "PASS password123\r\nNICK visitor\r\nUSER visitor 0 * :Visitor\r\nJOIN #private\r\n" | nc localhost 6667
```
**✅ Attendu** : Erreur d'accès au canal

### Test 11 : Mode +k (Channel key)
```bash
# Créer canal avec clé
echo -e "PASS password123\r\nNICK owner\r\nUSER owner 0 * :Owner\r\nJOIN #secret\r\nMODE #secret +k secretkey\r\n" | nc localhost 6667 &

# JOIN sans clé (doit échouer)
echo -e "PASS password123\r\nNICK user\r\nUSER user 0 * :User\r\nJOIN #secret\r\n" | nc localhost 6667 &

# JOIN avec clé (doit réussir)
echo -e "PASS password123\r\nNICK user2\r\nUSER user2 0 * :User2\r\nJOIN #secret secretkey\r\n" | nc localhost 6667
```

### Test 12 : Mode +l (User limit)
```bash
echo -e "PASS password123\r\nNICK limiter\r\nUSER limiter 0 * :Limiter\r\nJOIN #limited\r\nMODE #limited +l 2\r\n" | nc localhost 6667
```
**✅ Attendu** : Limite d'utilisateurs appliquée

---

## 👑 Tests des Privilèges d'Opérateur (10 minutes)

### Test 13 : Commande KICK
```bash
# Terminal 1 - Opérateur
echo -e "PASS password123\r\nNICK moderator\r\nUSER mod 0 * :Moderator\r\nJOIN #moderated\r\n" | nc localhost 6667 &

# Terminal 2 - Utilisateur à kicker
echo -e "PASS password123\r\nNICK troublemaker\r\nUSER trouble 0 * :Troublemaker\r\nJOIN #moderated\r\n" | nc localhost 6667 &

# Terminal 3 - Kick
echo -e "PASS password123\r\nNICK moderator\r\nUSER mod 0 * :Moderator\r\nKICK #moderated troublemaker :Inappropriate behavior\r\n" | nc localhost 6667
```
**✅ Attendu** : troublemaker est éjecté du canal

### Test 14 : Commande INVITE
```bash
# Opérateur invite un utilisateur
echo -e "PASS password123\r\nNICK inviter\r\nUSER inviter 0 * :Inviter\r\nJOIN #exclusive\r\nMODE #exclusive +i\r\nINVITE guest #exclusive\r\n" | nc localhost 6667 &

# Utilisateur invité rejoint
echo -e "PASS password123\r\nNICK guest\r\nUSER guest 0 * :Guest\r\nJOIN #exclusive\r\n" | nc localhost 6667
```
**✅ Attendu** : guest peut rejoindre après invitation

### Test 15 : Attribution de privilèges +o
```bash
echo -e "PASS password123\r\nNICK founder\r\nUSER founder 0 * :Founder\r\nJOIN #newchan\r\nMODE #newchan +o target_user\r\n" | nc localhost 6667
```
**✅ Attendu** : target_user devient opérateur

---

## 🚪 Test de Déconnexion (5 minutes)

### Test 16 : Commande QUIT
```bash
# Client rejoint un canal puis quit
echo -e "PASS password123\r\nNICK quitter\r\nUSER quitter 0 * :Quitter\r\nJOIN #goodbye\r\nPRIVMSG #goodbye :I'm leaving\r\nQUIT :Thanks for everything!\r\n" | nc localhost 6667
```
**✅ Attendu** : 
- Message QUIT diffusé aux autres membres
- Client déconnecté proprement
- Pas de crash du serveur

### Test 17 : Déconnexion brutale
```bash
# Connecter puis tuer brutalement
echo -e "PASS password123\r\nNICK brutal\r\nUSER brutal 0 * :Brutal\r\nJOIN #test\r\n" | nc localhost 6667 &
PID=$!
sleep 2
kill -9 $PID
```
**✅ Attendu** : Serveur détecte la déconnexion et nettoie les ressources

---

## 🧪 Tests de Robustesse (10 minutes)

### Test 18 : Commandes invalides
```bash
echo -e "PASS password123\r\nNICK tester\r\nUSER tester 0 * :Tester\r\nINVALIDCOMMAND param\r\nJOIN\r\nKICK\r\nMODE\r\n" | nc localhost 6667
```
**✅ Attendu** : Gestion gracieuse des erreurs, pas de crash

### Test 19 : Paramètres manquants
```bash
echo -e "PASS password123\r\nNICK param_test\r\nUSER param_test 0 * :Param Test\r\nJOIN\r\nPRIVMSG\r\nKICK #test\r\nINVITE user\r\n" | nc localhost 6667
```
**✅ Attendu** : Erreurs 461 (Not enough parameters)

### Test 20 : Messages longs
```bash
echo -e "PASS password123\r\nNICK long_test\r\nUSER long 0 * :Long Test\r\nJOIN #test\r\nPRIVMSG #test :$(python3 -c 'print("A" * 500)')\r\n" | nc localhost 6667
```
**✅ Attendu** : Gestion correcte des messages longs

### Test 21 : Caractères spéciaux
```bash
echo -e "PASS password123\r\nNICK special\r\nUSER special 0 * :Special Test\r\nJOIN #test\r\nPRIVMSG #test :Message with émojis 🚀 and spéciål chars!\r\n" | nc localhost 6667
```
**✅ Attendu** : Support des caractères Unicode

---

## 🔄 Tests Multi-clients (10 minutes)

### Test 22 : Interaction entre plusieurs clients
```bash
# Script automatisé pour tester les interactions
#!/bin/bash
# Terminal 1 - Observer
echo -e "PASS password123\r\nNICK observer\r\nUSER observer 0 * :Observer\r\nJOIN #interaction\r\n" | nc localhost 6667 &

# Terminal 2 - User1
echo -e "PASS password123\r\nNICK user1\r\nUSER user1 0 * :User1\r\nJOIN #interaction\r\nPRIVMSG #interaction :Hello from user1\r\n" | nc localhost 6667 &

# Terminal 3 - User2
echo -e "PASS password123\r\nNICK user2\r\nUSER user2 0 * :User2\r\nJOIN #interaction\r\nPRIVMSG #interaction :Hello from user2\r\n" | nc localhost 6667 &
```

### Test 23 : Charge multiple connexions
```bash
# Test de charge basique
for i in {1..20}; do
    echo -e "PASS password123\r\nNICK stress$i\r\nUSER stress$i 0 * :Stress $i\r\nJOIN #stress\r\nPRIVMSG #stress :Message from $i\r\nQUIT\r\n" | nc localhost 6667 &
done
```
**✅ Attendu** : Serveur gère toutes les connexions sans crash

---

## ⚙️ Tests Techniques (5 minutes)

### Test 24 : Vérification mémoire
```bash
# Avant les tests
ps aux | grep ft_irc | grep -v grep

# Après plusieurs connexions/déconnexions
# Vérifier que l'usage mémoire reste stable
```

### Test 25 : Performance réseau
```bash
# Vérifier les connexions actives
netstat -an | grep :6667

# Pas de connexions orphelines après les tests
```

---

## 📊 Critères d'Évaluation

### ✅ Fonctionnalités Obligatoires (60%)
- [ ] **Authentification** : PASS, NICK, USER fonctionnels
- [ ] **Canaux** : JOIN, PART, création automatique
- [ ] **Messages** : PRIVMSG vers canaux et utilisateurs
- [ ] **Opérateurs** : KICK, INVITE, TOPIC, MODE
- [ ] **Modes** : +i, +t, +k, +o, +l implémentés
- [ ] **Déconnexion** : QUIT propre

### ✅ Qualité Technique (25%)
- [ ] **Compilation** : Sans erreurs ni warnings
- [ ] **C++98** : Conformité stricte au standard
- [ ] **I/O non-bloquant** : poll() utilisé correctement
- [ ] **Pas de forking** : Architecture single-threaded
- [ ] **Gestion mémoire** : Pas de fuites détectées

### ✅ Robustesse (15%)
- [ ] **Gestion d'erreurs** : Codes IRC appropriés
- [ ] **Validation entrées** : Paramètres vérifiés
- [ ] **Stabilité** : Pas de crash sur entrées invalides
- [ ] **Déconnexions** : Gestion des coupures réseau
- [ ] **Limites** : Protection contre les abus

---

## 🚨 Points d'Attention Critique

### ❌ Échecs Automatiques
1. **Compilation échoue** avec make
2. **Crash du serveur** sur une commande basique
3. **Fonctionnalité obligatoire manquante** (KICK, INVITE, MODE...)
4. **Utilisation de fork()** ou threads
5. **Non-conformité C++98** (auto, nullptr, etc.)

### ⚠️ Points de Vigilance
1. **Messages dupliqués** (JOIN, KICK, TOPIC, MODE apparaissent 2x)
2. **Case sensitivity** incorrecte pour les nicknames
3. **Trailing parameters** mal gérés dans les messages
4. **Permissions** non vérifiées pour les opérateurs
5. **Nettoyage** incomplet à la déconnexion

---

## 🕐 Planning de Correction (60-75 minutes)

### Phase 1 : Setup et Tests Basiques (15 min)
- Compilation et démarrage
- Tests d'authentification
- Premier JOIN et message

### Phase 2 : Fonctionnalités Canaux (20 min)
- Tests des canaux multi-utilisateurs
- Messages privés et de canal
- Commande TOPIC

### Phase 3 : Modes et Opérateurs (20 min)
- Tests des 5 modes obligatoires
- KICK, INVITE fonctionnels
- Attribution de privilèges

### Phase 4 : Robustesse et Edge Cases (10 min)
- Commande QUIT
- Gestion d'erreurs
- Tests de charge basiques

### Phase 5 : Questions et Validation (10 min)
- Discussion du code
- Vérification de la compréhension
- Points bonus éventuels

---

## 💡 Questions Techniques Suggérées

### Architecture
- "Expliquez l'utilisation de poll() dans votre serveur"
- "Comment gérez-vous les déconnexions brutales ?"
- "Pourquoi avoir choisi cette architecture modulaire ?"

### Protocole IRC
- "Comment parsez-vous les trailing parameters ?"
- "Quelle est la différence entre les modes +i et +k ?"
- "Comment assurez-vous l'unicité des nicknames ?"

### C++98
- "Pourquoi utiliser des iterators plutôt que les boucles range-based ?"
- "Comment gérez-vous la mémoire sans smart pointers ?"
- "Quelles sont les contraintes spécifiques du C++98 ?"

---

## 🎯 Barème Indicatif

| Composant | Points | Détail |
|-----------|--------|--------|
| **Compilation** | 5 | Make sans erreurs |
| **Authentification** | 15 | PASS/NICK/USER complets |
| **Canaux Basiques** | 15 | JOIN/PART/PRIVMSG |
| **Modes Canaux** | 20 | +i,+t,+k,+o,+l fonctionnels |
| **Opérateurs** | 15 | KICK/INVITE/TOPIC |
| **Robustesse** | 10 | Gestion erreurs, QUIT |
| **Qualité Code** | 10 | C++98, architecture, mémoire |
| **Bonus** | 10 | Fonctionnalités extra, optimisations |
| **Total** | 100 | |

---

**Guide préparé par l'équipe pédagogique**  
**École 42 - Version 2025**  
**Durée estimée : 60-75 minutes**
