import numpy as np
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers
import os

class GestureModel:
    """
    Neural Network Model for Hand Gesture Recognition
    """
    
    def __init__(self, input_shape, num_classes=8):
        self.input_shape = input_shape
        self.num_classes = num_classes
        self.model = None
        self.history = None
    
    def build_model(self):
        """Build CNN model for gesture recognition"""
        print("[INFO] Building neural network model...")
        
        model = keras.Sequential([
            layers.Input(shape=(self.input_shape,)),
            layers.Reshape((self.input_shape, 1)),
            layers.Conv1D(32, 3, activation='relu', padding='same'),
            layers.BatchNormalization(),
            layers.MaxPooling1D(2),
            layers.Dropout(0.3),
            layers.Conv1D(64, 3, activation='relu', padding='same'),
            layers.BatchNormalization(),
            layers.MaxPooling1D(2),
            layers.Dropout(0.3),
            layers.Conv1D(128, 3, activation='relu', padding='same'),
            layers.BatchNormalization(),
            layers.MaxPooling1D(2),
            layers.Dropout(0.3),
            layers.GlobalAveragePooling1D(),
            layers.Dense(256, activation='relu'),
            layers.BatchNormalization(),
            layers.Dropout(0.4),
            layers.Dense(128, activation='relu'),
            layers.BatchNormalization(),
            layers.Dropout(0.4),
            layers.Dense(self.num_classes, activation='softmax')
        ])
        
        self.model = model
        print("[INFO] Model built successfully")
    
    def compile_model(self):
        """Compile the model"""
        self.model.compile(
            optimizer=keras.optimizers.Adam(learning_rate=0.001),
            loss='sparse_categorical_crossentropy',
            metrics=['accuracy']
        )
        print("[INFO] Model compiled")
    
    def save_model(self, model_dir='models'):
        """Save trained model"""
        os.makedirs(model_dir, exist_ok=True)
        model_path = os.path.join(model_dir, 'gesture_model.h5')
        self.model.save(model_path)
        print(f"[INFO] Model saved to {model_path}")


if __name__ == '__main__':
    print("="*50)
    print("Model Training Module")
    print("="*50)
    print("[INFO] Model training module ready")
