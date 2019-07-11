#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#define dbg_begin() Serial.begin(115200)
#define dbg_print(args...) Serial.print(args)
#define dbg_println(str) Serial.println(str)
#else
#define dbg_begin()
#define dbg_print(args...)
#define dbg_println(str)
#endif

#endif
