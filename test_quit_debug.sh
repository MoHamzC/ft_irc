#!/bin/bash

# Test QUIT avec debugging amélioré

PORT=6667
PASSWORD="testpass"
SERVER="localhost"

echo "🧪 Test QUIT avec debugging détaillé"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Démarrer le serveur en arrière-plan
./ft_irc $PORT $PASSWORD &
SERVER_PID=$!

# Attendre que le serveur démarre
sleep 2

echo "📝 Test 1: QUIT avec raison explicite (:Au revoir)"

# Test QUIT avec debugging
{
    echo "PASS $PASSWORD"
    echo "NICK debugger"
    echo "USER debugger 0 * :Debug User"
    echo "JOIN #debug"
    echo "PRIVMSG #debug :Je vais faire QUIT"
    sleep 2
    echo "QUIT :Au revoir tout le monde"
    sleep 3
} | nc -v $SERVER $PORT 2>&1 | while IFS= read -r line; do
    echo "CLIENT: $line"
done &

sleep 5

echo ""
echo "📝 Test 2: QUIT sans paramètre"

{
    echo "PASS $PASSWORD"
    echo "NICK simple"
    echo "USER simple 0 * :Simple User"
    echo "JOIN #debug"
    echo "PRIVMSG #debug :QUIT sans paramètre"
    sleep 2
    echo "QUIT"
    sleep 3
} | nc -v $SERVER $PORT 2>&1 | while IFS= read -r line; do
    echo "CLIENT2: $line"
done &

sleep 5

echo ""
echo "📝 Vérification des connexions restantes:"
netstat -an | grep :$PORT | grep ESTABLISHED

# Arrêter le serveur
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "🏁 Test terminé"
