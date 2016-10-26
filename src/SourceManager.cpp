#include <cstring>
#include <iostream>
#include <algorithm>
#include <cassert>
#include "SourceManager.h"

using namespace cmm;

void SourceManager::DumpError(LocTy L, const std::string &Msg) {
  auto it = std::lower_bound(LineNoOffsets.begin(), LineNoOffsets.end(), L);
  if (*it > L) {
    assert(it != LineNoOffsets.begin());
    --it;
  }

  auto LineIndex = it - LineNoOffsets.begin();  // LineNo = LineIndex + 1
  auto ColIndex = L - LineNoOffsets[LineIndex]; // ColNo = ColIndex + 1

  std::cerr << "Error (Line " << LineIndex + 1 << ", Col)"
            << ColIndex + 1 << ": " << Msg << std::endl;
}

SourceManager::SourceManager(const std::string &SourcePath,
                             bool DumpInstantly)
    : SourceStream(SourcePath), DumpInstantly(DumpInstantly) {
  if (SourceStream.fail()) {
    Error(strerror(errno));
    return;
  }
  LineNoOffsets.reserve(ReservedLineNo);
  LineNoOffsets.emplace_back(std::streampos(0));
}

int SourceManager::get() {
  int CurChar = SourceStream.get();
  std::streampos CurPos = SourceStream.tellg();
  if (CurChar == '\n') {
    auto it = std::lower_bound(LineNoOffsets.begin(),
                               LineNoOffsets.end(),
                               CurPos);
    if (*it != CurPos)
      LineNoOffsets.insert(it, CurPos);
  }
  return CurChar;
}

void SourceManager::Error(LocTy L, const std::string &Msg) {
  if (DumpInstantly)
    DumpError(L, Msg);
  else
    ErrorList.emplace_back(L, Msg);
}

void SourceManager::Error(const std::string &Msg) {
  Error(SourceStream.tellg(), Msg);
}
