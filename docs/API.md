# API Documentation - Hand Gesture Recognition

## 🔌 Socket Communication Protocol

### Connection Details

**Protocol:** TCP/IP Sockets  
**Host:** 127.0.0.1 (localhost)  
**Port:** 5000  
**Format:** JSON over TCP  

### Connection Handshake

```
C++ Client                          Python Server
    │
    ├──── [TCP Connect to 127.0.0.1:5000]
    │
    ├──────────────────────────────────> [Accept Connection]
    │
    ├──── [Send: {"type": "hello", "version": "1.0"}]
    │
    │<──────────────────────────────────  [ACK]
    │
    └──── Ready for gesture data
```

---

## 📨 Message Formats

### 1. Handshake Message

**C++ → Python (Initial Connection)**

```json
{
  "type": "hello",
  "version": "1.0",
  "client_id": "cpp_gesture_app",
  "timestamp": 1693564800.123
}
```

**Response from Python**

```json
{
  "type": "ack",
  "status": "ready",
  "server_version": "1.0",
  "timestamp": 1693564800.125
}
```

---

### 2. Frame Data Message (C++ → Python)

**Sent for each video frame**

```json
{
  "type": "frame_data",
  "frame_id": 1,
  "timestamp": 1693564800.150,
  "hand_detected": true,
  "num_hands": 1,
  "hands": [
    {
      "hand_id": 0,
      "handedness": "Right",
      "landmarks": [
        [0.5, 0.5, 0.0, 0.99],    # x, y, z, confidence
        [0.51, 0.48, 0.01, 0.98],
        [0.52, 0.46, 0.02, 0.97],
        [0.53, 0.44, 0.03, 0.96],
        [0.54, 0.42, 0.04, 0.95],
        [0.55, 0.40, 0.05, 0.94],
        [0.56, 0.38, 0.06, 0.93],
        [0.57, 0.36, 0.07, 0.92],
        [0.58, 0.34, 0.08, 0.91],
        [0.59, 0.32, 0.09, 0.90],
        [0.60, 0.30, 0.10, 0.89],
        [0.61, 0.28, 0.11, 0.88],
        [0.62, 0.26, 0.12, 0.87],
        [0.63, 0.24, 0.13, 0.86],
        [0.64, 0.22, 0.14, 0.85],
        [0.65, 0.20, 0.15, 0.84],
        [0.66, 0.18, 0.16, 0.83],
        [0.67, 0.16, 0.17, 0.82],
        [0.68, 0.14, 0.18, 0.81],
        [0.69, 0.12, 0.19, 0.80],
        [0.70, 0.10, 0.20, 0.79]
      ]
    }
  ],
  "frame_size": {
    "width": 640,
    "height": 480
  }
}
```

**Field Descriptions:**
- `frame_id`: Unique frame counter (increments with each frame)
- `timestamp`: Unix timestamp when frame was captured
- `hand_detected`: Boolean, true if hand found in frame
- `num_hands`: Number of hands detected (1 or 2)
- `landmarks`: 21 keypoints per hand (MediaPipe format)
  - Format: [x, y, z, confidence]
  - x, y: Normalized coordinates (0.0 to 1.0)
  - z: Depth value (relative)
  - confidence: Detection confidence (0.0 to 1.0)

---

### 3. Gesture Prediction Response (Python → C++)

**Response with recognition result**

```json
{
  "type": "prediction",
  "frame_id": 1,
  "timestamp": 1693564800.200,
  "gesture": "open_palm",
  "confidence": 0.95,
  "top_3_predictions": [
    {
      "gesture": "open_palm",
      "probability": 0.95
    },
    {
      "gesture": "peace_sign",
      "probability": 0.03
    },
    {
      "gesture": "thumbs_up",
      "probability": 0.02
    }
  ],
  "processing_time_ms": 48,
  "landmarks": [
    [0.5, 0.5],
    [0.51, 0.48],
    [0.52, 0.46],
    ...
  ]
}
```

**Field Descriptions:**
- `gesture`: Predicted gesture name
- `confidence`: Confidence score (0.0 to 1.0)
- `top_3_predictions`: Alternative predictions ranked by probability
- `processing_time_ms`: Time taken for prediction
- `landmarks`: 21 normalized hand keypoints

---

### 4. Error Message

**When an error occurs**

```json
{
  "type": "error",
  "error_code": 500,
  "error_message": "Model inference failed",
  "timestamp": 1693564800.250,
  "frame_id": 5
}
```

**Error Codes:**
| Code | Meaning |
|------|----------|
| 400 | Bad request format |
| 404 | Model not found |
| 500 | Model inference error |
| 503 | Server overloaded |

---

### 5. Heartbeat/Ping Message

**Keep-alive message**

```json
{
  "type": "ping",
  "timestamp": 1693564800.300
}
```

**Response from other side**

```json
{
  "type": "pong",
  "timestamp": 1693564800.302
}
```

---

## 🔄 Message Flow Example

### Complete Gesture Recognition Cycle

```
[1] C++ Client connects
    └─> {"type": "hello", ...}
        <─ {"type": "ack", ...}

[2] C++ sends frame 1
    └─> {"type": "frame_data", "frame_id": 1, "landmarks": [...], ...}
        <─ {"type": "prediction", "gesture": "open_palm", "confidence": 0.95}

[3] C++ sends frame 2
    └─> {"type": "frame_data", "frame_id": 2, "landmarks": [...], ...}
        <─ {"type": "prediction", "gesture": "open_palm", "confidence": 0.92}

[4] C++ sends frame 3 (no hand detected)
    └─> {"type": "frame_data", "frame_id": 3, "hand_detected": false}
        <─ {"type": "prediction", "gesture": "no_gesture", "confidence": 1.0}

[5] C++ sends frame 4
    └─> {"type": "frame_data", "frame_id": 4, "landmarks": [...], ...}
        <─ {"type": "prediction", "gesture": "peace_sign", "confidence": 0.89}

[6] Periodic heartbeat
    └─> {"type": "ping", ...}
        <─ {"type": "pong", ...}
```

---

## 📋 Gesture Types

| Gesture ID | Gesture Name | Description |
|-----------|--------------|-------------|
| 0 | `open_palm` | All fingers extended, palm facing camera |
| 1 | `closed_fist` | All fingers closed |
| 2 | `peace_sign` | Index and middle finger extended |
| 3 | `thumbs_up` | Thumb pointing up, fist closed |
| 4 | `thumbs_down` | Thumb pointing down, fist closed |
| 5 | `rock_sign` | Index and pinky extended |
| 6 | `ok_sign` | Thumb and index forming circle |
| 7 | `pointing_up` | Index finger pointing up |

---

## 🔧 Implementation Examples

### C++ Client Example

```cpp
#include <iostream>
#include <nlohmann/json.hpp>
#include <socket.h>

using json = nlohmann::json;

class GestureClient {
private:
    int socket_fd;
    const char* SERVER_IP = "127.0.0.1";
    int SERVER_PORT = 5000;

public:
    void connect() {
        // Create socket and connect to server
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        // ... connection logic ...
    }

    void send_frame_data(const std::vector<std::vector<float>>& landmarks) {
        json msg;
        msg["type"] = "frame_data";
        msg["frame_id"] = frame_counter++;
        msg["timestamp"] = get_timestamp();
        msg["hand_detected"] = true;
        msg["landmarks"] = landmarks;
        
        std::string data = msg.dump();
        send(socket_fd, data.c_str(), data.length(), 0);
    }

    json receive_prediction() {
        char buffer[4096];
        int bytes = recv(socket_fd, buffer, sizeof(buffer), 0);
        std::string response(buffer, bytes);
        return json::parse(response);
    }
};
```

### Python Server Example

```python
import socket
import json
from threading import Thread

class GestureServer:
    def __init__(self, port=5000):
        self.port = port
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    def start(self):
        self.server_socket.bind(('127.0.0.1', self.port))
        self.server_socket.listen(1)
        print(f"[INFO] Server listening on 127.0.0.1:{self.port}")
        
        client_socket, address = self.server_socket.accept()
        print(f"[INFO] Client connected from {address}")
        
        while True:
            data = client_socket.recv(4096).decode('utf-8')
            if not data:
                break
            
            msg = json.loads(data)
            
            if msg['type'] == 'frame_data':
                gesture = self.predict_gesture(msg['landmarks'])
                response = {
                    'type': 'prediction',
                    'gesture': gesture['name'],
                    'confidence': gesture['confidence'],
                    'frame_id': msg['frame_id']
                }
                client_socket.send(json.dumps(response).encode())
    
    def predict_gesture(self, landmarks):
        # Your ML model prediction logic
        return {'name': 'open_palm', 'confidence': 0.95}
```

---

## 🧪 Testing the API

### Using netcat (nc)

```bash
# Terminal 1: Start Python server
python socket_server.py

# Terminal 2: Connect and send test message
nc 127.0.0.1 5000
{"type": "hello", "version": "1.0"}
```

### Using Python Client Test

```python
import socket
import json

socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket_fd.connect(('127.0.0.1', 5000))

# Send hello
hello_msg = json.dumps({"type": "hello", "version": "1.0"})
socket_fd.send(hello_msg.encode())

# Receive ack
response = socket_fd.recv(1024).decode()
print("Response:", response)

socket_fd.close()
```

---

## 📊 Performance Constraints

- **Max Frame Rate:** 30 fps
- **Max Latency:** 100ms (socket + processing)
- **Max Hands:** 2
- **Confidence Threshold:** 0.5 (configurable)
- **Max Message Size:** 16KB

---

## 🔐 Security Notes

1. Currently uses plain JSON over TCP (no encryption)
2. For production, consider:
   - SSL/TLS encryption
   - Authentication tokens
   - Rate limiting
   - Input validation

---

**For implementation details, see [SETUP.md](SETUP.md) and [ARCHITECTURE.md](ARCHITECTURE.md)**
