#include "Server.hpp"


int main(int argc, char* argv[])
 {
    if (argc != 3) 
	{
        std::cerr << PURPLE << "Usage: " << argv[0] << " <port> <password>" << RESET << std::endl;
        return (1);
    }
    
    int port = atoi(argv[1]);
    std::string password = argv[2];
    
    if (port < 1024 || port > 65535)  // modifation por reserved before 1024
	{
        std::cerr << "Invalid port number" << std::endl;
        return (1);
    }
    
    if (password.empty()) 
	{
        std::cerr << RED << "Password cannot be empty" << RESET << std::endl;
        return 1;
    }
    
    Server server(port, password);
    
	try 
	{
		server.start();
	} 
	catch (const std::runtime_error& e) 
	{
		std::cerr << RED << "Server initialization error: " << RESET << e.what() << std::endl;
		return (1);
	}
    std::cout << GREEN << "Server started" << RESET << std::endl;
    std::cout << GREEN << "Use Ctrl+C to stop" << RESET << std::endl;

    try 
	{
        server.run();
    } 
	catch (const std::exception& e) 
	{
        std::cerr << "Server error: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}



// #include "Server.hpp"

// int main(int ac, char* av[]) 
// {
//     if (ac != 3)
// 	{
//         std::cerr << "error, must be : ./ircserv <port> <password>" << std::endl;
//         return (1);
//     }

//     int port = atoi(av[1]);
//     std::string password = av[2];

//     try 
// 	{
//         Server server(port, password);
//         server.run();
//     } 
// 	catch (std::exception& e) 
// 	{
//         std::cerr << "server error: " << e.what() << std::endl;
//         return 1;
//     }
//     return (0);
// }
