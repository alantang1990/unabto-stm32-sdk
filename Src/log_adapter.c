/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include "unabto_platform.h"
#include "lcd_log.h"

void p(const char *fmt, ...)
{
  static char tmp[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(tmp, 128, fmt, args);
  va_end(args);
  LCD_UsrLog("%s\n", tmp);
}
