#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include "SourceManager.h"

using namespace cmm;

void SourceManager::DumpError(LocTy L, ErrorKind K,
                              const std::string &Msg) const {
  auto LineCol = getLineColByLoc(L);

  std::cerr << (K == ErrorKind::Error ? "Error" : "Warning")
            <<" at Line " << LineCol.first + 1 << ", Col "
            << LineCol.second + 1 << ": " << Msg << std::endl;
}

SourceManager::SourceManager(const std::string &SourcePath,
                             bool DumpInstantly)
    : SourceStream(SourcePath), DumpInstantly(DumpInstantly) {
  LineNoOffsets.emplace_back(std::streampos(0));
  if (SourceStream.fail()) {
    Error(std::strerror(errno));
    std::exit(EXIT_FAILURE);
    return;
  }
  LineNoOffsets.reserve(ReservedLineNo);
}

int SourceManager::get() {
  int CurChar = SourceStream.get();
  std::streampos CurPos = SourceStream.tellg();
  if (CurChar == '\n') {
    auto it = std::lower_bound(LineNoOffsets.begin(),
                               LineNoOffsets.end(),
                               CurPos);
    if (it == LineNoOffsets.end())
      LineNoOffsets.emplace_back(CurPos);
    else if (*it != CurPos)
      LineNoOffsets.insert(it, CurPos);
#if 0
    std::cout << "--------------\n";
    for (size_t i = 0; i < LineNoOffsets.size(); ++i)
      std::cout << i << " " << LineNoOffsets[i] << std::endl;
    std::cout << "----OWARI-----\n";
#endif
  }
  return CurChar;
}

void SourceManager::Error(LocTy L, const std::string &Msg) {
  if (DumpInstantly)
    DumpError(L, ErrorKind::Error, Msg);
  else
    ErrorList.emplace_back(L, ErrorKind::Error, Msg);
}

void SourceManager::Error(const std::string &Msg) {
  Error(SourceStream.tellg(), Msg);
}

void SourceManager::Warning(LocTy L, const std::string &Msg) {
  if (DumpInstantly)
    DumpError(L, ErrorKind::Warning, Msg);
  else
    ErrorList.emplace_back(L, ErrorKind::Warning, Msg);
}

void SourceManager::Warning(const std::string &Msg) {
  Warning(SourceStream.tellg(), Msg);
}

std::pair<size_t, size_t> SourceManager::getLineColByLoc(LocTy L) const {
  auto it = std::upper_bound(LineNoOffsets.cbegin(), LineNoOffsets.cend(), L);
  return (it == LineNoOffsets.cend())
      ? std::make_pair(static_cast<size_t>(LineNoOffsets.size() - 1),
                       static_cast<size_t>(L - LineNoOffsets.back()))
      : std::make_pair(static_cast<size_t>(it - LineNoOffsets.begin()),
                       static_cast<size_t>(L - *it));
}