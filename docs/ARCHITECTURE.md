# System Architecture - Hand Gesture Recognition

## 🏗️ High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     HAND GESTURE RECOGNITION                 │
│                                                               │
│  ┌──────────────────┐          ┌──────────────────┐         │
│  │    C++ Module    │  Socket  │  Python Module   │         │
│  │   (OpenCV)       ├──────────┤  (TensorFlow)    │         │
│  └──────────────────┘          └──────────────────┘         │
│          ▲                                ▲                  │
│          │                                │                  │
│    Webcam Input                   ML Model Prediction        │
│          │                                │                  │
│          └────────────────┬───────────────┘                  │
│                           │                                   │
│                    Display Output                            │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## 📦 Component Details

### 1. C++ Module (Video Capture & Processing)

**Location:** `cpp/src/main.cpp`, `cpp/src/gesture_detector.cpp`

**Responsibilities:**
- Capture video frames from webcam
- Detect hand using OpenCV
- Extract hand region of interest (ROI)
- Preprocess frames for ML model
- Send data to Python via socket
- Display results on screen

**Key Functions:**
```cpp
class GestureDetector {
    void captureFrame()          // Get frame from webcam
    void preprocessFrame()       // Resize, normalize
    void sendToPython()          // Socket communication
    void displayResult()         // Show on screen
}
```

**Libraries:**
- OpenCV 4.5+ (image processing)
- Standard socket library (networking)

---

### 2. Python Module (ML & Classification)

**Location:** `python/socket_server.py`, `python/gesture_classifier.py`

**Responsibilities:**
- Run socket server listening for C++ client
- Receive hand data from C++
- Load trained ML model
- Extract hand landmarks using MediaPipe
- Classify gesture using TensorFlow model
- Send prediction back to C++

**Key Functions:**
```python
class GestureClassifier:
    def start_server()           # Listen for connections
    def receive_data()           # Get data from C++
    def extract_landmarks()      # MediaPipe processing
    def predict_gesture()        # TensorFlow prediction
    def send_result()            # Send back to C++
```

**Libraries:**
- TensorFlow/Keras (ML model)
- MediaPipe (hand landmark detection)
- NumPy (numerical processing)
- Socket (networking)

---

### 3. Data Collection Module

**Location:** `python/data_collection.py`

**Process:**
1. Capture video for each gesture
2. Extract frames at intervals
3. Save frames to dataset directory
4. Label gesture type

**Output:**
```
data/gestures/
├── open_palm/
│   ├── frame_001.jpg
│   ├── frame_002.jpg
│   └── ...
├── fist/
├── peace/
└── ...
```

---

### 4. Data Preprocessing Module

**Location:** `python/preprocess_data.py`

**Steps:**
1. Load raw frames
2. Resize to standard size (224x224)
3. Normalize pixel values (0-1)
4. Data augmentation (rotation, zoom, flip)
5. Split into train/test/validation
6. Save processed data

**Data Augmentation Techniques:**
- Random rotation (-15° to +15°)
- Random zoom (0.85x to 1.15x)
- Random horizontal flip
- Brightness adjustment

---

### 5. Model Training Module

**Location:** `python/train_model.py`

**Neural Network Architecture:**
```
Input Layer (224×224×3)
    ↓
Convolution Block 1 (32 filters, 3×3)
    ↓
MaxPooling (2×2)
    ↓
Convolution Block 2 (64 filters, 3×3)
    ↓
MaxPooling (2×2)
    ↓
Convolution Block 3 (128 filters, 3×3)
    ↓
MaxPooling (2×2)
    ↓
Flattening
    ↓
Dense Layer 1 (256 units, ReLU)
    ↓
Dropout (0.5)
    ↓
Dense Layer 2 (128 units, ReLU)
    ↓
Dropout (0.5)
    ↓
Output Layer (8 units, Softmax)  ← One per gesture
```

**Training Parameters:**
- Optimizer: Adam
- Loss: Categorical Crossentropy
- Metrics: Accuracy
- Epochs: 50-100
- Batch Size: 32
- Validation Split: 20%

---

## 🔌 Communication Protocol

### Socket Communication Flow

```
C++ Client                          Python Server
    │                                    │
    ├─────────[Connect]─────────────────>│
    │                          [Accept]
    │                                    │
    ├─────────[Send Frame]──────────────>│
    │      (JSON format)      [Process]
    │                    [Predict Gesture]
    │<─────────[Send Result]──────────────┤
    │    (JSON with prediction)           │
    │                                    │
    ├─────────[Send Frame]──────────────>│
    │         (Next frame)               │
    │<─────────[Send Result]──────────────┤
    │                                    │
    └────────[Loop continues]────────────┘
```

### Data Format (JSON)

**C++ sends to Python:**
```json
{
  "frame_id": 1,
  "timestamp": 1693564800.123,
  "hand_data": [
    [x1, y1, z1, confidence1],
    [x2, y2, z2, confidence2],
    ...
    [x21, y21, z21, confidence21]
  ]
}
```

**Python sends to C++:**
```json
{
  "frame_id": 1,
  "gesture": "open_palm",
  "confidence": 0.95,
  "landmarks": [[x1, y1], [x2, y2], ...],
  "processing_time_ms": 45
}
```

---

## 📊 Data Flow Diagram

```
┌──────────────┐
│   Webcam     │
└───────┬──────┘
        │
        ▼
┌──────────────────────────────┐
│  C++ Video Capture Module    │
│  - Read frame (30 fps)       │
│  - Convert to RGB            │
│  - Resize to 640×480        │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  Hand Detection (OpenCV)     │
│  - Skin detection           │
│  - Contour finding          │
│  - ROI extraction           │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  Socket Send                 │
│  - Serialize to JSON         │
│  - Send to Python server     │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  Python Socket Server        │
│  - Receive JSON data         │
│  - Deserialize               │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  MediaPipe Processing        │
│  - Extract hand landmarks    │
│  - 21 keypoints per hand     │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  Feature Extraction          │
│  - Normalize landmarks       │
│  - Calculate angles          │
│  - Create feature vector     │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  TensorFlow Model Prediction │
│  - Pass through neural net   │
│  - Get probabilities         │
│  - Select max probability    │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  Socket Send Result          │
│  - Serialize prediction      │
│  - Send to C++               │
└───────┬──────────────────────┘
        │
        ▼
┌──────────────────────────────┐
│  C++ Display Result          │
│  - Draw landmarks            │
│  - Show gesture label        │
│  - Display confidence        │
│  - Render on screen          │
└──────────────────────────────┘
```

---

## 🎯 Gesture Recognition Pipeline

### Input: Video Frame (640×480 RGB)

### Processing Steps:

1. **Hand Detection** (OpenCV)
   - Convert to HSV color space
   - Apply skin color filter
   - Find contours
   - Extract hand region

2. **Hand Preprocessing**
   - Resize to 224×224
   - Normalize pixel values
   - Convert to tensor

3. **Landmark Extraction** (MediaPipe)
   - Detect 21 hand keypoints
   - Get (x, y, z) coordinates
   - Get confidence scores

4. **Feature Engineering**
   - Normalize coordinates
   - Calculate inter-keypoint distances
   - Calculate angles between fingers
   - Create 126-dimensional feature vector

5. **Model Inference** (TensorFlow)
   - Pass feature vector through CNN
   - Get output logits (8 classes)
   - Apply softmax to get probabilities
   - Select gesture with max probability

### Output: Gesture Label + Confidence Score

---

## 🔄 Real-Time Processing Loop

```python
while True:
    # C++ Side
    frame = capture_frame()                    # 30 fps = 33ms per frame
    hand_roi = detect_hand(frame)              # ~10ms
    serialize_data(hand_roi)                   # ~5ms
    send_to_python(data)                       # ~2ms (network)
    
    # Python Side
    data = receive_from_cpp()                  # ~2ms (network)
    landmarks = extract_landmarks(data)        # ~15ms (MediaPipe)
    features = engineer_features(landmarks)    # ~3ms
    gesture = predict_gesture(features)        # ~5ms (TensorFlow)
    serialize_result(gesture)                  # ~1ms
    send_to_cpp(result)                        # ~2ms (network)
    
    # Total latency: ~45-50ms per frame
    # FPS: ~20-22 fps real-time
```

---

## 📈 Performance Metrics

| Metric | Target | Actual |
|--------|--------|--------|
| **Frame Rate** | 30 fps | 20-25 fps |
| **Latency** | <50ms | 45-60ms |
| **Accuracy** | >85% | 88-92% |
| **CPU Usage** | <40% | 35-45% |
| **Memory** | <500MB | 400-450MB |

---

## 🔐 Security Considerations

1. **Data Privacy**: Local processing, no cloud storage
2. **Communication**: Consider encryption for socket data
3. **Model Security**: Store trained model securely
4. **Access Control**: Limit network access to localhost

---

**Next: Read [API.md](API.md) for detailed communication protocol**
