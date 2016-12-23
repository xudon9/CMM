#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include "CMMParser.h"

namespace cvm {
#define ADD_FUNCTION(FUNC) BasicValue FUNC(std::list<BasicValue> &Args)

namespace Native {
ADD_FUNCTION(TypeOf);
ADD_FUNCTION(Length);
ADD_FUNCTION(StrLength);
ADD_FUNCTION(Random);
ADD_FUNCTION(Srand);
ADD_FUNCTION(Print);
ADD_FUNCTION(PrintLn);
ADD_FUNCTION(System);
ADD_FUNCTION(Time);
ADD_FUNCTION(Exit);

ADD_FUNCTION(ToInt);
ADD_FUNCTION(ToBool);
ADD_FUNCTION(ToString);
ADD_FUNCTION(ToDouble);

ADD_FUNCTION(Read);
ADD_FUNCTION(ReadLn);
ADD_FUNCTION(ReadInt);

ADD_FUNCTION(Sqrt);
ADD_FUNCTION(Pow);
ADD_FUNCTION(Exp);
ADD_FUNCTION(Log);
ADD_FUNCTION(Log10);
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
ADD_FUNCTION(ColorPair);
}

namespace Unix {
ADD_FUNCTION(Fork);
}
#endif // defined(__APPLE__) || defined(__linux__)

#undef ADD_FUNCTION
}

#endif // !NATIVEFUNCTIONS_H
