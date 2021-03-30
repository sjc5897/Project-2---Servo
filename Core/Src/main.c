/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM15_Init(void);
/* USER CODE BEGIN PFP */
void first_time_servo_init();
void process_recipe();
void start_timer_fifteen();
void stop_timer_fifteen();
void operate_servos();
void process_user_command();

int parse_user_input();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// TEST RECIPES

unsigned char test_wait[] = {MOV + 1, WAIT+31, WAIT + 31, MOV + 5,RECIPE_END};				//Test Recipe for all move pos
unsigned char test_loop_basic[] = {LOOP + 0, MOV+1, WAIT+10, MOV+5, END_LOOP, RECIPE_END};
unsigned char test_loop_actual_loop[] = {LOOP + 2, MOV+1, WAIT+10, MOV+5, END_LOOP, RECIPE_END};
unsigned char test_loop_internal_loop[] = {MOV + 3, LOOP + 2, MOV+1, WAIT+10, LOOP + 4, MOV+5, END_LOOP, RECIPE_END};

//These test recipes were the ones specified in the write up
// Mandatory Test
unsigned char test_recipe_manditory[] = {MOV | 0, MOV | 5, MOV | 0, MOV | 3,
										 LOOP | 0, MOV | 1, MOV | 4, END_LOOP,
										 MOV | 0, MOV | 2, WAIT | 0, MOV | 3,
										 WAIT | 31, WAIT | 31, WAIT | 31, MOV | 4,
										 RECIPE_END};
// Position Test
unsigned char test_positions[] = {MOV |  0, WAIT | 10, MOV | 1, WAIT | 10, MOV | 2,
								  WAIT | 10, MOV | 3, WAIT | 10, MOV | 4, WAIT | 10,
								  MOV | 5, WAIT | 10, MOV | 0,
								  RECIPE_END};

// Continue Override Test
unsigned char test_continue_override[] = {MOV | 2, RECIPE_END, MOV | 5, RECIPE_END};

//Error towards end
unsigned char test_error_towards_end[] = {MOV | 2, LOOP | 2, MOV | 1, MOV | 5, END_LOOP, MOV | 0, MOV | 11, MOV | 3, RECIPE_END};

unsigned char fiazal_test[] = {LOOP+2, MOV+0, MOV+1, MOV+2, MOV+3, MOV+4, MOV+5, END_LOOP, RECIPE_END};
unsigned char fiazal_test_2[] = {MOV+1, WAIT+31, WAIT+31, WAIT+31, MOV+5,RECIPE_END};

//Tests Opcode fan
unsigned char test_fan_one[] = {MOV | 0, FAN | 2, MOV | 5, RECIPE_END};

unsigned char test_fan_two[] = {MOV | 5, FAN | 2, MOV | 0, RECIPE_END};

unsigned char *recipes_a[] = {test_fan_one,NULL};
unsigned char *recipes_b[] = {fiazal_test_2, NULL};



Servo servos[SERVO_N];								//Stores Servos
char command_buffer[SERVO_N + 1];					//Used for user commands
const char *valid_chars = "PpCcLlRrBbNn";


//UART VARS
uint8_t uart_buffer[UART_BUFFER_SIZE];
uint8_t uart_input_buffer[UART_INPUT_BUFFER_SIZE];
uint32_t uart_buffer_size;


int interrupted = 0;							//Interupt flag
int command_index = 0;							//Index wrriting in the command buffer
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  // Notes on below timer uses:
  // TIMER_2: 	Runs at 50hz and is used for PWM on two channels, GPIOA 0 and 1
  // TIMER_15:	Runs at 1kHz, used to generate an interrupt every 100ms for the main loop timing
  // TIMER_16:	Runs at 1kHz, used to time actions for servo1
  // TIMER_17:	Runs at 1kHz, used to time actions for servo2
  // Green_LED:	Set to GPIOE8
  // Red_LED:	Set to GPIOB2

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART2_UART_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */


  // Initialize SERVOS
  uart_buffer_size = sprintf((char *)uart_buffer, "Initializing Servos\r\n");
  uart_transmit(&huart2, uart_buffer, uart_buffer_size);
  first_time_servo_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int to_parse = 0;
  uart_buffer_size = sprintf((char *)uart_buffer,"Input User Command:\r\n>");
  uart_transmit(&huart2, uart_buffer, uart_buffer_size);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	// start timing the loop
	start_timer_fifteen();

	// Parse user input
	to_parse = parse_user_input();

	if(to_parse){
		process_user_command();
		to_parse = 0;
		command_index = 0;
		uart_buffer_size = sprintf((char *)uart_buffer,"\r\n>");
		uart_transmit(&huart2, uart_buffer, uart_buffer_size);
	}
	// Operates on the servos
	operate_servos();

	// Wait for interupt
	while(!interrupted){}

	//Reset loop
	stop_timer_fifteen();
	interrupted = 0;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV16;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 80-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 10000-1;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 100-1;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 10000-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 10000-1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 65535;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Red_LED_GPIO_Port, Red_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Red_LED_Pin */
  GPIO_InitStruct.Pin = Red_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Red_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Green_LED_Pin */
  GPIO_InitStruct.Pin = Green_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Green_LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//Purpose: 	Starts timer15 in interrupt mode for main loop timing
//Input:	None
//Output:	None
void start_timer_fifteen(){
	HAL_TIM_Base_Start_IT(&htim15);
}
//Purpose: 	Stops timer15 in interrupt mode for main loop timing
//Input:	None
//Output:	None
void stop_timer_fifteen(){
	HAL_TIM_Base_Stop_IT(&htim15);
}

//Purpose: 	HAL function that handles the interrupt on timer 15
//Maths:	Timer 15 runs (after all prescalers) at 1kHz. this means a tick every 1ms.
//			The ARR is 100 bits, so an interrupt occurs 1ms * 100 = 100ms
//Input:	TIM_HandleTypeDef, timer that was interrupt.
//Output:	Void
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	// We only care about timer two being interrupted
	if (htim == &htim15){
		// Set flag true
		interrupted = 1;
	}
}

//Purpose: 	Initializes all the servos for the project
//Input:	None
//Output:	Void
void first_time_servo_init(){
	// CONFIG THE time
	int i = 0;
	servos[i] = servo_init(i,htim2,htim16);
	i++;
	servos[i] = servo_init(i,htim2,htim17);
}
//Purpose:	Processes inputed user commands
//Input:	None
//Output:	Void
void process_user_command(){
	for(int i = 0; i < SERVO_N; i++){
		switch(command_buffer[i]){
			//Pause
			case 'P':
			case 'p':
				pause_servo(&servos[i]);
				break;
			//Continue
			case 'C':
			case 'c':
				continue_servo(&servos[i]);
				break;

			case 'L':
			case 'l':
				left_servo(&servos[i]);
				break;
			case 'R':
			case 'r':
				right_servo(&servos[i]);
				break;
			case 'B':
			case 'b':
				begin_servo(&servos[i]);
				break;
		}
	}

}

//Propose: 	Checks if UART has user input
//Input: 	None
//Output:	int, represents if the main loop should process the command or not.

int parse_user_input(){
	char input;

	// Get input
	uart_recieve(&huart2, uart_input_buffer, UART_INPUT_BUFFER_SIZE);
	input = uart_input_buffer[0];
	uart_input_buffer[0] = 0;

	// If enter and we have two commands
	if(input == ASCII_NEWLINE && command_index == SERVO_N){
		//return one to process
		return 1;
	}
	// if x, cancel command and restart
	if(input == 'X' ||input == 'x'){
		// reset the buffer
		uart_buffer_size = sprintf((char *)uart_buffer,"\r\n>");
		uart_transmit(&huart2, uart_buffer, uart_buffer_size);
		command_index = 0;
	}
	// If backspace
	else if(input == ASCII_BACKSPACE){
		if(command_index > 0){
			command_index--;
			uart_buffer_size = sprintf((char *)uart_buffer,"%c",input);
			uart_transmit(&huart2, uart_buffer, uart_buffer_size);
		}
	}
	// If input is a valid charater
	else if(input != 0 && strchr(valid_chars,input)){
		if(command_index < SERVO_N){
			command_buffer[command_index] = input;
			command_index++;
			uart_buffer_size = sprintf((char *)uart_buffer,"%c",input);
			uart_transmit(&huart2, uart_buffer, uart_buffer_size);
		}
	}
	return 0;
}


//Propose:	Operates both servos
//Input:	None
//Output:	None
void operate_servos(){
	// operate each servo
	operate(&servos[0],recipes_a[servos[0].recipe_index]);
	operate(&servos[1],recipes_b[servos[1].recipe_index]);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
