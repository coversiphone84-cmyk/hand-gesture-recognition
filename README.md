# Hand Gesture Recognition System

A comprehensive system that combines **C++**, **Python**, **OpenCV**, **MediaPipe**, and **TensorFlow** to recognize hand gestures in real-time.

## 🎯 Project Overview

This project demonstrates the integration of multiple technologies:
- **C/C++**: Real-time video capture and frame processing using OpenCV
- **Python**: Machine Learning model training and gesture classification
- **MediaPipe**: Hand landmark detection
- **TensorFlow/Keras**: Deep learning model for gesture recognition
- **Socket Communication**: C++ and Python communication via sockets

## 🎬 Supported Gestures

- ✋ Open Hand (Palm)
- ✊ Closed Fist
- ☮️ Peace Sign (Victory)
- 👍 Thumbs Up
- 👎 Thumbs Down
- 🤟 Rock Sign
- 👌 OK Sign
- ☝️ Pointing Up

## 📋 Project Structure

```
hand-gesture-recognition/
├── cpp/                          # C++ components
│   ├── src/
│   │   ├── main.cpp             # Main video capture program
│   │   ├── gesture_detector.cpp  # Hand detection using OpenCV
│   │   └── socket_client.cpp     # Socket communication with Python
│   ├── include/
│   │   ├── gesture_detector.h
│   │   └── socket_client.h
│   └── CMakeLists.txt            # Build configuration
│
├── python/                        # Python components
│   ├── data_collection.py        # Collect gesture training data
│   ├── preprocess_data.py        # Data preprocessing and augmentation
│   ├── train_model.py            # Train ML model
│   ├── gesture_classifier.py     # Gesture classification server
│   ├── socket_server.py          # Socket server for C++ communication
│   ├── requirements.txt          # Python dependencies
│   └── models/                   # Trained models storage
│       └── gesture_model.h5      # Trained Keras model
│
├── data/                          # Training data
│   ├── train/                    # Training dataset
│   ├── test/                     # Testing dataset
│   └── gestures/                 # Raw gesture recordings
│
├── docs/                          # Documentation
│   ├── SETUP.md                  # Setup instructions
│   ├── ARCHITECTURE.md           # System architecture
│   └── API.md                    # Communication API
│
└── README.md                      # This file
```

## 🚀 Quick Start

### Prerequisites

**System Requirements:**
- Ubuntu 20.04+ or Windows 10+
- Webcam/Camera
- 4GB RAM minimum
- Python 3.8+
- C++17 compatible compiler

### Installation

See [SETUP.md](docs/SETUP.md) for detailed installation instructions.

### Quick Run

**Terminal 1 - Start Python Server:**
```bash
cd python
pip install -r requirements.txt
python socket_server.py
```

**Terminal 2 - Run C++ Application:**
```bash
cd cpp
mkdir build && cd build
cmake ..
make
./gesture_recognition
```

## 📊 Project Phases

### Phase 1: Data Collection
- Capture hand gesture videos using webcam
- Extract frames and hand landmarks
- Store dataset for training

### Phase 2: Data Preprocessing
- Clean and normalize data
- Data augmentation (rotation, scaling, etc.)
- Train/test/validation split

### Phase 3: Model Training
- Build neural network in TensorFlow/Keras
- Train on collected gesture data
- Validate and evaluate performance
- Save trained model

### Phase 4: Real-time Recognition
- C++ captures video frames
- MediaPipe extracts hand landmarks
- Python model predicts gesture
- Display results in real-time

## 🔌 Communication Flow

```
Webcam
   ↓
C++ (OpenCV) - Frame Capture & Preprocessing
   ↓
MediaPipe - Hand Landmark Detection
   ↓
Socket → Python Server
   ↓
TensorFlow Model - Gesture Classification
   ↓
Socket ← Result
   ↓
C++ Display - Show Recognition Result
```

## 📈 Expected Accuracy

- Single Gesture: 85-95%
- Real-time Recognition: 80-90%
- Multiple Hands: 75-85%

## 🎓 Learning Outcomes

After completing this project, you'll understand:

✅ Computer Vision with OpenCV  
✅ Hand Detection & Pose Estimation  
✅ Deep Learning with TensorFlow  
✅ C++ and Python Integration  
✅ Real-time Application Development  
✅ Data Collection & Preprocessing  
✅ Model Training & Evaluation  
✅ Socket Programming  

## 📚 Documentation

- [Setup Guide](docs/SETUP.md) - Installation and configuration
- [Architecture](docs/ARCHITECTURE.md) - System design and components
- [API Documentation](docs/API.md) - Communication protocol
- [Training Guide](docs/TRAINING.md) - Model training process

## 🛠️ Technologies Used

| Technology | Purpose |
|-----------|----------|
| **C++17** | Backend video processing |
| **OpenCV 4.5+** | Computer vision library |
| **Python 3.8+** | ML and data processing |
| **MediaPipe** | Hand detection framework |
| **TensorFlow 2.x** | Deep learning framework |
| **Keras** | Neural network API |
| **NumPy** | Numerical computing |
| **Pandas** | Data manipulation |
| **Matplotlib** | Data visualization |
| **Socket Programming** | Inter-process communication |

## 📝 License

MIT License - Feel free to use and modify!

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests.

## 📧 Support

For issues and questions, please create an issue on GitHub.

---

**Happy Coding! 🚀**
