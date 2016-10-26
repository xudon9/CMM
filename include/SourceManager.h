/*
 * Author: Wang Hsutung
 * Date: 2016/10/25
 * Locale: Wuhan, Hubei
 * Email: hsu[AT]whu.edu.cn
 */

#ifndef SOURCEMANAGER_H
#define SOURCEMANAGER_H

#include <fstream>
#include <string>
#include <vector>
#include <utility>

namespace cmm {

class SourceManager {
public:
  using LocTy = std::streampos;
  using ErrorTy = std::pair<LocTy, std::string>;

private:
  static const size_t ReservedLineNo = 120;
  std::ifstream SourceStream;
  std::vector<LocTy> LineNoOffsets;
  std::vector<ErrorTy> ErrorList;
  bool DumpInstantly : 1;

  void DumpError(LocTy L, const std::string &Msg);

public:
  SourceManager(const std::string &SourcePath,
                bool DumpInstantly = true);

  // Functions provided by std::fstream
  bool fail() const { return SourceStream.fail(); };
  int get();
  int peek() { return SourceStream.peek(); };
  void unget() { SourceStream.unget(); }
  LocTy getLoc() { return SourceStream.tellg(); }

  void Error(LocTy L, const std::string &Msg);
  void Error(const std::string &Msg);
};

}

#endif /* SOURCEMANAGER_H */
