#include "gesture_detector.h"
#include <iostream>
#include <ctime>
#include <chrono>
#include <algorithm>

using namespace cv;
using namespace std;

GestureDetector::GestureDetector(int cam_index, int width, int height, int fps_val)
    : camera_index(cam_index), frame_width(width), frame_height(height), 
      fps(fps_val), frame_counter(0) {
}

GestureDetector::~GestureDetector() {
    release_camera();
}

bool GestureDetector::initialize_camera() {
    camera.open(camera_index);
    
    if (!camera.isOpened()) {
        cerr << "[ERROR] Failed to open camera at index " << camera_index << endl;
        return false;
    }
    
    // Set camera properties
    camera.set(cv::CAP_PROP_FRAME_WIDTH, frame_width);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height);
    camera.set(cv::CAP_PROP_FPS, fps);
    camera.set(cv::CAP_PROP_BUFFERSIZE, 1);  // Reduce buffer
    
    cout << "[INFO] Camera initialized successfully" << endl;
    cout << "[INFO] Resolution: " << frame_width << "x" << frame_height << endl;
    cout << "[INFO] FPS: " << fps << endl;
    
    return true;
}

Mat GestureDetector::capture_frame() {
    Mat frame;
    camera >> frame;
    
    if (frame.empty()) {
        cerr << "[ERROR] Failed to capture frame" << endl;
        return frame;
    }
    
    frame_counter++;
    return frame;
}

bool GestureDetector::detect_hand(const Mat& frame, Rect& hand_roi) {
    if (frame.empty()) {
        return false;
    }
    
    Mat hsv, mask;
    cvtColor(frame, hsv, COLOR_BGR2HSV);
    
    // Skin color range in HSV
    Scalar lower_skin = Scalar(0, 20, 70);
    Scalar upper_skin = Scalar(20, 255, 255);
    
    inRange(hsv, lower_skin, upper_skin, mask);
    
    // Morphological operations
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    morphologyEx(mask, mask, MORPH_OPEN, kernel, Point(-1, -1), 2);
    morphologyEx(mask, mask, MORPH_CLOSE, kernel, Point(-1, -1), 2);
    
    // Find contours
    vector<vector<Point>> contours;
    findContours(mask.clone(), contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
    if (contours.empty()) {
        return false;
    }
    
    // Find largest contour (hand)
    int largest_idx = 0;
    double largest_area = 0;
    
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > largest_area && area > 500) {  // Minimum hand area
            largest_area = area;
            largest_idx = i;
        }
    }
    
    if (largest_area < 500) {
        return false;
    }
    
    // Get bounding rectangle
    hand_roi = boundingRect(contours[largest_idx]);
    
    // Add padding
    int padding = 20;
    hand_roi.x = max(0, hand_roi.x - padding);
    hand_roi.y = max(0, hand_roi.y - padding);
    hand_roi.width = min(frame.cols - hand_roi.x, hand_roi.width + 2 * padding);
    hand_roi.height = min(frame.rows - hand_roi.y, hand_roi.height + 2 * padding);
    
    return true;
}

Json::Value GestureDetector::landmarks_to_json(const vector<vector<float>>& landmarks) {
    Json::Value json_landmarks(Json::arrayValue);
    
    for (const auto& landmark : landmarks) {
        Json::Value json_point(Json::arrayValue);
        for (float val : landmark) {
            json_point.append(val);
        }
        json_landmarks.append(json_point);
    }
    
    return json_landmarks;
}

Mat GestureDetector::preprocess_frame(const Mat& frame) {
    Mat processed;
    resize(frame, processed, Size(224, 224));
    processed.convertTo(processed, CV_32F, 1.0 / 255.0);
    return processed;
}

void GestureDetector::draw_landmarks(Mat& frame, const vector<vector<float>>& landmarks) {
    if (landmarks.empty()) {
        return;
    }
    
    int height = frame.rows;
    int width = frame.cols;
    
    // Draw landmarks as circles
    for (const auto& landmark : landmarks) {
        if (landmark.size() >= 3) {
            float x = landmark[0] * width;
            float y = landmark[1] * height;
            float confidence = landmark[3];
            
            // Color based on confidence
            Scalar color = confidence > 0.7 ? Scalar(0, 255, 0) : Scalar(0, 165, 255);
            circle(frame, Point(x, y), 5, color, -1);
        }
    }
    
    // Draw connections between landmarks (optional)
    // This would require knowing the finger structure
}

void GestureDetector::draw_label(Mat& frame, const string& gesture, float confidence) {
    // Convert confidence to percentage
    int confidence_percent = static_cast<int>(confidence * 100);
    
    // Create label string
    string label = gesture + " (" + to_string(confidence_percent) + "%)";
    
    // Draw background rectangle
    int font_face = FONT_HERSHEY_SIMPLEX;
    double font_scale = 1.0;
    int font_thickness = 2;
    
    Size text_size = getTextSize(label, font_face, font_scale, font_thickness, nullptr);
    
    int x = 10;
    int y = 40;
    int padding = 10;
    
    rectangle(frame, Point(x - padding, y - text_size.height - padding),
              Point(x + text_size.width + padding, y + padding),
              Scalar(0, 0, 0), -1);
    
    // Draw text
    putText(frame, label, Point(x, y), font_face, font_scale, 
            Scalar(0, 255, 0), font_thickness);
}

void GestureDetector::release_camera() {
    if (camera.isOpened()) {
        camera.release();
        cout << "[INFO] Camera released" << endl;
    }
}
