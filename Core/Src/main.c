/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_DMA_Init(void);

static void MX_SPI1_Init(void);

static void MX_SDIO_SD_Init(void);

static void MX_USART3_UART_Init(void);

static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint32_t port_main_memory_read_w(uint32_t offset) {
    /* Dispatch address to different chips */
    uint8_t dispatch = offset >> 23;
    uint8_t scratch;
    uint32_t res = 0;

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_RESET);
            break;
        default:
            return 0xff;
    }

    HAL_SPI_Transmit(&hspi1, (uint8_t *) "\x03", 1, 100);
    scratch = offset >> 16;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    scratch = offset >> 8;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    scratch = offset;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);

    scratch = 0xff;
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t recv;
        HAL_SPI_TransmitReceive(&hspi1, &scratch, &recv, 1, 100);
        res |= recv << (i << 3);
    }

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_SET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_SET);
            break;
        default:
            return 0xff;
    }

    int n = 50;
    while (n) n--;

    return res;
}

void port_main_memory_write_w(uint32_t offset, uint32_t data) {
    /* Dispatch address to different chips */
    uint8_t dispatch = offset >> 23;
    uint8_t scratch;

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_RESET);
            break;
        default:
            return;
    }

    HAL_SPI_Transmit(&hspi1, (uint8_t *) "\x02", 1, 100);
    scratch = offset >> 16;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    scratch = offset >> 8;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    scratch = offset;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);

    for (uint8_t i = 0; i < 4; i++) {
        scratch = data >> (i << 3);
        HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    }

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_SET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_SET);
            break;
        default:
            return;
    }

    int n = 50;
    while (n) n--;
}

void port_main_memory_load_b(uint32_t offset, uint8_t byteData) {
    /* Dispatch address to different chips */
    uint8_t dispatch = offset >> 23;
    uint8_t scratch;

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_RESET);
            break;
        default:
            return;
    }

    HAL_SPI_Transmit(&hspi1, (uint8_t *) "\x02", 1, 100);
    scratch = offset >> 16;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    scratch = offset >> 8;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);
    scratch = offset;
    HAL_SPI_Transmit(&hspi1, &scratch, 1, 100);

    HAL_SPI_Transmit(&hspi1, &byteData, 1, 100);

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_SET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_SET);
            break;
        default:
            return;
    }

    int n = 50;
    while (n) n--;
}

void port_main_memory_load_byte_batch(uint32_t offset, uint8_t *byteData, uint32_t batchSize) {
    /* Dispatch address to different chips */
    uint8_t dispatch = offset >> 23;
    uint8_t scratch;

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_RESET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_RESET);
            break;
        default:
            return;
    }

    uint8_t sendBuffer[] = {0x02, offset >> 16, offset >> 8, offset};
    HAL_SPI_Transmit(&hspi1, sendBuffer, 4, 100);
    HAL_SPI_Transmit(&hspi1, byteData, batchSize, 100);

    switch (dispatch) {
        case 0:
            HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_SET);
            break;
        case 1:
            HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_SET);
            break;
        case 3:
            HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_SET);
            break;
        default:
            return;
    }

    int n = 50;
    while (n) n--;
}

void psramReset() {
    HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_RESET);

    uint8_t sendBuffer[] = {0x66, 0x99};
    HAL_SPI_Transmit(&hspi1, sendBuffer, 2, 100);

    HAL_GPIO_WritePin(PSRAM_CS0_GPIO_Port, PSRAM_CS0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSRAM_CS1_GPIO_Port, PSRAM_CS1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSRAM_CS2_GPIO_Port, PSRAM_CS2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PSRAM_CS3_GPIO_Port, PSRAM_CS3_Pin, GPIO_PIN_SET);
}

void psramTest() {
    uint32_t dutAddr;

    for (dutAddr = 0; dutAddr < 0x10000; dutAddr++) {
        port_main_memory_write_w(dutAddr << 2, dutAddr << 2);
    }

    for (dutAddr = 0; dutAddr < 0x10000; dutAddr++) {
        uint32_t dutData = port_main_memory_read_w(dutAddr << 2);
        if ((dutAddr << 2) != dutData) {
            continue;
        }
    }
}

void fatfsTest() {
    FATFS fatfs;
    FIL file;
    FRESULT fResult;
    BYTE buffer[128];
    UINT byteRead;
    fResult = f_mount(&fatfs, "0:", 1);
    if (fResult != FR_OK) {
        return;
    }

    fResult = f_open(&file, "0:hello.txt", FA_OPEN_EXISTING | FA_READ);
    if (fResult != FR_OK) {
        return;
    }

    while (!f_eof(&file)) {
        f_read(&file, buffer, 128, &byteRead);
    }

    f_close(&file);
}

void port_console_write(uint8_t c) {
    HAL_UART_Transmit(&huart3, &c, 1, 100);
}

int port_console_read(void) {
    return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SPI1_Init();
    MX_SDIO_SD_Init();
    MX_FATFS_Init();
    MX_USART3_UART_Init();
    MX_TIM3_Init();
    /* USER CODE BEGIN 2 */
//    psramTest();
    fatfsTest();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void) {

    /* USER CODE BEGIN SDIO_Init 0 */

    /* USER CODE END SDIO_Init 0 */

    /* USER CODE BEGIN SDIO_Init 1 */

    /* USER CODE END SDIO_Init 1 */
    hsd.Instance = SDIO;
    hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
    hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    hsd.Init.ClockDiv = 3;
    /* USER CODE BEGIN SDIO_Init 2 */

    /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void) {

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void) {

    /* USER CODE BEGIN TIM3_Init 0 */

    /* USER CODE END TIM3_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM3_Init 1 */

    /* USER CODE END TIM3_Init 1 */
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 9 - 1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 8 - 1;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM3_Init 2 */

    /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void) {

    /* USER CODE BEGIN USART3_Init 0 */

    /* USER CODE END USART3_Init 0 */

    /* USER CODE BEGIN USART3_Init 1 */

    /* USER CODE END USART3_Init 1 */
    huart3.Instance = USART3;
    huart3.Init.BaudRate = 115200;
    huart3.Init.WordLength = UART_WORDLENGTH_8B;
    huart3.Init.StopBits = UART_STOPBITS_1;
    huart3.Init.Parity = UART_PARITY_NONE;
    huart3.Init.Mode = UART_MODE_TX_RX;
    huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart3.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart3) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USART3_Init 2 */

    /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) {

    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Channel4_5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel4_5_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel4_5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, PSRAM_CS0_Pin | PSRAM_CS1_Pin | PSRAM_CS2_Pin | PSRAM_CS3_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin : PF14 */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /*Configure GPIO pins : PSRAM_CS0_Pin PSRAM_CS1_Pin PSRAM_CS2_Pin PSRAM_CS3_Pin */
    GPIO_InitStruct.Pin = PSRAM_CS0_Pin | PSRAM_CS1_Pin | PSRAM_CS2_Pin | PSRAM_CS3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
