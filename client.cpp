// Sockets stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

// Other
#include <iostream>
#include <string.h>
#include <stdlib.h>

const char * data = "Hello world!";

int main(int argc, char *argv[]) {
	if (argc < 3) {
      std::cout << "USAGE: ./client <server hostname> <port>" << std::endl;
      exit(1);
	}
   
   const char * hostname = argv[1];
   int port = atoi(argv[2]);

	// Create socket
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1) {
      std::cout << "Error getting socket: " << strerror(errno) << std::endl;
		return 1;
	}

   // gethostbyname returns a struct including the network address of
   // the specified host (does a DNS lookup)
   struct hostent * hp = gethostbyname(hostname);
   if(hp == nullptr)
   {
      std::cout << "Unknown host: " << hostname << std::endl;
      return 1; 
   }

	// Connect socket using name specified by command line. */
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	memcpy(reinterpret_cast<char *>(&server.sin_addr),
			reinterpret_cast<char *>(hp->h_addr), hp->h_length);
	server.sin_port = htons(port);

	// Connect to the server
	if (connect(sock, reinterpret_cast<struct sockaddr *>(&server),
			sizeof(server)) == -1) {
      std::cout << "Error connecting to server: " << strerror(errno) << std::endl;
		return 1;
	}

	int total_bytes_sent = 0, bytes_sent = 0;
	int bytes_to_send = strlen(data) + 1;
	
   while (bytes_to_send != 0) {
		// Attempt to send all unsent message bytes remaining
      bytes_sent = send(sock, data + total_bytes_sent, bytes_to_send, 0);
		if (bytes_sent == -1) {
         std::cout << "Error sending message: " << strerror(errno) << std::endl;
			return 1;
		}

		bytes_to_send -= bytes_sent;
	   total_bytes_sent += bytes_sent;
	}

	close(sock);

	return 0;
}

