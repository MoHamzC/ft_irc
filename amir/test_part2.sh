#!/bin/bash

# Script de test pour la partie 2 - Gestion des clients et authentification
# Usage: ./test_part2.sh [port] [password]

PORT=${1:-6667}
PASSWORD=${2:-test123}
SERVER_PID=""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_error() {
    echo -e "${RED}[✗]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Function to start server
start_server() {
    print_status "Starting ft_irc server on port $PORT..."
    ./ft_irc $PORT $PASSWORD &
    SERVER_PID=$!
    sleep 2
    
    if kill -0 $SERVER_PID 2>/dev/null; then
        print_success "Server started (PID: $SERVER_PID)"
        return 0
    else
        print_error "Failed to start server"
        return 1
    fi
}

# Function to stop server
stop_server() {
    if [ ! -z "$SERVER_PID" ]; then
        print_status "Stopping server (PID: $SERVER_PID)..."
        kill $SERVER_PID 2>/dev/null
        wait $SERVER_PID 2>/dev/null
        print_success "Server stopped"
    fi
}

# Function to test connection
test_connection() {
    local test_name="$1"
    local commands="$2"
    local expected="$3"
    
    print_status "Testing: $test_name"
    
    # Send commands and capture response
    local response=$(echo -e "$commands" | nc -w 5 localhost $PORT 2>/dev/null)
    
    if echo "$response" | grep -q "$expected"; then
        print_success "$test_name - PASSED"
        return 0
    else
        print_error "$test_name - FAILED"
        echo "Expected: $expected"
        echo "Got: $response"
        return 1
    fi
}

# Function to test authentication sequence
test_auth_sequence() {
    print_status "Testing complete authentication sequence..."
    
    local commands="PASS $PASSWORD\r\nNICK testnick\r\nUSER testuser 0 * :Test User\r\nPING :test\r\nQUIT :Goodbye\r\n"
    local response=$(echo -e "$commands" | nc -w 5 localhost $PORT 2>/dev/null)
    
    # Check for welcome message
    if echo "$response" | grep -q "001.*Welcome"; then
        print_success "Authentication sequence - PASSED"
        return 0
    else
        print_error "Authentication sequence - FAILED"
        echo "Response: $response"
        return 1
    fi
}

# Function to test invalid scenarios
test_invalid_scenarios() {
    print_status "Testing invalid scenarios..."
    
    # Test wrong password
    test_connection "Wrong password" "PASS wrongpass\r\nNICK test\r\n" "464.*Password incorrect"
    
    # Test no nickname
    test_connection "No nickname given" "PASS $PASSWORD\r\nNICK\r\n" "431.*No nickname given"
    
    # Test invalid nickname
    test_connection "Invalid nickname" "PASS $PASSWORD\r\nNICK 123invalid\r\n" "432.*Erroneous nickname"
    
    # Test incomplete registration
    test_connection "Incomplete registration" "PASS $PASSWORD\r\nNICK test\r\nPRIVMSG #test :hello\r\n" "451.*not registered"
}

# Main test function
run_tests() {
    print_status "Starting ft_irc Part 2 Tests"
    echo "=================================="
    
    # Compile the project
    print_status "Compiling project..."
    if make re >/dev/null 2>&1; then
        print_success "Compilation successful"
    else
        print_error "Compilation failed"
        return 1
    fi
    
    # Start server
    if ! start_server; then
        return 1
    fi
    
    # Run tests
    local total_tests=0
    local passed_tests=0
    
    # Test basic connection
    print_status "=== Basic Connection Tests ==="
    if test_connection "Server response" "PING :test\r\n" "PONG"; then
        ((passed_tests++))
    fi
    ((total_tests++))
    
    # Test authentication
    print_status "=== Authentication Tests ==="
    if test_auth_sequence; then
        ((passed_tests++))
    fi
    ((total_tests++))
    
    # Test PASS command
    if test_connection "PASS command" "PASS $PASSWORD\r\nNICK testuser\r\nUSER test 0 * :Test\r\n" "001.*Welcome"; then
        ((passed_tests++))
    fi
    ((total_tests++))
    
    # Test NICK command
    if test_connection "NICK command" "PASS $PASSWORD\r\nNICK validnick\r\nUSER test 0 * :Test\r\n" "001.*Welcome"; then
        ((passed_tests++))
    fi
    ((total_tests++))
    
    # Test USER command
    if test_connection "USER command" "PASS $PASSWORD\r\nNICK testuser2\r\nUSER myuser 0 * :My Real Name\r\n" "001.*Welcome"; then
        ((passed_tests++))
    fi
    ((total_tests++))
    
    # Test invalid scenarios
    print_status "=== Invalid Scenario Tests ==="
    test_invalid_scenarios
    
    # Stop server
    stop_server
    
    # Results
    echo "=================================="
    print_status "Test Results: $passed_tests/$total_tests tests passed"
    
    if [ $passed_tests -eq $total_tests ]; then
        print_success "All tests passed! 🎉"
        return 0
    else
        print_error "Some tests failed 😞"
        return 1
    fi
}

# Cleanup function
cleanup() {
    print_warning "Cleaning up..."
    stop_server
    exit 1
}

# Set trap for cleanup
trap cleanup SIGINT SIGTERM

# Check if ft_irc executable exists
if [ ! -f "./ft_irc" ]; then
    print_error "ft_irc executable not found. Please compile first with 'make'"
    exit 1
fi

# Check if nc (netcat) is available
if ! command -v nc &> /dev/null; then
    print_error "netcat (nc) is required for testing but not installed"
    exit 1
fi

# Run the tests
run_tests
exit_code=$?

# Final cleanup
stop_server

exit $exit_code