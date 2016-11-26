#include <iostream>
#include <algorithm>
#include <cassert>
#include "SourceMgr.h"

using namespace cmm;

void SourceMgr::dumpError(LocTy L, ErrorKind K,
                              const std::string &Msg) const {
  auto LineCol = getLineColByLoc(L);

  std::cerr << (K == ErrorKind::Error ? "Error" : "Warning")
            << " at (Line " << LineCol.first + 1 << ", Col "
            << LineCol.second + 1 << "): " << Msg << std::endl;
}

SourceMgr::SourceMgr(const std::string &SourcePath,
                             bool DumpInstantly)
  : SourceStream(SourcePath), DumpInstantly(DumpInstantly) {
  if (SourceStream.fail()) {
    std::cerr << "Fatal Error: Cannot open file '" << SourcePath
              << "', exited." << std::endl;
    std::exit(EXIT_FAILURE);
  }
  LineNoOffsets.reserve(ReservedLineNo);
  LineNoOffsets.emplace_back(std::streampos(0));
}

int SourceMgr::get() {
  int CurChar = SourceStream.get();
  std::streampos CurPos = SourceStream.tellg();
  if (CurChar == '\n') {
    auto It = std::lower_bound(LineNoOffsets.begin(), LineNoOffsets.end(),
                               CurPos);
    if (It == LineNoOffsets.cend() || *It != CurPos)
      LineNoOffsets.insert(It, CurPos);
  }
  return CurChar;
}

void SourceMgr::Error(LocTy L, const std::string &Msg) {
  if (DumpInstantly)
    dumpError(L, ErrorKind::Error, Msg);
  else
    ErrorList.emplace_back(L, ErrorKind::Error, Msg);
}

void SourceMgr::Error(const std::string &Msg) {
  Error(SourceStream.tellg(), Msg);
}

void SourceMgr::Warning(LocTy L, const std::string &Msg) {
  if (DumpInstantly)
    dumpError(L, ErrorKind::Warning, Msg);
  else
    ErrorList.emplace_back(L, ErrorKind::Warning, Msg);
}

void SourceMgr::Warning(const std::string &Msg) {
  Warning(SourceStream.tellg(), Msg);
}

std::pair<size_t, size_t> SourceMgr::getLineColByLoc(LocTy L) const {
  auto It = std::upper_bound(LineNoOffsets.cbegin(), LineNoOffsets.cend(), L);
  assert(It >= LineNoOffsets.begin() && It <= LineNoOffsets.end() && 
         "getLineColByLoc: iterator out of bound");
  --It;
  size_t LineIndex = It - LineNoOffsets.cbegin();
  size_t ColIndex = static_cast<size_t>(L - *It);
  return std::make_pair(LineIndex, ColIndex);
}