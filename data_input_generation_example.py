import numpy as np

n_samples = 1000
time = np.linspace(0, 20, n_samples)

######################################################################
# Generating sin-like movement
acc_clean = np.array([np.sin(time), np.cos(time), np.sin(2 * time)]).T  # Accelerometer (3-axis)
gyro_clean = np.array([np.cos(time), np.sin(2 * time), np.cos(3 * time)]).T  # Gyroscope (3-axis)
mag_clean = np.array([np.sin(3 * time), np.cos(3 * time), np.sin(4 * time)]).T  # Magnetometer (3-axis)

######################################################################
# Inserting drift errors
drift_acc = np.linspace(0, 0.5, n_samples).reshape(-1, 1)  # Drift for accelerometer
drift_gyro = np.linspace(0, 0.2, n_samples).reshape(-1, 1)  # Drift for gyroscope
drift_mag = np.linspace(0, 0.1, n_samples).reshape(-1, 1)  # Drift for magnetometer
acc_raw = acc_clean + drift_acc + np.random.normal(0, 0.05, acc_clean.shape)
gyro_raw = gyro_clean + drift_gyro + np.random.normal(0, 0.02, gyro_clean.shape)
mag_raw = mag_clean + drift_mag + np.random.normal(0, 0.01, mag_clean.shape)


# Combine "clean", "raw" readings  into a single array
clean_movement = np.hstack([acc_clean, gyro_clean, mag_clean])
drift_movement = np.hstack([acc_raw, gyro_raw, mag_raw]) 

