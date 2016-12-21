/*
 * Author: Wang Xudong
 * Date: 2016/10/25
 * Locale: Wuhan, Hubei
 * Email: hsu[AT]whu.edu.cn
 */

#ifndef SOURCEMGR_H
#define SOURCEMGR_H

#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <utility>

namespace cmm {

class SourceMgr {
public:
  using LocTy = size_t;
  enum class ErrorKind { Error, Warning };
  using ErrorTy = std::tuple<LocTy, ErrorKind, std::string>;

private:
  static const size_t ReservedLineNo = 120;
  std::ifstream SourceStream;
  std::vector<LocTy> LineNoOffsets;
  std::vector<ErrorTy> ErrorList;
  std::string SourceContent;
  LocTy CurrentLoc;
  bool DumpInstantly : 1;

  void dumpError(LocTy L, ErrorKind K, const std::string &Msg) const;

public:
  SourceMgr(const std::string &SourcePath,
                bool DumpInstantly = true);

  /// Functions that simulate member functions of std::fstream
  bool fail() const { return SourceStream.fail(); };
  int get();
  int peek();
  void unget();
  LocTy getLoc() { return CurrentLoc; }
  void seekLoc(LocTy Loc) { CurrentLoc = Loc; }

  void Error(LocTy L, const std::string &Msg);
  void Error(const std::string &Msg);
  void Warning(LocTy L, const std::string &Msg);
  void Warning(const std::string &Msg);

  std::pair<size_t, size_t> getLineColByLoc(LocTy Loc) const;

  // for debug
  void dumpFile();
};

}

#endif // !SOURCEMGR_H
