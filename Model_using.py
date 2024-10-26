import numpy as np
import joblib
from tensorflow.keras.models import load_model

# Load the model and scaler
model = load_model("model/imu_correction_model.h5")
scaler = joblib.load("model/scaler.joblib")

def preprocess_data(X_raw, scaler, time_steps=10):
    """
    Preprocesses new IMU raw data: normalize and reshape.
    """
    X_raw_scaled = scaler.transform(X_raw)  # Apply the scaler
    X_processed = []

    for i in range(time_steps, len(X_raw_scaled)):
        X_processed.append(X_raw_scaled[i-time_steps:i, :])

    return np.array(X_processed)

# Example new raw data, 100 input lists with 9 values
X_new_raw = np.random.rand(100, 9)  

# Preprocess the new data and predict
X_new_processed = preprocess_data(X_new_raw, scaler)
predictions = model.predict(X_new_processed)

print("Corrected IMU Values:", predictions)
