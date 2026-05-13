#include "ecg.h"

typedef struct {
  uint16_t amp_percent; /* 当前状态幅值百分比 */
  int16_t dc_offset; /* 当前状态直流偏移 */
} ECG_StateProfile;

static uint32_t ECG_MakeSample(uint16_t base12bit, uint16_t amp12bit, uint16_t wave);
static uint16_t ECG_GetStateWave(uint16_t base_wave);

static uint8_t g_ecg_state = 0U; /* 当前ECG状态索引0~29 */
static uint8_t g_enabled = 0U; /* ECG输出使能标志 */
static uint16_t g_heart_rate_bpm = 75U; /* 当前心率参数BPM */
static uint16_t g_amplitude_mv = 1000U; /* 当前幅值参数mV */
static uint32_t g_sample_idx = 0U; /* 波形采样点索引 */
static uint32_t g_last_tick = 0U; /* 上次输出时间戳 */

static const ECG_StateProfile ecg_profiles[ECG_STATE_COUNT] = {
  {100,0},{95,-20},{90,-40},{105,10},{110,20},{115,30},{120,40},{85,-60},{80,-80},{75,-100},
  {130,60},{135,80},{140,100},{92,-30},{88,-50},{108,20},{112,35},{118,45},{125,55},{98,-10},
  {102,0},{106,15},{94,-25},{89,-45},{84,-70},{128,50},{132,70},{138,95},{96,-15},{104,10}
};

static const uint16_t ecg_wave_128[128] = {
  2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
  2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
  2100,2120,2140,2160,2180,2200,2180,2160,2140,2120,2100,2080,2060,2050,2048,2048,
  2020,1980,1940,1900,1840,1760,1600,1300,900,600,900,1300,1600,1760,1900,1980,
  2048,2060,2080,2100,2120,2140,2160,2180,2200,2180,2160,2140,2120,2100,2080,2060,
  2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,
  2060,2070,2080,2090,2100,2110,2100,2090,2080,2070,2060,2050,2048,2048,2048,2048,
  2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048
};

void ECG_Init(DAC_HandleTypeDef *hdac){
  DAC_ChannelConfTypeDef sConfig={0}; GPIO_InitTypeDef g={0};
  __HAL_RCC_DAC_CLK_ENABLE(); __HAL_RCC_GPIOA_CLK_ENABLE();
  g.Pin=GPIO_PIN_4; g.Mode=GPIO_MODE_ANALOG; g.Pull=GPIO_NOPULL; HAL_GPIO_Init(GPIOA,&g);
  hdac->Instance=DAC; if(HAL_DAC_Init(hdac)!=HAL_OK) Error_Handler();
  sConfig.DAC_Trigger=DAC_TRIGGER_NONE; sConfig.DAC_OutputBuffer=DAC_OUTPUTBUFFER_ENABLE;
  if(HAL_DAC_ConfigChannel(hdac,&sConfig,DAC_CHANNEL_1)!=HAL_OK) Error_Handler();
  if(HAL_DAC_Start(hdac,DAC_CHANNEL_1)!=HAL_OK) Error_Handler();
}

void ECG_SetState(uint8_t state_id){ g_ecg_state = (state_id<ECG_STATE_COUNT)?state_id:0U; }
void ECG_SetHeartRate(uint16_t bpm){ if(bpm<DAC_HEART_RATE_MIN_BPM) bpm=DAC_HEART_RATE_MIN_BPM; if(bpm>DAC_HEART_RATE_MAX_BPM) bpm=DAC_HEART_RATE_MAX_BPM; g_heart_rate_bpm=bpm; }
void ECG_SetAmplitude(uint16_t mv){ if(mv>3300U) mv=3300U; g_amplitude_mv=mv; }
void ECG_SetEnable(uint8_t enable){ g_enabled = enable ? 1U : 0U; }

void ECG_Process(DAC_HandleTypeDef *hdac)
{
  uint32_t now=HAL_GetTick(); /* 当前系统时间tick */
  uint32_t sample_period_ms=(60000U/g_heart_rate_bpm)/128U; /* 每个采样点输出周期ms */
  uint16_t amp12bit=(uint16_t)((g_amplitude_mv*4095U)/3300U); /* 幅值换算到12位DAC */
  uint16_t wave; /* 当前输出波形采样值 */
  if(sample_period_ms==0U) sample_period_ms=1U;

  if(!g_enabled){ HAL_DAC_SetValue(hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,2048); return; }
  if((now-g_last_tick)<sample_period_ms) return;
  g_last_tick = now;

  wave = ECG_GetStateWave(ecg_wave_128[g_sample_idx]);
  HAL_DAC_SetValue(hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,ECG_MakeSample(2048U,amp12bit,wave));
  g_sample_idx = (g_sample_idx + 1U) % 128U;
  if(g_sample_idx==0U) HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
}

static uint16_t ECG_GetStateWave(uint16_t base_wave){
  int32_t c=(int32_t)base_wave-2048;
  int32_t s=(c*(int32_t)ecg_profiles[g_ecg_state].amp_percent)/100;
  int32_t y=2048+s+ecg_profiles[g_ecg_state].dc_offset;
  if(y<0) y=0; else if(y>4095) y=4095; return (uint16_t)y;
}

static uint32_t ECG_MakeSample(uint16_t base12bit, uint16_t amp12bit, uint16_t wave){
  int32_t c=(int32_t)wave-2048; int32_t s=((int32_t)amp12bit*c)/2048; int32_t v=(int32_t)base12bit+s;
  if(v<0) v=0; else if(v>4095) v=4095; return (uint32_t)v;
}
