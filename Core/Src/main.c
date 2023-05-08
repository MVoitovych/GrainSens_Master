/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "gpio.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "commands.h"
#include "stdio.h"
#include "string.h"
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

/* USER CODE BEGIN PV */
uint8_t UART1_buffer[8];
uint8_t UART1_TX_buffer[32];
uint16_t UART2_RX_buffer[8];
uint16_t UART2_TX_buffer[8];
uint8_t UART1_is_transmitted = 1;
uint8_t UART1_is_received = 0;
const uint8_t packetSize = 8;
uint8_t UART2_is_transmitted = 1;
uint8_t UART2_is_received = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void formPacket();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, UART1_buffer, 4);
  HAL_UART_Receive_IT(&huart2, (uint8_t *)UART2_RX_buffer, packetSize);
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

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &huart1) {
    UART1_is_received = 1;

    if (UART1_is_transmitted) {
      sprintf((char *)UART1_TX_buffer,
              (const char *)"was sent %c%c%c%c comand\r\n", UART1_buffer[0],
              UART1_buffer[1], UART1_buffer[2], UART1_buffer[3]);
      formPacket();

      HAL_UART_Transmit_IT(&huart2, (uint8_t *)UART2_TX_buffer, packetSize);
      HAL_UART_Transmit_IT(&huart1, UART1_TX_buffer,
                           strlen((const char *)UART1_TX_buffer));
      UART1_is_received = 0;
      UART1_is_transmitted = 0;
      UART2_is_received = 0;
      UART2_is_transmitted = 0;
    }
    UART_Start_Receive_IT(&huart1, UART1_buffer, 4);
  }

  else if (huart == &huart2) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    UART_Start_Receive_IT(&huart2, (uint8_t *)UART2_RX_buffer, packetSize);
  }
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

  if (huart == &huart1) {
    UART1_is_transmitted = 1;

    if (UART1_is_received) {

      sprintf((char *)UART1_TX_buffer,
              (const char *)"was sent %c%c%c%c comand\r\n", UART1_buffer[0],
              UART1_buffer[1], UART1_buffer[2], UART1_buffer[3]);
      HAL_UART_Transmit_IT(&huart1, UART1_TX_buffer,
                           strlen((const char *)UART1_TX_buffer));
      UART1_is_received = 0;
      UART1_is_transmitted = 0;
      UART2_is_received = 0;
      UART2_is_transmitted = 0;
    }
  }
}

void formPacket() {
  UART2_TX_buffer[0] = 0x0100 | UART1_buffer[0];
  UART2_TX_buffer[1] = UART1_buffer[1];
  UART2_TX_buffer[2] = UART1_buffer[2];
  UART2_TX_buffer[3] = UART1_buffer[3];
  uint32_t Crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)UART2_TX_buffer,
                                   (packetSize / 2 - 2));
  UART2_TX_buffer[4] = (Crc & 0xFF000000) >> 24;
  UART2_TX_buffer[5] = (Crc & 0x00FF0000) >> 16;
  UART2_TX_buffer[6] = (Crc & 0x0000FF00) >> 8;
  UART2_TX_buffer[7] = (Crc & 0x000000FF);
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
