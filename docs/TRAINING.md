# Training Guide - Hand Gesture Recognition

## Training Pipeline Overview

```
Data Collection
    ↓
Data Preprocessing & Augmentation
    ↓
Model Training
    ↓
Model Evaluation
    ↓
Model Deployment
```

---

## Step 1: Data Collection

### Quick Start

```bash
cd python
python data_collection.py
```

### Process

1. Camera will open for each gesture
2. Make the gesture and hold it
3. Press SPACE to capture frame
4. Repeat until desired samples collected
5. Press 'Q' to move to next gesture

### Configuration

Edit `data_collection.py`:

```python
collector = DataCollector(data_dir='data/gestures')
collector.collect_all_gestures(num_samples=100)
```

**Recommendations:**
- Collect 100-200 samples per gesture for good results
- Vary lighting conditions
- Vary hand positions and angles
- Include both hands if possible

---

## Step 2: Data Preprocessing

### Run Preprocessing

```bash
python preprocess_data.py
```

### What It Does

1. Loads raw gesture data
2. Normalizes landmarks to 0-1 range
3. Calculates feature vectors (distances, angles)
4. Augments data (rotation, noise, zoom)
5. Splits into train/val/test sets (70/15/15)
6. Saves processed data to data/processed/

### Output Files

```
data/processed/
├── X_train.npy
├── y_train.npy
├── X_val.npy
├── y_val.npy
├── X_test.npy
├── y_test.npy
├── scaler.pkl
└── gesture_labels.json
```

---

## Step 3: Model Training

### Run Training

```bash
python train_model.py
```

### Training Configuration

```python
model.train(
    X_train, y_train,
    X_val, y_val,
    epochs=50,
    batch_size=32
)
```

### Model Architecture

Conv1D → BatchNorm → MaxPool → Dropout → ...
Dense layers with dropout → Output softmax

---

## Step 4: Model Evaluation

After training, review metrics:
- Accuracy
- Precision, Recall, F1-Score
- Confusion Matrix

---

## Complete Pipeline

```bash
# 1. Collect data
python data_collection.py

# 2. Preprocess data
python preprocess_data.py

# 3. Train model
python train_model.py
```

**Total time:** 50-90 minutes (one-time setup)

---

## Troubleshooting

### Low Accuracy (< 80%)

- Collect more data (200-300 samples per gesture)
- Improve data diversity (lighting, angles, distances)
- Increase augmentation factor
- Train for more epochs

### Model Overfitting

- Increase dropout rates
- Increase data augmentation
- Collect more validation data
- Reduce model complexity

### Out of Memory

- Reduce batch size: 32 → 16 or 8
- Reduce model size
- Collect fewer samples

---

## Next Steps

1. Trained model saved to models/gesture_model.h5
2. Run real-time recognition
3. Improve model by collecting more data
4. Deploy to production

Your model is ready for real-time gesture recognition!
