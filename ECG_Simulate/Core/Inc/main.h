#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f7xx_hal.h"

#define LED_GPIO_PORT GPIOD /* LED所在GPIO端口 */
#define LED_PIN GPIO_PIN_9 /* LED所在GPIO引脚 */

#define DAC_HEART_RATE_MIN_BPM 20U /* 最小心率限制 */
#define DAC_HEART_RATE_MAX_BPM 200U /* 最大心率限制 */

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif
