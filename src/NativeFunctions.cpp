#include "NativeFunctions.h"

#include "CMMParser.h"
#include <cstdlib>

namespace cvm {

cvm::BasicValue NativePrint(std::list<cvm::BasicValue> &Args) {
  for (auto &Arg : Args) {
    std::cout << Arg.toString() << " ";
  }
  return cvm::BasicValue();
}

cvm::BasicValue NativePrintln(std::list<cvm::BasicValue> &Args) {
  NativePrint(Args);
  std::cout << std::endl;
  return cvm::BasicValue();
}

cvm::BasicValue NativeSystem(std::list<cvm::BasicValue> &Args) {
  for (auto &Arg : Args) {
    std::system(Arg.toString().c_str());
  }
  return cvm::BasicValue();
}

}
