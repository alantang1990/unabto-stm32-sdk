/* Includes ------------------------------------------------------------------*/
#include <unabto/unabto_external_environment.h>
#include "stm32f7xx_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
bool random_initialized = false;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void nabto_random(uint8_t* buf, size_t len)
{
  if (!random_initialized) {
    __HAL_RCC_RNG_CLK_ENABLE();
    RNG->CR |= RNG_CR_RNGEN;
    random_initialized = true;
  }
  size_t ix;
  for (ix = 0; ix < len; ++ix) {
    while (!(RNG->SR & (RNG_SR_DRDY)));
    *buf++ = (uint8_t) RNG->DR;
  }
}
