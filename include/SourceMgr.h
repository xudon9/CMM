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
  using LocTy = std::streampos;
  enum class ErrorKind { Error, Warning };
  using ErrorTy = std::tuple<LocTy, ErrorKind, std::string>;

private:
  static const size_t ReservedLineNo = 120;
  std::ifstream SourceStream;
  std::vector<LocTy> LineNoOffsets;
  std::vector<ErrorTy> ErrorList;
  bool DumpInstantly : 1;

  void dumpError(LocTy L, ErrorKind K, const std::string &Msg) const;

public:
  SourceMgr(const std::string &SourcePath,
                bool DumpInstantly = true);

  /// Functions that simulate memeber functions of std::fstream
  bool fail() const { return SourceStream.fail(); };
  int get();
  int peek() { return SourceStream.peek(); };
  void unget() { SourceStream.unget(); }
  LocTy getLoc() { return SourceStream.tellg(); }
  void seekLoc(LocTy Loc) { SourceStream.seekg(Loc); }

  void Error(LocTy L, const std::string &Msg);
  void Error(const std::string &Msg);
  void Warning(LocTy L, const std::string &Msg);
  void Warning(const std::string &Msg);

  std::pair<size_t, size_t> getLineColByLoc(LocTy Loc) const;
};

}

#endif // !SOURCEMGR_H
