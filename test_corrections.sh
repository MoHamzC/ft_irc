#!/bin/bash

# Script de test pour vérifier les corrections JOIN/KICK et nickname case sensitivity

PORT=6667
PASSWORD="testpass"
SERVER="localhost"

echo "🧪 Démarrage des tests de correction..."

# Démarrer le serveur en arrière-plan
./ft_irc $PORT $PASSWORD &
SERVER_PID=$!

# Attendre que le serveur démarre
sleep 2

echo "📝 Test 1: Vérification que Bob et bob sont considérés comme identiques"

# Test des nicknames avec différentes casses
(
    echo "PASS $PASSWORD"
    echo "NICK Bob"
    echo "USER alice 0 * :Alice User"
    sleep 1
) | nc $SERVER $PORT > /tmp/test1_output.log &

sleep 1

# Essayer de prendre le même nick avec une casse différente
(
    echo "PASS $PASSWORD"  
    echo "NICK bob"
    echo "USER bob 0 * :Bob User"
    sleep 1
) | nc $SERVER $PORT > /tmp/test2_output.log &

sleep 2

echo "📝 Test 2: Vérification des messages JOIN (ne doivent apparaître qu'une fois)"

# Test JOIN sans messages dupliqués
(
    echo "PASS $PASSWORD"
    echo "NICK Charlie"
    echo "USER charlie 0 * :Charlie User"
    echo "JOIN #test"
    sleep 2
) | nc $SERVER $PORT > /tmp/test3_output.log &

sleep 3

# Arrêter le serveur
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "🔍 Analyse des résultats:"

echo "--- Test 1 & 2: Nicknames case sensitivity ---"
if grep -q "433" /tmp/test2_output.log; then
    echo "✅ PASS: Bob et bob sont correctement considérés comme identiques (433 Nickname is already in use)"
else
    echo "❌ FAIL: Les nicknames Bob et bob ne sont pas considérés comme identiques"
fi

echo "--- Test 3: Messages JOIN dupliqués ---"
JOIN_COUNT=$(grep -c "JOIN #test" /tmp/test3_output.log)
if [ "$JOIN_COUNT" -eq 1 ]; then
    echo "✅ PASS: Message JOIN apparaît exactement 1 fois"
elif [ "$JOIN_COUNT" -eq 2 ]; then
    echo "❌ FAIL: Message JOIN apparaît 2 fois (toujours dupliqué)"
elif [ "$JOIN_COUNT" -eq 0 ]; then
    echo "⚠️  WARN: Aucun message JOIN trouvé (problème de connexion?)"
else
    echo "❓ Message JOIN apparaît $JOIN_COUNT fois"
fi

echo ""
echo "📋 Logs détaillés:"
echo "=== Test Bob ==="
cat /tmp/test1_output.log
echo "=== Test bob ==="  
cat /tmp/test2_output.log
echo "=== Test JOIN ==="
cat /tmp/test3_output.log

# Nettoyage
rm -f /tmp/test1_output.log /tmp/test2_output.log /tmp/test3_output.log

echo "🏁 Tests terminés"
