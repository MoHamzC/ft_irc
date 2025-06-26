#!/bin/bash

# Test spécifique pour la commande QUIT

PORT=6667
PASSWORD="testpass"
SERVER="localhost"

echo "🧪 Test de la commande QUIT"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Démarrer le serveur en arrière-plan
./ft_irc $PORT $PASSWORD &
SERVER_PID=$!

# Attendre que le serveur démarre
sleep 2

echo "📝 Test 1: QUIT basique"

# Test QUIT simple
(
    echo "PASS $PASSWORD"
    echo "NICK tester"
    echo "USER tester 0 * :Test User"
    echo "JOIN #test"
    echo "PRIVMSG #test :Hello, I'm about to quit"
    sleep 1
    echo "QUIT :Goodbye everyone"
    sleep 1
) | nc $SERVER $PORT &

sleep 3

echo "📝 Test 2: QUIT avec raison personnalisée"

# Test QUIT avec raison
(
    echo "PASS $PASSWORD"
    echo "NICK quitter"
    echo "USER quitter 0 * :Another Test User"
    echo "JOIN #test"
    echo "PRIVMSG #test :I'm going to quit with a custom reason"
    sleep 1
    echo "QUIT :Going to lunch!"
    sleep 1
) | nc $SERVER $PORT &

sleep 3

echo "📝 Test 3: QUIT sans raison"

# Test QUIT sans raison
(
    echo "PASS $PASSWORD"
    echo "NICK noReason"
    echo "USER noReason 0 * :No Reason User"
    echo "JOIN #test"
    echo "PRIVMSG #test :Quitting without reason"
    sleep 1
    echo "QUIT"
    sleep 1
) | nc $SERVER $PORT &

sleep 3

# Observer les connexions actives
echo "📊 État du serveur après les tests QUIT:"
netstat -an | grep :$PORT | grep ESTABLISHED | wc -l | xargs echo "Connexions actives:"

# Arrêter le serveur
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "🏁 Test QUIT terminé"
echo "Si QUIT fonctionne correctement, vous devriez voir:"
echo "  - Messages de déconnexion dans les logs du serveur"
echo "  - Notifications QUIT aux autres clients du canal"
echo "  - Connexions fermées proprement"
