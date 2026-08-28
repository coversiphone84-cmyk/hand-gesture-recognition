#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <json/json.h>
#include "gesture_detector.h"
#include "socket_client.h"

using namespace std;
using namespace cv;

// Configuration
const int CAMERA_INDEX = 0;
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int FPS = 30;
const string SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 5000;

// Global variables
bool running = true;
string last_gesture = "none";
float last_confidence = 0.0;
Mat current_frame;
Mutex frame_mutex;

void video_capture_thread(GestureDetector& detector, SocketClient& client) {
    cout << "[INFO] Video capture thread started" << endl;
    
    int frame_count = 0;
    
    while (running) {
        // Capture frame
        Mat frame = detector.capture_frame();
        
        if (frame.empty()) {
            cerr << "[ERROR] Failed to capture frame" << endl;
            this_thread::sleep_for(chrono::milliseconds(33));  // 30 fps
            continue;
        }
        
        // Detect hand
        Rect hand_roi;
        bool hand_detected = detector.detect_hand(frame, hand_roi);
        
        // Create dummy landmarks (in real scenario, use MediaPipe)
        vector<vector<float>> landmarks;
        
        if (hand_detected) {
            // Extract hand region
            Mat hand_img = frame(hand_roi);
            
            // Create dummy 21 landmarks (normalized coordinates)
            for (int i = 0; i < 21; i++) {
                vector<float> landmark;
                // Generate random landmarks for demo
                landmark.push_back(0.4f + (i % 5) * 0.1f);  // x
                landmark.push_back(0.4f + (i / 5) * 0.1f);  // y
                landmark.push_back(0.0f);                   // z
                landmark.push_back(0.9f);                   // confidence
                landmarks.push_back(landmark);
            }
        }
        
        // Get current timestamp
        auto now = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        double timestamp = chrono::duration<double>(duration).count();
        
        // Send to Python server
        if (!client.send_frame_data(detector.get_frame_counter(), 
                                    timestamp, 
                                    hand_detected, 
                                    landmarks)) {
            cerr << "[ERROR] Failed to send frame data" << endl;
            running = false;
            break;
        }
        
        // Receive prediction
        Json::Value prediction = client.receive_prediction();
        
        if (!prediction.isMember("gesture")) {
            cerr << "[ERROR] Invalid prediction response" << endl;
            continue;
        }
        
        // Extract gesture and confidence
        {
            lock_guard<mutex> lock(frame_mutex);
            last_gesture = prediction["gesture"].asString();
            last_confidence = prediction["confidence"].asFloat();
            current_frame = frame.clone();
        }
        
        frame_count++;
        
        if (frame_count % 30 == 0) {
            cout << "[INFO] Frame " << frame_count << " | Gesture: " 
                 << last_gesture << " (" << last_confidence << ")" << endl;
        }
        
        // Maintain frame rate
        this_thread::sleep_for(chrono::milliseconds(33));  // 30 fps
    }
    
    cout << "[INFO] Video capture thread stopped" << endl;
}

void display_thread() {
    cout << "[INFO] Display thread started" << endl;
    
    while (running) {
        Mat display_frame;
        string gesture;
        float confidence;
        
        {
            lock_guard<mutex> lock(frame_mutex);
            if (!current_frame.empty()) {
                display_frame = current_frame.clone();
            }
            gesture = last_gesture;
            confidence = last_confidence;
        }
        
        if (!display_frame.empty()) {
            // Draw gesture label
            if (gesture != "none") {
                GestureDetector detector;
                detector.draw_label(display_frame, gesture, confidence);
            }
            
            // Display frame
            imshow("Hand Gesture Recognition", display_frame);
        }
        
        int key = waitKey(1);
        if (key == 27) {  // ESC key
            running = false;
            break;
        }
        
        this_thread::sleep_for(chrono::milliseconds(33));
    }
    
    cout << "[INFO] Display thread stopped" << endl;
    destroyAllWindows();
}

int main() {
    cout << "========================================" << endl;
    cout << "Hand Gesture Recognition System" << endl;
    cout << "C++ + OpenCV + Python + TensorFlow" << endl;
    cout << "========================================" << endl << endl;
    
    // Initialize gesture detector
    GestureDetector detector(CAMERA_INDEX, FRAME_WIDTH, FRAME_HEIGHT, FPS);
    
    if (!detector.initialize_camera()) {
        cerr << "[FATAL] Failed to initialize camera" << endl;
        return 1;
    }
    
    // Initialize socket client
    SocketClient client(SERVER_IP, SERVER_PORT);
    
    cout << "[INFO] Attempting to connect to Python server..." << endl;
    
    // Try to connect with retries
    int retries = 5;
    while (retries > 0 && !client.connect()) {
        cerr << "[WARNING] Connection failed, retrying in 2 seconds..." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        retries--;
    }
    
    if (!client.is_connected()) {
        cerr << "[FATAL] Could not connect to Python server after retries" << endl;
        return 1;
    }
    
    // Send handshake
    cout << "[INFO] Sending handshake message..." << endl;
    if (!client.send_handshake()) {
        cerr << "[FATAL] Failed to send handshake" << endl;
        return 1;
    }
    
    cout << "[INFO] System ready! Press ESC to exit." << endl << endl;
    
    // Start threads
    thread capture_thread(video_capture_thread, ref(detector), ref(client));
    thread display_thread(display_thread);
    
    // Wait for threads to complete
    capture_thread.join();
    display_thread.join();
    
    // Cleanup
    detector.release_camera();
    client.disconnect();
    
    cout << "[INFO] Application closed" << endl;
    
    return 0;
}
