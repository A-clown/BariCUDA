#ifndef __ECG_H
#define __ECG_H

#include "main.h"

#define ECG_STATE_COUNT 30U /* ECG状态总数量 */

void ECG_Init(DAC_HandleTypeDef *hdac);
void ECG_SetState(uint8_t state_id);
void ECG_SetHeartRate(uint16_t heart_rate_bpm);
void ECG_SetAmplitude(uint16_t amplitude_mv);
void ECG_SetEnable(uint8_t enable);
void ECG_Process(DAC_HandleTypeDef *hdac);

#endif
