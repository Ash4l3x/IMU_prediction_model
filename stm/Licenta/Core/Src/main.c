#include "main.h"
#include "adress_defines.h"
#include <stdint.h>
#include <stdio.h>  // for debugging or serial output

#define SENSOR_RIGHT_UPPER 0x29
#define SENSOR_RIGHT_LOWER 0x28

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C2_Init(void);

void Error_Handler(void);

void BNO055_Init(I2C_HandleTypeDef *i2c, uint16_t sensor_address);
void Read_Accelerometer(I2C_HandleTypeDef *i2c, uint16_t sensor_address, int16_t *accel_data);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_I2C2_Init();

    // Initialize the sensors
    BNO055_Init(&hi2c1, SENSOR_RIGHT_LOWER);
    //BNO055_Init(&hi2c2, SENSOR_RIGHT_UPPER);

    int16_t accel_data[3]; // X, Y, Z acceleration data

    while (1) {
        // Read accelerometer data from SENSOR_RIGHT_LOWER
        Read_Accelerometer(&hi2c1, SENSOR_RIGHT_LOWER, accel_data);

        printf("X: %d, Y: %d, Z: %d\n", accel_data[0], accel_data[1], accel_data[2]);

        HAL_Delay(100); // Delay 100ms between reads
    }
}

void BNO055_Init(I2C_HandleTypeDef *i2c, uint16_t sensor_address) {
    uint8_t opr_mode = 0x08; // Accel-only mode
    uint8_t data[2];

    data[0] = BNO055_OPR_MODE_ADDR; // Register for operation mode
    data[1] = opr_mode; // Set to accelerometer mode

    if (HAL_I2C_Master_Transmit(i2c, sensor_address << 1, data, 2, HAL_MAX_DELAY) != HAL_OK) {
        Error_Handler(); // Handle errors
    }

    HAL_Delay(20); // Delay for mode change stabilization
}

void Read_Accelerometer(I2C_HandleTypeDef *i2c, uint16_t sensor_address, int16_t *accel_data) {
    uint8_t accel_reg_start = BNO055_ACCEL_DATA_X_LSB_ADDR; // Starting register
    uint8_t accel_raw[6]; // Store raw bytes of accelerometer data

    if (HAL_I2C_Master_Transmit(i2c, sensor_address << 1, &accel_reg_start, 1, HAL_MAX_DELAY) != HAL_OK) {
        Error_Handler(); // Handle errors
    }

    if (HAL_I2C_Master_Receive(i2c, sensor_address << 1, accel_raw, 6, HAL_MAX_DELAY) != HAL_OK) {
        Error_Handler(); // Handle errors
    }

    // Convert raw data to 16-bit signed integers
    accel_data[0] = (int16_t)((accel_raw[1] << 8) | accel_raw[0]); // X
    accel_data[1] = (int16_t)((accel_raw[3] << 8) | accel_raw[2]); // Y
    accel_data[2] = (int16_t)((accel_raw[5] << 8) | accel_raw[4]); // Z
}


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_I2C2_Init(void) {
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
}
#endif
