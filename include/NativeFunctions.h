#ifndef NATIVEFUNCTIONS_H
#define NATIVEFUNCTIONS_H

#include "CMMParser.h"

namespace cvm {

cvm::BasicValue NativePrint(std::list<cvm::BasicValue> &Args);
cvm::BasicValue NativePrintln(std::list<cvm::BasicValue> &Args);
cvm::BasicValue NativeSystem(std::list<cvm::BasicValue> &Args);
}

#endif // !NATIVEFUNCTIONS_H
