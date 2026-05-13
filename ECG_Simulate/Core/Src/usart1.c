#include "usart1.h"
#include "ecg.h"

UART_HandleTypeDef huart1; /* 串口1硬件句柄 */

static uint8_t usart1_rx_byte;      /* 串口单字节接收缓存 */
static uint8_t usart1_frame[8];     /* 串口协议帧缓存 */
static uint8_t usart1_frame_index;  /* 当前帧写入索引 */

static void USART1_ProcessCommand(uint8_t *buf, uint8_t len); /* 串口协议命令解析函数 */
static uint8_t USART1_IsAsciiDigit(uint8_t ch); /* 判断字符是否为数字 */

void USART1_Init(void)
{
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0}; /* GPIO初始化结构体 */
  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

  usart1_frame_index = 0U;
}

void USART1_Process(void)
{
  if (HAL_UART_Receive(&huart1, &usart1_rx_byte, 1, 1) == HAL_OK)
  {
    if (usart1_rx_byte == 0xEE)
    {
      usart1_frame_index = 0U;
    }

    if (usart1_frame_index < sizeof(usart1_frame))
    {
      usart1_frame[usart1_frame_index++] = usart1_rx_byte;
    }

    if (usart1_rx_byte == 0xFF && usart1_frame_index >= 2U)
    {
      USART1_ProcessCommand(usart1_frame, usart1_frame_index);
      usart1_frame_index = 0U;
    }
  }
}

static void USART1_ProcessCommand(uint8_t *buf, uint8_t len)
{
  if (len == 6U && buf[0] == 0xEE && buf[1] == 0x11 && buf[2] == 0x04 && buf[5] == 0xFF)
  {
    if (buf[3] == 0x0F && buf[4] == 0x00) { ECG_SetEnable(1U); }
    if (buf[3] == 0x00 && buf[4] == 0x00) { ECG_SetEnable(0U); }
  }
  else if ((len == 6U || len == 7U) && buf[0] == 0xEE && buf[1] == 0x21 && buf[len - 1U] == 0xFF)
  {
    uint16_t heart_rate_bpm = 0U; /* 心率参数BPM */
    uint8_t idx = 2U; /* 命令参数起始索引 */

    while (idx < (uint8_t)(len - 1U))
    {
      if (!USART1_IsAsciiDigit(buf[idx])) { return; }
      heart_rate_bpm = (uint16_t)(heart_rate_bpm * 10U + (uint16_t)(buf[idx] - '0'));
      idx++;
    }

    ECG_SetHeartRate(heart_rate_bpm);
  }
  else if (len == 6U && buf[0] == 0xEE && buf[1] == 0x31 && buf[2] == 0x06 && buf[5] == 0xFF)
  {
    if (USART1_IsAsciiDigit(buf[3]) && USART1_IsAsciiDigit(buf[4]))
    {
      uint8_t mode_number = (uint8_t)((buf[3] - '0') * 10U + (buf[4] - '0')); /* 模式编号1~30 */
      if (mode_number >= 1U && mode_number <= 30U)
      {
        ECG_SetState((uint8_t)(mode_number - 1U));
      }
    }
  }
}

static uint8_t USART1_IsAsciiDigit(uint8_t ch)
{
  return (ch >= '0' && ch <= '9') ? 1U : 0U;
}
