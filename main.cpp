#include "Server.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>

// Couleurs pour l'affichage
#define RESET       "\033[0m"
#define GREEN       "\033[32m"
#define RED         "\033[31m"
#define BLUE        "\033[34m"
#define YELLOW      "\033[33m"
#define PURPLE      "\033[35m"

// Variable globale pour gérer l'arrêt propre
Server* g_server = NULL;

void signalHandler(int signal) {
    (void)signal;
    std::cout << std::endl << YELLOW << "Shutting down server..." << RESET << std::endl;
    if (g_server) {
        // Le serveur s'arrêtera à la prochaine itération de poll()
        std::cout << YELLOW << "Press Ctrl+C again to force quit" << RESET << std::endl;
    }
    exit(0);
}

void setupSignalHandlers() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN); // Ignorer SIGPIPE
}

void printUsage(const char* programName) {
    std::cout << PURPLE << "Usage: " << programName << " <port> <password>" << RESET << std::endl;
    std::cout << "  port:     Port number (1024-65535)" << std::endl;
    std::cout << "  password: Server password for authentication" << std::endl;
}

bool validatePort(int port) {
    return port >= 1024 && port <= 65535;
}

bool validatePassword(const std::string& password) {
    return !password.empty() && password.find(' ') == std::string::npos;
}

int main(int argc, char* argv[]) {
    // Vérifier les arguments
    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    // Parser les arguments
    int port = std::atoi(argv[1]);
    std::string password = argv[2];
    
    // Validation
    if (!validatePort(port)) {
        std::cerr << RED << "Error: Invalid port number. Must be between 1024 and 65535." << RESET << std::endl;
        return 1;
    }
    
    if (!validatePassword(password)) {
        std::cerr << RED << "Error: Invalid password. Cannot be empty or contain spaces." << RESET << std::endl;
        return 1;
    }
    
    // Configuration des signaux
    setupSignalHandlers();
    
    try {
        // Créer et démarrer le serveur
        Server server(port, password);
        g_server = &server; // Pour le signal handler
        
        std::cout << BLUE << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
        std::cout << BLUE << "                 FT_IRC SERVER" << RESET << std::endl;
        std::cout << BLUE << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
        std::cout << "Port:     " << GREEN << port << RESET << std::endl;
        std::cout << "Password: " << GREEN << "[SET]" << RESET << std::endl;
        std::cout << BLUE << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << RESET << std::endl;
        
        // Démarrer le serveur
        server.start();
        
        std::cout << GREEN << "✅ Server is ready for connections!" << RESET << std::endl;
        std::cout << YELLOW << "Use Ctrl+C to stop the server" << RESET << std::endl;
        std::cout << std::endl;
        
        // Boucle principale
        server.run();
        
        g_server = NULL;
        
    } catch (const std::runtime_error& e) {
        std::cerr << RED << "Server error: " << e.what() << RESET << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << RED << "Unexpected error: " << e.what() << RESET << std::endl;
        return 1;
    }
    
    std::cout << GREEN << "Server stopped successfully." << RESET << std::endl;
    return 0;
}
