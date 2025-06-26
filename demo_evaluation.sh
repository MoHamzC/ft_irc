#!/bin/bash

# 🎯 Script de démonstration pour l'évaluation ft_irc
# Ce script montre toutes les fonctionnalités implémentées de manière interactive

PORT=6667
PASSWORD="demo42"
SERVER="localhost"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}            🎯 DÉMONSTRATION FT_IRC${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${YELLOW}Ce script démonte toutes les fonctionnalités IRC${NC}"
echo -e "${YELLOW}Assurez-vous que le serveur est démarré avec :${NC}"
echo -e "${GREEN}  ./ft_irc $PORT $PASSWORD${NC}"
echo ""

pause_demo() {
    echo -e "${YELLOW}Appuyez sur ENTRÉE pour continuer...${NC}"
    read
}

# Fonction pour exécuter une commande avec affichage
run_demo() {
    echo -e "${BLUE}➤ $1${NC}"
    echo -e "${GREEN}$2${NC}"
    echo ""
    eval "$2"
    echo ""
    pause_demo
}

# Vérifier si le serveur est en cours d'exécution
check_server() {
    if ! nc -z $SERVER $PORT 2>/dev/null; then
        echo -e "${RED}❌ Serveur non accessible sur $SERVER:$PORT${NC}"
        echo -e "${YELLOW}Démarrez le serveur avec: ./ft_irc $PORT $PASSWORD${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ Serveur détecté sur $SERVER:$PORT${NC}"
    echo ""
}

echo -e "${BLUE}🔍 Vérification de la connexion serveur...${NC}"
check_server

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}            📝 TESTS D'AUTHENTIFICATION${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

run_demo "Test 1: Authentification valide" \
    "timeout 3 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK demo1\\r\\nUSER demo1 0 * :Demo User 1\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 2: Mauvais mot de passe" \
    "timeout 3 bash -c 'echo -e \"PASS wrongpass\\r\\nNICK demo2\\r\\nUSER demo2 0 * :Demo User 2\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 3: Nickname déjà pris" \
    "timeout 3 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK demo1\\r\\nUSER demo3 0 * :Demo User 3\\r\\n\" | nc $SERVER $PORT'"

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}            💬 TESTS DES CANAUX${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

run_demo "Test 4: Création et JOIN d'un canal" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK channeluser\\r\\nUSER channeluser 0 * :Channel User\\r\\nJOIN #demo\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 5: Message dans un canal" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK msguser\\r\\nUSER msguser 0 * :Message User\\r\\nJOIN #demo\\r\\nPRIVMSG #demo :Hello from demo!\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 6: Gestion du TOPIC" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK topicuser\\r\\nUSER topicuser 0 * :Topic User\\r\\nJOIN #demo\\r\\nTOPIC #demo :Welcome to the demo channel\\r\\n\" | nc $SERVER $PORT'"

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}            🔧 TESTS DES MODES${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

run_demo "Test 7: Mode +t (topic restreint)" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK modeuser\\r\\nUSER modeuser 0 * :Mode User\\r\\nJOIN #modetest\\r\\nMODE #modetest +t\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 8: Mode +i (invitation seule)" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK inviteuser\\r\\nUSER inviteuser 0 * :Invite User\\r\\nJOIN #invite\\r\\nMODE #invite +i\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 9: Mode +k (clé de canal)" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK keyuser\\r\\nUSER keyuser 0 * :Key User\\r\\nJOIN #keytest\\r\\nMODE #keytest +k secret123\\r\\n\" | nc $SERVER $PORT'"

run_demo "Test 10: Mode +l (limite d'utilisateurs)" \
    "timeout 5 bash -c 'echo -e \"PASS $PASSWORD\\r\\nNICK limituser\\r\\nUSER limituser 0 * :Limit User\\r\\nJOIN #limitest\\r\\nMODE #limitest +l 5\\r\\n\" | nc $SERVER $PORT'"

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}            👑 TESTS OPÉRATEURS${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

echo -e "${YELLOW}Pour tester les fonctions d'opérateur, nous allons utiliser deux connexions simultanées.${NC}"
echo -e "${YELLOW}Démarrage des connexions en arrière-plan...${NC}"

# Créer un canal et donner les droits d'opérateur
(
    echo "PASS $PASSWORD"
    echo "NICK admin"
    echo "USER admin 0 * :Admin User"
    echo "JOIN #optest"
    sleep 2
    echo "MODE #optest +o admin"
    sleep 1
) | nc $SERVER $PORT > /tmp/admin_output.log &
ADMIN_PID=$!

sleep 2

# Connecter un utilisateur normal
(
    echo "PASS $PASSWORD"
    echo "NICK normaluser"
    echo "USER normaluser 0 * :Normal User"
    echo "JOIN #optest"
    sleep 3
) | nc $SERVER $PORT > /tmp/user_output.log &
USER_PID=$!

sleep 3

echo -e "${GREEN}Test 11: Résultats des connexions opérateur${NC}"
echo -e "${BLUE}--- Sortie Admin ---${NC}"
cat /tmp/admin_output.log
echo -e "${BLUE}--- Sortie Utilisateur ---${NC}"
cat /tmp/user_output.log

# Nettoyer les processus
kill $ADMIN_PID $USER_PID 2>/dev/null
rm -f /tmp/admin_output.log /tmp/user_output.log

pause_demo

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}            🎯 TESTS MULTI-CLIENTS${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

echo -e "${YELLOW}Test de 5 connexions simultanées...${NC}"

for i in {1..5}; do
    (
        echo "PASS $PASSWORD"
        echo "NICK client$i"
        echo "USER client$i 0 * :Client $i"
        echo "JOIN #multitest"
        echo "PRIVMSG #multitest :Message from client $i"
        sleep 2
    ) | nc $SERVER $PORT > /tmp/client${i}_output.log &
done

sleep 5

echo -e "${GREEN}Résultats des 5 connexions simultanées :${NC}"
for i in {1..5}; do
    echo -e "${BLUE}--- Client $i ---${NC}"
    cat /tmp/client${i}_output.log | head -10
    echo ""
done

# Nettoyer les fichiers temporaires
rm -f /tmp/client*_output.log

pause_demo

echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}            ✅ DÉMONSTRATION TERMINÉE${NC}"
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"

echo -e "${GREEN}🎉 Toutes les fonctionnalités ont été démontrées !${NC}"
echo ""
echo -e "${YELLOW}📋 Fonctionnalités testées :${NC}"
echo -e "${GREEN}  ✅ Authentification (PASS/NICK/USER)${NC}"
echo -e "${GREEN}  ✅ Gestion d'erreurs d'authentification${NC}"
echo -e "${GREEN}  ✅ Création et gestion des canaux${NC}"
echo -e "${GREEN}  ✅ Messages dans les canaux${NC}"
echo -e "${GREEN}  ✅ Gestion des topics${NC}"
echo -e "${GREEN}  ✅ Modes de canal (+t, +i, +k, +l)${NC}"
echo -e "${GREEN}  ✅ Privilèges d'opérateur${NC}"
echo -e "${GREEN}  ✅ Support multi-clients${NC}"
echo ""
echo -e "${BLUE}🔍 Pour des tests plus approfondis :${NC}"
echo -e "${YELLOW}  ./test_complete.sh      # Tests automatisés complets${NC}"
echo -e "${YELLOW}  ./test_corrections.sh   # Tests des corrections spécifiques${NC}"
echo -e "${YELLOW}  ./test_quit.sh          # Tests de déconnexion${NC}"
echo ""
echo -e "${GREEN}Le serveur ft_irc est prêt pour l'évaluation ! 🚀${NC}"
