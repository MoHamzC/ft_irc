#!/bin/bash

# Test manuel interactif pour QUIT

PORT=6667
PASSWORD="testpass"

echo "🧪 Test interactif pour QUIT"
echo "Démarrage du serveur..."

# Démarrer le serveur
./ft_irc $PORT $PASSWORD &
SERVER_PID=$!

sleep 2

echo "Vous pouvez maintenant tester manuellement avec:"
echo "  echo -e 'PASS testpass\\r\\nNICK test\\r\\nUSER test 0 * :Test\\r\\nJOIN #test\\r\\nQUIT :Au revoir\\r\\n' | nc localhost $PORT"
echo ""
echo "Appuyez sur Entrée pour arrêter le serveur..."

read

# Arrêter le serveur
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "Serveur arrêté."
