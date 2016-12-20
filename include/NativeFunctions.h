#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include "CMMParser.h"

namespace cvm {
#define ADD_FUNCTION(FUNC) BasicValue FUNC(std::list<BasicValue> &Args);

namespace Native {
ADD_FUNCTION(Random);
ADD_FUNCTION(Srand);
ADD_FUNCTION(Print);
ADD_FUNCTION(Println);
ADD_FUNCTION(System);
ADD_FUNCTION(Time);
ADD_FUNCTION(Exit);
}

#if defined(__APPLE__) || defined(__linux__)
namespace Ncurses {
ADD_FUNCTION(InitScreen);
ADD_FUNCTION(GetMaxY);
ADD_FUNCTION(GetMaxX);
ADD_FUNCTION(EndWindow);
ADD_FUNCTION(NoEcho);
ADD_FUNCTION(CursSet);
ADD_FUNCTION(Keypad);
ADD_FUNCTION(Timeout);
ADD_FUNCTION(GetChar);
ADD_FUNCTION(MoveAddChar);
ADD_FUNCTION(MoveAddString);
ADD_FUNCTION(MessageBox);
ADD_FUNCTION(StartColor);
ADD_FUNCTION(InitPair);
ADD_FUNCTION(AttrOn);
ADD_FUNCTION(AttrOff);
}

namespace Unix {
ADD_FUNCTION(Fork);
}
#endif // defined(__APPLE__) || defined(__linux__)

#undef ADD_FUNCTION
}

#endif // !NATIVEFUNCTIONS_H
