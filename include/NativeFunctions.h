#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include "CMMParser.h"

namespace cvm {

namespace Native {
BasicValue Random(std::list<BasicValue> &Args);
BasicValue Srand(std::list<BasicValue> &Args);
BasicValue Print(std::list<BasicValue> &Args);
BasicValue Println(std::list<BasicValue> &Args);
BasicValue System(std::list<BasicValue> &Args);
BasicValue Time(std::list<BasicValue> &Args);
}

#if defined(__APPLE__) || defined(__linux__)
namespace Ncurses {
BasicValue EndWindow(std::list<BasicValue> &Args);
BasicValue InitScreen(std::list<BasicValue> &Args);
BasicValue NoEcho(std::list<BasicValue> &Args);
BasicValue CursSet(std::list<BasicValue> &Args);
BasicValue Keypad(std::list<BasicValue> &Args);
BasicValue Timeout(std::list<BasicValue> &Args);
BasicValue GetChar(std::list<BasicValue> &Args);
BasicValue MoveAddChar(std::list<BasicValue> &Args);
BasicValue MoveAddString(std::list<BasicValue> &Args);
}
#endif
}

#endif // !NATIVEFUNCTIONS_H
