#include "socket_client.h"
#include <iostream>
#include <cstring>
#include <chrono>
#include <iomanip>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define closesocket close
#endif

using namespace std;

SocketClient::SocketClient(const string& ip, int port)
    : socket_fd(-1), server_ip(ip), server_port(port), connected(false) {
}

SocketClient::~SocketClient() {
    disconnect();
}

bool SocketClient::connect() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (socket_fd < 0) {
        cerr << "[ERROR] Failed to create socket" << endl;
        return false;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    if (inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        cerr << "[ERROR] Invalid IP address: " << server_ip << endl;
        closesocket(socket_fd);
        return false;
    }
    
    if (::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "[ERROR] Failed to connect to server " << server_ip << ":" 
             << server_port << endl;
        closesocket(socket_fd);
        return false;
    }
    
    connected = true;
    cout << "[INFO] Connected to Python server at " << server_ip << ":" 
         << server_port << endl;
    
    return true;
}

bool SocketClient::send_handshake() {
    if (!connected) {
        cerr << "[ERROR] Not connected to server" << endl;
        return false;
    }
    
    Json::Value msg;
    msg["type"] = "hello";
    msg["version"] = "1.0";
    msg["client_id"] = "cpp_gesture_app";
    
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    double timestamp = chrono::duration<double>(duration).count();
    msg["timestamp"] = timestamp;
    
    Json::FastWriter writer;
    string data = writer.write(msg);
    
    return send_data(data);
}

bool SocketClient::send_frame_data(int frame_id,
                                   double timestamp,
                                   bool hand_detected,
                                   const vector<vector<float>>& landmarks) {
    if (!connected) {
        cerr << "[ERROR] Not connected to server" << endl;
        return false;
    }
    
    Json::Value msg;
    msg["type"] = "frame_data";
    msg["frame_id"] = frame_id;
    msg["timestamp"] = timestamp;
    msg["hand_detected"] = hand_detected;
    msg["num_hands"] = hand_detected ? 1 : 0;
    
    if (hand_detected) {
        Json::Value hands(Json::arrayValue);
        Json::Value hand;
        hand["hand_id"] = 0;
        hand["handedness"] = "Right";
        
        Json::Value json_landmarks(Json::arrayValue);
        for (const auto& landmark : landmarks) {
            Json::Value json_point(Json::arrayValue);
            for (float val : landmark) {
                json_point.append(val);
            }
            json_landmarks.append(json_point);
        }
        
        hand["landmarks"] = json_landmarks;
        hands.append(hand);
        msg["hands"] = hands;
    }
    
    msg["frame_size"]["width"] = 640;
    msg["frame_size"]["height"] = 480;
    
    Json::FastWriter writer;
    string data = writer.write(msg);
    
    return send_data(data);
}

Json::Value SocketClient::receive_prediction() {
    string response = receive_data();
    
    Json::Value root;
    Json::CharReaderBuilder reader;
    string errs;
    
    istringstream s(response);
    if (!Json::parseFromStream(reader, s, &root, &errs)) {
        cerr << "[ERROR] JSON parse error: " << errs << endl;
    }
    
    return root;
}

bool SocketClient::send_heartbeat() {
    if (!connected) {
        return false;
    }
    
    Json::Value msg;
    msg["type"] = "ping";
    
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    double timestamp = chrono::duration<double>(duration).count();
    msg["timestamp"] = timestamp;
    
    Json::FastWriter writer;
    string data = writer.write(msg);
    
    return send_data(data);
}

bool SocketClient::send_data(const string& data) {
    if (socket_fd < 0) {
        return false;
    }
    
    int bytes_sent = send(socket_fd, data.c_str(), data.length(), 0);
    
    if (bytes_sent < 0) {
        cerr << "[ERROR] Failed to send data" << endl;
        connected = false;
        return false;
    }
    
    return true;
}

string SocketClient::receive_data(int max_bytes) {
    if (socket_fd < 0) {
        return "";
    }
    
    char buffer[max_bytes + 1];
    memset(buffer, 0, sizeof(buffer));
    
    int bytes_received = recv(socket_fd, buffer, max_bytes, 0);
    
    if (bytes_received <= 0) {
        cerr << "[ERROR] Failed to receive data or connection closed" << endl;
        connected = false;
        return "";
    }
    
    return string(buffer, bytes_received);
}

void SocketClient::disconnect() {
    if (socket_fd >= 0) {
        closesocket(socket_fd);
        socket_fd = -1;
    }
    connected = false;
    cout << "[INFO] Disconnected from server" << endl;
}
