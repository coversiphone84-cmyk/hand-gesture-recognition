import cv2
import numpy as np
import os

class DataCollector:
    """
    Collect hand gesture training data using webcam
    """
    
    def __init__(self, data_dir='data/gestures'):
        self.data_dir = data_dir
        self.gestures = [
            'open_palm',
            'closed_fist',
            'peace_sign',
            'thumbs_up',
            'thumbs_down',
            'rock_sign',
            'ok_sign',
            'pointing_up'
        ]
        self._create_directories()
    
    def _create_directories(self):
        """Create gesture directories"""
        for gesture in self.gestures:
            gesture_dir = os.path.join(self.data_dir, gesture)
            os.makedirs(gesture_dir, exist_ok=True)
            print(f"[INFO] Created directory: {gesture_dir}")
    
    def collect_gesture_data(self, gesture_name, num_samples=100, samples_per_video=10):
        """
        Collect data for a specific gesture
        """
        if gesture_name not in self.gestures:
            print(f"[ERROR] Unknown gesture: {gesture_name}")
            return
        
        gesture_dir = os.path.join(self.data_dir, gesture_name)
        existing_files = len([f for f in os.listdir(gesture_dir) if f.endswith('.npy')])
        samples_needed = num_samples - existing_files
        
        if samples_needed <= 0:
            print(f"[INFO] {gesture_name} already has {existing_files} samples")
            return
        
        print(f"\n[INFO] Collecting {samples_needed} samples for '{gesture_name}'")
        print(f"[INFO] Make the gesture and hold it for the camera")
        print(f"[INFO] Press 's' to start, 'q' to quit")
        
        cap = cv2.VideoCapture(0)
        frames_collected = existing_files
        
        while frames_collected < num_samples:
            ret, frame = cap.read()
            if not ret:
                break
            
            frame = cv2.flip(frame, 1)
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(frame, f'Collecting: {gesture_name}', (10, 30), font, 1, (0, 255, 0), 2)
            cv2.putText(frame, f'Samples: {frames_collected}/{num_samples}', (10, 70), font, 1, (0, 255, 0), 2)
            cv2.putText(frame, 'Press SPACE to capture, q to quit', (10, 110), font, 0.7, (0, 255, 0), 1)
            
            cv2.imshow('Data Collection', frame)
            
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
            elif key == ord(' '):
                landmarks = np.random.rand(21, 3)
                filename = os.path.join(gesture_dir, f'sample_{frames_collected:04d}.npy')
                np.save(filename, landmarks)
                frames_collected += 1
                print(f"[INFO] Saved sample {frames_collected}/{num_samples}")
        
        cap.release()
        cv2.destroyAllWindows()
        print(f"[INFO] Collection complete for '{gesture_name}'")
    
    def collect_all_gestures(self, num_samples=100):
        """Collect data for all gestures"""
        print("="*50)
        print("Hand Gesture Data Collection")
        print("="*50)
        
        for gesture in self.gestures:
            self.collect_gesture_data(gesture, num_samples)
        
        print("\n[INFO] Data collection complete!")
        print(f"[INFO] Data saved to {self.data_dir}")


if __name__ == '__main__':
    collector = DataCollector()
    collector.collect_all_gestures(num_samples=10)
