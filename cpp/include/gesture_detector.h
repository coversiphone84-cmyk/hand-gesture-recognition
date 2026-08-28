#ifndef GESTURE_DETECTOR_H
#define GESTURE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <json/json.h>

class GestureDetector {
private:
    cv::VideoCapture camera;
    int camera_index;
    int frame_width;
    int frame_height;
    int fps;
    int frame_counter;

public:
    // Constructor
    GestureDetector(int cam_index = 0, int width = 640, int height = 480, int fps_val = 30);
    
    // Destructor
    ~GestureDetector();
    
    // Initialize camera
    bool initialize_camera();
    
    // Capture and process frame
    cv::Mat capture_frame();
    
    // Detect hand in frame
    bool detect_hand(const cv::Mat& frame, cv::Rect& hand_roi);
    
    // Convert landmarks to JSON format
    Json::Value landmarks_to_json(const std::vector<std::vector<float>>& landmarks);
    
    // Preprocess frame for ML
    cv::Mat preprocess_frame(const cv::Mat& frame);
    
    // Draw landmarks on frame
    void draw_landmarks(cv::Mat& frame, const std::vector<std::vector<float>>& landmarks);
    
    // Draw gesture label on frame
    void draw_label(cv::Mat& frame, const std::string& gesture, float confidence);
    
    // Get frame counter
    int get_frame_counter() const { return frame_counter; }
    
    // Reset frame counter
    void reset_frame_counter() { frame_counter = 0; }
    
    // Release camera
    void release_camera();
};

#endif // GESTURE_DETECTOR_H
