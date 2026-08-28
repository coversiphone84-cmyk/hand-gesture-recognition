#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <string>
#include <vector>
#include <json/json.h>

class SocketClient {
private:
    int socket_fd;
    std::string server_ip;
    int server_port;
    bool connected;

public:
    // Constructor
    SocketClient(const std::string& ip = "127.0.0.1", int port = 5000);
    
    // Destructor
    ~SocketClient();
    
    // Connect to server
    bool connect();
    
    // Send handshake message
    bool send_handshake();
    
    // Send frame data
    bool send_frame_data(int frame_id, 
                         double timestamp,
                         bool hand_detected,
                         const std::vector<std::vector<float>>& landmarks);
    
    // Receive prediction
    Json::Value receive_prediction();
    
    // Send heartbeat
    bool send_heartbeat();
    
    // Send data
    bool send_data(const std::string& data);
    
    // Receive data
    std::string receive_data(int max_bytes = 4096);
    
    // Check if connected
    bool is_connected() const { return connected; }
    
    // Disconnect
    void disconnect();
};

#endif // SOCKET_CLIENT_H
