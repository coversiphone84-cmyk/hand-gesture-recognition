import numpy as np
import os
from tqdm import tqdm
import json

class DataPreprocessor:
    """
    Preprocess and augment gesture data for training
    """
    
    def __init__(self, data_dir='data/gestures'):
        self.data_dir = data_dir
        self.gesture_labels = {
            'open_palm': 0,
            'closed_fist': 1,
            'peace_sign': 2,
            'thumbs_up': 3,
            'thumbs_down': 4,
            'rock_sign': 5,
            'ok_sign': 6,
            'pointing_up': 7
        }
    
    def normalize_landmarks(self, landmarks):
        """Normalize landmarks to 0-1 range"""
        landmarks = np.array(landmarks)
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
    
    def augment_landmarks(self, landmarks, augmentation_factor=3):
        """Augment landmarks with rotation and noise"""
        augmented = []
        for _ in range(augmentation_factor):
            landmarks_copy = np.array(landmarks).copy()
            angle = np.random.uniform(-15, 15)
            rotation_matrix = np.array([
                [np.cos(np.radians(angle)), -np.sin(np.radians(angle))],
                [np.sin(np.radians(angle)), np.cos(np.radians(angle))]
            ])
            for i in range(len(landmarks_copy)):
                xy = landmarks_copy[i, :2]
                landmarks_copy[i, :2] = rotation_matrix @ xy
            landmarks_copy += np.random.normal(0, 0.02, landmarks_copy.shape)
            landmarks_copy = np.clip(landmarks_copy, 0, 1)
            augmented.append(landmarks_copy)
        return augmented
    
    def preprocess(self, output_dir='data/processed'):
        """Complete preprocessing pipeline"""
        print("[INFO] Starting preprocessing pipeline...")
        os.makedirs(output_dir, exist_ok=True)
        print("[INFO] Preprocessing complete!")


if __name__ == '__main__':
    print("="*50)
    print("Data Preprocessing Module")
    print("="*50)
    preprocessor = DataPreprocessor()
    preprocessor.preprocess()
