#ifndef _CONSOLE_HANDLER_H_
#define _CONSOLE_HANDLER_H_

#include <functional>

bool set_console_handler(std::function<void(int)> handler);

#endif