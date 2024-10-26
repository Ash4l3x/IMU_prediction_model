import numpy as np
from sklearn.preprocessing import StandardScaler
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import LSTM, Dense, Dropout, Input
from tensorflow.keras.optimizers import Adam
import joblib

from data_input_generation_example import clean_movement, drift_movement, n_samples

class IMUDriftCorrectionModel:
    def __init__(self, time_steps=10):
        self.time_steps = time_steps  # Number of time steps (sequence length) to look back
        self.model = Sequential()
        self.model.add(Input(shape=(self.time_steps, 9)))  # Specify input shape
        self.model.add(LSTM(128, return_sequences=True))
        self.model.add(Dropout(0.2))
        self.model.add(LSTM(64, return_sequences=False))
        self.model.add(Dropout(0.2))
        self.model.add(Dense(9, activation='linear'))

        self.model.compile(optimizer=Adam(learning_rate=0.001), loss='mean_squared_error')
        self.scaler = StandardScaler()

    def preprocess_data(self, X_raw):
        """
        Preprocess IMU raw data: normalize and create time windows.
        X_raw: shape (n_samples, 9)
        """
        # Normalize the data
        X_raw_scaled = self.scaler.fit_transform(X_raw)

        # Reshape data into time steps for LSTM input: (n_samples - time_steps, time_steps, 9)
        X_processed = []
        for i in range(self.time_steps, len(X_raw_scaled)):
            X_processed.append(X_raw_scaled[i-self.time_steps:i, :])
        
        return np.array(X_processed)

    def train(self, X_raw, y_corrected, epochs=100, batch_size=32):
        """
        Train the model.
        X_raw: raw IMU data with shape (n_samples, 9)
        y_corrected: corrected IMU data with shape (n_samples, 9)
        """
        X_processed = self.preprocess_data(X_raw)
        y_corrected_processed = y_corrected[self.time_steps:]

        print("X_processed shape:", X_processed.shape)
        print("y_corrected_processed shape:", y_corrected_processed.shape)

        # Ensure the shapes match before training
        assert X_processed.shape[0] == y_corrected_processed.shape[0], \
            "Mismatch in number of samples between features and labels."

        # Train the model
        self.model.fit(X_processed, y_corrected_processed, epochs=epochs, batch_size=batch_size)

    def predict(self, X_raw):
        """
        Predict corrected IMU values.
        X_raw: raw IMU data with shape (n_samples, 9)
        Returns: corrected IMU data with shape (n_samples, 9)
        """
        X_processed = self.preprocess_data(X_raw)
        return self.model.predict(X_processed)

# Training the model
imu_model = IMUDriftCorrectionModel(time_steps=10)  # Adjust the time_steps if necessary
imu_model.train(drift_movement, clean_movement, epochs=20)

# Saving the model
imu_model.model.save("model/imu_correction_model.h5")
joblib.dump(imu_model.scaler, "model/scaler.joblib")

# Predicting on new IMU data (Example)
# predictions = imu_model.predict(drift_movement)
# print("Predicted corrected values:", predictions)
