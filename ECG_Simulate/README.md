# STM32F750VBT6 HAL 示例（Keil MDK）

支持功能：
- PD9 LED 闪烁指示
- PA4 DAC ECG 模拟输出
- USART1 指令控制 ECG 启停/心率/模式

## 串口1控制协议

串口1功能已拆分到独立文件：`Core/Src/usart1.c` 与 `Core/Inc/usart1.h`。

波特率：`115200,8,N,1`

- ECG输出启动：`EE 11 04 0F 00 FF`
- ECG输出关闭：`EE 11 04 00 00 FF`
- ECG心率设置（示例72BPM）：`EE 21 30 37 32 FF`
  - `0x37 0x32` 为 ASCII 字符 `'7''2'`
  - 支持范围 `20~200 BPM`（两位命令可覆盖 20~99）
- ECG模式设置（示例模式20）：`EE 31 06 32 30 FF`
  - `0x32 0x30` 为 ASCII `'2''0'`
  - 支持模式 `01~30`，映射内部状态 `0~29`

## ECG模块接口

- `ECG_Init()`：初始化 DAC PA4
- `ECG_SetEnable()`：启停输出
- `ECG_SetHeartRate()`：设置心率
- `ECG_SetState()`：设置状态（30种）
- `ECG_SetAmplitude()`：设置幅值
- `ECG_Process()`：主循环周期调用，非阻塞输出

## Keil 依赖

需包含 HAL 源文件：
- `stm32f7xx_hal.c`
- `stm32f7xx_hal_gpio.c`
- `stm32f7xx_hal_rcc.c`
- `stm32f7xx_hal_rcc_ex.c`
- `stm32f7xx_hal_cortex.c`
- `stm32f7xx_hal_pwr.c`
- `stm32f7xx_hal_pwr_ex.c`
- `stm32f7xx_hal_flash.c`
- `stm32f7xx_hal_flash_ex.c`
- `stm32f7xx_hal_dac.c`
- `stm32f7xx_hal_dac_ex.c`
- `stm32f7xx_hal_uart.c`
