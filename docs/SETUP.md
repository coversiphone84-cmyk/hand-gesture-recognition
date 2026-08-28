# Setup Guide - Hand Gesture Recognition

## 📋 Prerequisites

### System Requirements
- **OS**: Ubuntu 20.04+ / Windows 10+ / macOS
- **RAM**: 4GB minimum (8GB recommended)
- **CPU**: Intel i5 or equivalent
- **Webcam**: USB camera or built-in

### Software Requirements
- Python 3.8 or higher
- C++ compiler (g++, clang, MSVC)
- CMake 3.15+
- Git

## 🔧 Installation Steps

### Step 1: Clone Repository

```bash
git clone https://github.com/yourusername/hand-gesture-recognition.git
cd hand-gesture-recognition
```

### Step 2: Setup Python Environment

#### Option A: Using Virtual Environment (Recommended)

```bash
# Create virtual environment
python3 -m venv venv

# Activate virtual environment
# On Linux/macOS:
source venv/bin/activate

# On Windows:
venv\Scripts\activate

# Install dependencies
cd python
pip install -r requirements.txt
cd ..
```

#### Option B: Using Anaconda

```bash
conda create -n gesture_recognition python=3.9
conda activate gesture_recognition
cd python
pip install -r requirements.txt
cd ..
```

### Step 3: Install C++ Dependencies

#### On Ubuntu/Debian:

```bash
# Install OpenCV
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake git libgtk-3-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev

# Install OpenCV from source (or use pre-built)
cd /tmp
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc)
sudo make install
cd ../../../
```

#### On macOS:

```bash
# Using Homebrew
brew install cmake
brew install opencv
```

#### On Windows:

```bash
# Using vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install opencv:x64-windows
```

### Step 4: Build C++ Project

```bash
cd cpp
mkdir build
cd build
cmake ..
make
cd ../../
```

### Step 5: Verify Installation

**Test Python Setup:**
```bash
python3 -c "import cv2; import mediapipe; import tensorflow; print('All Python packages installed!')"
```

**Test C++ Setup:**
```bash
cd cpp/build
./gesture_recognition --version
```

## 📦 Python Requirements

File: `python/requirements.txt`

```
numpy==1.24.3
pandas==2.0.3
opencv-python==4.8.0.76
mediapipe==0.10.1
tensorflow==2.13.0
keras==2.13.0
scikit-learn==1.3.0
matplotlib==3.7.2
pillow==10.0.0
tqdm==4.66.1
jupyter==1.0.0
ipython==8.14.0
```

## 🚀 Running the Project

### Step 1: Start Python Server

**Terminal 1:**
```bash
cd python
# Activate virtual environment (if using)
source venv/bin/activate  # or venv\Scripts\activate on Windows

# Run socket server
python socket_server.py

# Expected output:
# [INFO] Socket server started on localhost:5000
# [INFO] Waiting for C++ client connection...
```

### Step 2: Start C++ Application

**Terminal 2:**
```bash
cd cpp/build
./gesture_recognition

# Expected output:
# [INFO] Opening webcam...
# [INFO] Connected to Python server
# [INFO] Starting gesture recognition...
```

### Step 3: Test Gestures

Show different hand gestures to the camera:
- ✋ Open Palm
- ✊ Closed Fist
- ☮️ Peace Sign
- 👍 Thumbs Up
- And more!

The system will display recognized gestures with confidence scores.

## 📊 Training Custom Model

### Step 1: Collect Data

```bash
cd python
python data_collection.py
```

Follow on-screen instructions to capture gesture data.

### Step 2: Preprocess Data

```bash
python preprocess_data.py
```

This will:
- Normalize data
- Apply augmentation
- Create train/test split

### Step 3: Train Model

```bash
python train_model.py --epochs 50 --batch-size 32
```

### Step 4: Evaluate Model

```bash
python evaluate_model.py
```

## 🐛 Troubleshooting

### Issue: OpenCV not found
**Solution:**
```bash
pip install opencv-python
```

### Issue: Camera not detected
**Solution:**
- Check camera permissions: `ls -la /dev/video*`
- Try different camera index in code (0, 1, 2, etc.)

### Issue: Socket connection failed
**Solution:**
- Ensure Python server is running first
- Check firewall settings
- Verify port 5000 is not in use: `lsof -i :5000`

### Issue: Low accuracy
**Solution:**
- Collect more training data
- Improve lighting conditions
- Retrain model with more epochs
- Increase data augmentation

## 📝 Configuration Files

### C++ Config: `cpp/config.h`
```cpp
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define CAMERA_INDEX 0
#define FPS 30
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
```

### Python Config: `python/config.py`
```python
SERVER_PORT = 5000
MODEL_PATH = 'models/gesture_model.h5'
CONFIDENCE_THRESHOLD = 0.7
NUM_GESTURES = 8
MAX_HANDS = 2
```

## ✅ Verification Checklist

- [ ] Python 3.8+ installed
- [ ] C++ compiler installed
- [ ] OpenCV compiled/installed
- [ ] Virtual environment created and activated
- [ ] Python packages installed
- [ ] C++ project built successfully
- [ ] Python server starts without errors
- [ ] C++ application connects to server
- [ ] Webcam detects hand
- [ ] Gestures recognized

## 🎓 Next Steps

1. Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand the system design
2. Check [API.md](API.md) for communication protocol
3. Start [data collection](../python/data_collection.py)
4. Train your own [custom model](../python/train_model.py)

---

**Happy Coding! 🚀**
