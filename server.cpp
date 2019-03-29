// Sockets stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// Other stuff
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cassert>

int main(int argc, char * argv[])
{
   if(argc != 2)
   {
      std::cout << "USAGE: ./server <port>" << std::endl;
      return 1;
   }

   int port = atoi(argv[1]);

   // Create socket
   int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(sockfd == -1)
   {
      std::cout << "Error creating socket: " << strerror(errno) << std::endl;
      return 1;
   }
   
   // Set socket options (setsockopt)
   int on = 1;
   int sockopt_return_val = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
   assert(sockopt_return_val != -1);

   // Create a sockaddr_in struct for the proper port and bind() to it.
   struct sockaddr_in addr;
   socklen_t length = socklen_t(sizeof(addr));
   memset(&addr, 0, length);                   // Zero socket memory
   addr.sin_family = AF_INET;                  // Set socket family
   addr.sin_addr.s_addr = htonl(INADDR_ANY);   // Specify socket address (hostname)
   addr.sin_port = htons(port);                // Set port value

   // Bind to socket
   if(bind(sockfd, (struct sockaddr*) &addr, length) == -1)
   {
      std::cout << "Error binding to socket" << std::endl;
      return 1;
   }

   // Get the assigned port number
   if(getsockname(sockfd, (struct sockaddr*) &addr, &length) == -1)
   {
      std::cout << "Error getting port number" << std::endl;
      return 1;
   }

   // Begin listening for incoming connections.
   if(listen(sockfd, 10) == -1)
   {
      std::cout << "Error listening" << std::endl;
      return 1;
   }

   // Listen forever for connections
   while(true)
   {
      // Accept a connection to a client
      int newconnectionfd = accept(sockfd, (struct sockaddr *) 0, (unsigned *) 0);
      if(newconnectionfd == -1)
      {
         std::cout << "Error accepting connection: " << strerror(errno) << std::endl;
         return 1;
      }

      // Create a buffer for receiving the message
      char buffer[4 * 1024];
      size_t bytes_recvd = 0, total_bytes_recvd = 0;
      memset(buffer, 0, sizeof(buffer));

      // Receive bytes in a loop
      while((bytes_recvd = recv(newconnectionfd, buffer, sizeof(buffer), 0)) > 0) 
      {
         total_bytes_recvd += 1;
      }

      // Print the message received
      std::cout << buffer << std::endl;
   }

   // Close socket
   close(sockfd);
   
   return 0;
}
