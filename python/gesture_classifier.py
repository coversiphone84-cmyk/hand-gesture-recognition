import numpy as np
import tensorflow as tf
from tensorflow import keras
import pickle
import json
import os

class GestureClassifier:
    """
    Real-time gesture classification using trained model
    """
    
    def __init__(self, model_path='models/gesture_model.h5', scaler_path='data/processed/scaler.pkl'):
        self.model = None
        self.scaler = None
        self.gesture_labels = {}
        self.gesture_names = [
            'open_palm', 'closed_fist', 'peace_sign', 'thumbs_up',
            'thumbs_down', 'rock_sign', 'ok_sign', 'pointing_up'
        ]
        
        self.load_model(model_path)
        self.load_scaler(scaler_path)
    
    def load_model(self, model_path):
        """Load trained model"""
        if not os.path.exists(model_path):
            print(f"[WARNING] Model not found at {model_path}")
            return
        
        try:
            self.model = keras.models.load_model(model_path)
            print(f"[INFO] Model loaded from {model_path}")
        except Exception as e:
            print(f"[ERROR] Failed to load model: {str(e)}")
    
    def load_scaler(self, scaler_path):
        """Load feature scaler"""
        if not os.path.exists(scaler_path):
            print(f"[WARNING] Scaler not found at {scaler_path}")
            return
        
        try:
            with open(scaler_path, 'rb') as f:
                self.scaler = pickle.load(f)
            print(f"[INFO] Scaler loaded from {scaler_path}")
        except Exception as e:
            print(f"[ERROR] Failed to load scaler: {str(e)}")
    
    def preprocess_landmarks(self, landmarks):
        """Preprocess hand landmarks"""
        landmarks = np.array(landmarks)
        if landmarks.shape[0] > 0:
            landmarks[:, 0] = (landmarks[:, 0] - landmarks[:, 0].min()) / (landmarks[:, 0].max() - landmarks[:, 0].min() + 1e-7)
            landmarks[:, 1] = (landmarks[:, 1] - landmarks[:, 1].min()) / (landmarks[:, 1].max() - landmarks[:, 1].min() + 1e-7)
            landmarks[:, 2] = (landmarks[:, 2] - landmarks[:, 2].min()) / (landmarks[:, 2].max() - landmarks[:, 2].min() + 1e-7)
        return landmarks
    
    def calculate_features(self, landmarks):
        """Calculate feature vector from landmarks"""
        landmarks = np.array(landmarks).flatten()
        distances = []
        for i in range(len(landmarks) // 3):
            for j in range(i + 1, len(landmarks) // 3):
                idx_i = i * 3
                idx_j = j * 3
                dist = np.sqrt((landmarks[idx_i] - landmarks[idx_j])**2 + (landmarks[idx_i+1] - landmarks[idx_j+1])**2)
                distances.append(dist)
        features = np.concatenate([landmarks, distances])
        return features
    
    def predict(self, landmarks, confidence_threshold=0.5):
        """Predict gesture from landmarks"""
        landmarks = self.preprocess_landmarks(landmarks)
        features = self.calculate_features(landmarks)
        features = features.reshape(1, -1)
        
        if self.scaler:
            features = self.scaler.transform(features)
        
        if self.model:
            predictions = self.model.predict(features, verbose=0)
            predicted_class = np.argmax(predictions[0])
            confidence = predictions[0][predicted_class]
            top_3_indices = np.argsort(predictions[0])[-3:][::-1]
            top_3_predictions = [
                {'gesture': self.gesture_names[idx], 'probability': float(predictions[0][idx])}
                for idx in top_3_indices
            ]
        else:
            predicted_class = 0
            confidence = 0.5
            top_3_predictions = []
        
        if confidence < confidence_threshold:
            return {
                'gesture': 'unknown',
                'confidence': 0.0,
                'top_3_predictions': top_3_predictions
            }
        
        return {
            'gesture': self.gesture_names[predicted_class],
            'confidence': float(confidence),
            'top_3_predictions': top_3_predictions
        }


if __name__ == '__main__':
    print("Gesture Classifier Test")
    classifier = GestureClassifier()
    dummy_landmarks = np.random.rand(21, 3)
    result = classifier.predict(dummy_landmarks)
    print(f"Predicted gesture: {result['gesture']}")
    print(f"Confidence: {result['confidence']:.2f}")
