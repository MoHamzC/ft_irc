#!/bin/bash

# Test QUIT dans un environnement multi-clients

PORT=6667
PASSWORD="testpass"

echo "🧪 Test QUIT multi-clients"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Démarrer le serveur
./ft_irc $PORT $PASSWORD &
SERVER_PID=$!
sleep 2

echo "📝 Démarrage de 3 clients dans le même canal..."

# Client 1 - Observer
{
    echo "PASS $PASSWORD"
    echo "NICK observer"
    echo "USER observer 0 * :Observer"
    echo "JOIN #test"
    echo "PRIVMSG #test :Je suis l'observateur"
    sleep 10  # Rester connecté pour observer les QUIT
} | nc localhost $PORT &
CLIENT1_PID=$!

sleep 1

# Client 2 - Va quitter avec raison
{
    echo "PASS $PASSWORD"
    echo "NICK quitter1"
    echo "USER quitter1 0 * :Quitter 1"
    echo "JOIN #test"
    echo "PRIVMSG #test :Je vais quitter dans 3 secondes"
    sleep 3
    echo "QUIT :Au revoir les amis!"
} | nc localhost $PORT &
CLIENT2_PID=$!

sleep 1

# Client 3 - Va quitter sans raison
{
    echo "PASS $PASSWORD"
    echo "NICK quitter2"
    echo "USER quitter2 0 * :Quitter 2"
    echo "JOIN #test"
    echo "PRIVMSG #test :Moi aussi je pars"
    sleep 5
    echo "QUIT"
} | nc localhost $PORT &
CLIENT3_PID=$!

# Attendre que les clients quittent
sleep 8

echo ""
echo "📊 Connexions restantes:"
netstat -an | grep :$PORT | grep ESTABLISHED

# Nettoyer
kill $CLIENT1_PID $CLIENT2_PID $CLIENT3_PID 2>/dev/null
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "🏁 Test multi-clients terminé"
