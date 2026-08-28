import socket
import json
import threading
import time
from datetime import datetime
import sys

class SocketServer:
    """
    Python Socket Server for receiving hand gesture data from C++
    and sending back gesture predictions
    """
    
    def __init__(self, host='127.0.0.1', port=5000):
        self.host = host
        self.port = port
        self.server_socket = None
        self.client_socket = None
        self.running = False
        self.connected = False
        self.frame_count = 0
        
    def start(self):
        """Start the socket server"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(1)
            self.running = True
            
            print(f"[INFO] Socket server started on {self.host}:{self.port}")
            print(f"[INFO] Waiting for C++ client connection...")
            
            # Accept client connection
            self.client_socket, client_address = self.server_socket.accept()
            self.connected = True
            print(f"[INFO] Client connected from {client_address}")
            
            # Start receiving data
            self._receive_loop()
            
        except Exception as e:
            print(f"[ERROR] Server error: {str(e)}")
        finally:
            self.stop()
    
    def _receive_loop(self):
        """Main loop to receive and process messages"""
        while self.running and self.connected:
            try:
                # Receive data from C++ client
                data = self.client_socket.recv(4096).decode('utf-8')
                
                if not data:
                    print(f"[WARNING] Client disconnected")
                    self.connected = False
                    break
                
                # Parse JSON message
                try:
                    message = json.loads(data)
                    self._handle_message(message)
                except json.JSONDecodeError as e:
                    print(f"[ERROR] JSON decode error: {str(e)}")
                    
            except socket.timeout:
                continue
            except Exception as e:
                print(f"[ERROR] Receive error: {str(e)}")
                self.connected = False
                break
    
    def _handle_message(self, message):
        """Handle received message based on type"""
        msg_type = message.get('type', 'unknown')
        
        if msg_type == 'hello':
            self._handle_handshake(message)
        elif msg_type == 'frame_data':
            self._handle_frame_data(message)
        elif msg_type == 'ping':
            self._handle_ping(message)
        else:
            print(f"[WARNING] Unknown message type: {msg_type}")
    
    def _handle_handshake(self, message):
        """Handle initial handshake from C++ client"""
        print(f"[INFO] Handshake received from {message.get('client_id')}")
        
        # Send acknowledgment
        response = {
            'type': 'ack',
            'status': 'ready',
            'server_version': '1.0',
            'timestamp': time.time()
        }
        self._send_response(response)
    
    def _handle_frame_data(self, message):
        """Handle frame data from C++ client"""
        frame_id = message.get('frame_id')
        hand_detected = message.get('hand_detected', False)
        
        self.frame_count += 1
        
        if self.frame_count % 30 == 0:
            print(f"[INFO] Received frame {frame_id} | Hand detected: {hand_detected}")
        
        # Generate dummy prediction
        gesture = self._predict_gesture(message)
        
        # Send prediction response
        response = {
            'type': 'prediction',
            'frame_id': frame_id,
            'timestamp': time.time(),
            'gesture': gesture['name'],
            'confidence': gesture['confidence'],
            'top_3_predictions': gesture['top_3'],
            'processing_time_ms': gesture['processing_time'],
            'landmarks': []
        }
        
        self._send_response(response)
    
    def _handle_ping(self, message):
        """Handle heartbeat ping from C++ client"""
        response = {
            'type': 'pong',
            'timestamp': time.time()
        }
        self._send_response(response)
    
    def _predict_gesture(self, frame_data):
        """Predict gesture from frame data"""
        import random
        
        gestures = [
            'open_palm',
            'closed_fist',
            'peace_sign',
            'thumbs_up',
            'thumbs_down',
            'rock_sign',
            'ok_sign',
            'pointing_up'
        ]
        
        hand_detected = frame_data.get('hand_detected', False)
        
        if not hand_detected:
            return {
                'name': 'no_gesture',
                'confidence': 1.0,
                'top_3': [],
                'processing_time': 5
            }
        
        # Random gesture selection
        selected_gesture = random.choice(gestures)
        confidence = random.uniform(0.7, 0.99)
        
        top_3 = [
            {'gesture': selected_gesture, 'probability': confidence},
            {'gesture': random.choice([g for g in gestures if g != selected_gesture]), 
             'probability': random.uniform(0.01, 0.2)},
            {'gesture': random.choice([g for g in gestures if g != selected_gesture]), 
             'probability': random.uniform(0.01, 0.1)}
        ]
        
        return {
            'name': selected_gesture,
            'confidence': confidence,
            'top_3': top_3,
            'processing_time': random.randint(40, 60)
        }
    
    def _send_response(self, response):
        """Send JSON response to C++ client"""
        try:
            data = json.dumps(response)
            self.client_socket.send(data.encode('utf-8'))
        except Exception as e:
            print(f"[ERROR] Failed to send response: {str(e)}")
            self.connected = False
    
    def stop(self):
        """Stop the server"""
        self.running = False
        
        if self.client_socket:
            try:
                self.client_socket.close()
            except:
                pass
        
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
        
        print(f"[INFO] Server stopped")


if __name__ == '__main__':
    print("="*50)
    print("Hand Gesture Recognition - Python Socket Server")
    print("="*50)
    print()
    
    server = SocketServer(host='127.0.0.1', port=5000)
    
    try:
        server.start()
    except KeyboardInterrupt:
        print("\n[INFO] Shutting down server...")
        server.stop()
