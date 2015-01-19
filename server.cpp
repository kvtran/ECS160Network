#include <iostream>
#include <sstream>
#include <string>
#include <zmq.hpp>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#endif

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    
    socket.bind("tcp://*:160");
    std::cout << "Listening on port 160" << std::endl;
    
    while (1) {
        try {
            zmq::message_t request;
            socket.recv(&request);

            std::istringstream iss(static_cast<char*>(request.data()));
            std::cout << "Received data from client: " << iss.str() << std::endl;

            if (iss.str() == "Hello") {
                zmq::message_t reply(6);
                memcpy((void*) reply.data(), "World!", 6);
                std::cout << "Sending World! to client... " << std::endl;
                socket.send(reply);
            }
        }
        
        catch (const std::exception& e){
            std::cout << e.what() << std::endl;;
            exit(1);
        }
        
#ifndef _WIN32
        sleep(1);
#else
    Sleep (1);
#endif
        
    }
    return 0;
}