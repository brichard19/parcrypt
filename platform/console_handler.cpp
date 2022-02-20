#ifdef _WIN32
#include <Windows.h>
#else
#include <signal.h>
#endif

#include "platform.h"

namespace {
std::function<void(int)> _console_handler_ptr = nullptr;
};

namespace platform {

#ifdef _WIN32
static BOOL WINAPI console_handler(DWORD signal)
#else
static void console_handler(int signal)
#endif
{
  if(_console_handler_ptr) {
    _console_handler_ptr(signal);
  }
#ifdef _WIN32
  return TRUE;
#endif
}

bool set_console_handler(std::function<void(int)> handler)
{
  _console_handler_ptr = handler;

#ifdef _WIN32
  DWORD console_flags = 0;
  HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
  GetConsoleMode(input_handle, &console_flags);
  SetConsoleMode(input_handle, ENABLE_EXTENDED_FLAGS | (console_flags & ~ENABLE_QUICK_EDIT_MODE));

  if(!SetConsoleCtrlHandler(console_handler, TRUE)) {
    return false;
  }
#else
  struct sigaction sig_handler;
  sig_handler.sa_handler = console_handler;
  sigemptyset(&sig_handler.sa_mask);
  sig_handler.sa_flags = 0;
  sigaction(SIGINT, &sig_handler, NULL);
  sigaction(SIGTERM, &sig_handler, NULL);
#endif

  return true;
}

};