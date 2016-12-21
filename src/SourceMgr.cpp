#include "SourceMgr.h"
#include <iostream>
#include <algorithm>
#include <cassert>

using namespace cmm;

void SourceMgr::dumpError(LocTy L, ErrorKind K,
                              const std::string &Msg) const {
  auto LineCol = getLineColByLoc(L);

  std::cerr << (K == ErrorKind::Error ? "Error" : "Warning")
            << " at (Line " << LineCol.first + 1 << ", Col "
            << LineCol.second + 1 << "): " << Msg << std::endl;
}

SourceMgr::SourceMgr(const std::string &SourcePath, bool DumpInstantly)
  : SourceStream(SourcePath), CurrentLoc(0), DumpInstantly(DumpInstantly) {

  if (SourceStream.fail()) {
    std::cerr << "Fatal Error: Cannot open file '" << SourcePath
              << "', exited." << std::endl;
    std::exit(EXIT_FAILURE);
  }

  SourceStream.seekg(0, SourceStream.end);
  SourceContent.reserve(SourceStream.tellg());
  SourceStream.seekg(0, SourceStream.beg);
  LineNoOffsets.reserve(ReservedLineNo);
  LineNoOffsets.emplace_back(std::streampos(0));

  int CurChar;
  size_t Index;
  for (size_t Offset = 0; (CurChar = SourceStream.get()) !=
       std::char_traits<char>::eof(); ++Offset) {
    SourceContent.push_back(static_cast<char>(CurChar));

    if (CurChar == '\n')
      LineNoOffsets.push_back(Offset);
  }

  SourceStream.close();
}

int SourceMgr::get() {
  if (CurrentLoc == SourceContent.size())
    return std::char_traits<char>::eof();
  return SourceContent[CurrentLoc++];
}

int SourceMgr::peek() {
  if (CurrentLoc == SourceContent.size())
    return std::char_traits<char>::eof();
  return SourceContent[CurrentLoc];
}

void SourceMgr::unget() {
  if (CurrentLoc > 0)
    --CurrentLoc;
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
  //std::cout << L << std::endl;
  auto It = std::upper_bound(LineNoOffsets.cbegin(), LineNoOffsets.cend(), L);
  assert(It > LineNoOffsets.begin() && It <= LineNoOffsets.end() && 
         "getLineColByLoc: iterator out of bound");
  --It;
  size_t LineIndex = It - LineNoOffsets.cbegin();
  size_t ColIndex = static_cast<size_t>(L - *It);
  return std::make_pair(LineIndex, ColIndex);
}

void SourceMgr::dumpFile() {
  int CurChar;
  while ((CurChar = get()) != std::char_traits<char>::eof()) {
    std::cout << static_cast<char>(CurChar);
  }
}
