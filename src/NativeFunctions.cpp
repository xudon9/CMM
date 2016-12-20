#include "NativeFunctions.h"

#include "CMMParser.h"

#include <ctime>
#include <cstdlib>

#if defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#include <ncurses.h>
#endif

namespace cvm {

BasicValue Native::Exit(std::list<BasicValue> &Args) {
  if (Args.empty())
    std::exit(EXIT_SUCCESS);
  std::exit(Args.front().toInt());
}

BasicValue Native::Print(std::list<BasicValue> &Args) {
  for (auto &Arg : Args) {
    std::cout << Arg.toString() << " ";
  }
  return BasicValue();
}

BasicValue Native::Println(std::list<BasicValue> &Args) {
  Native::Print(Args);
  std::cout << "\n";
  return BasicValue();
}

BasicValue Native::System(std::list<BasicValue> &Args) {
  for (auto &Arg : Args) {
    std::system(Arg.toString().c_str());
  }
  return BasicValue();
}

BasicValue Native::Random(std::list<BasicValue> &Args) {
  if (Args.empty())
    return std::rand();

  if (Args.size() == 1)
      return std::rand() % Args.front().toInt();

  int Low = Args.front().toInt(), High = Args.back().toInt();
  return std::rand() % (High - Low) + Low;
}

BasicValue Native::Srand(std::list<BasicValue> &Args) {
  int Seed = (Args.empty() || !Args.front().isInt()) ? 0 : Args.front().IntVal;
  std::srand(static_cast<unsigned int>(Seed));
  return BasicValue();
}

BasicValue Native::Time(std::list<BasicValue> &/*Args*/) {
  return static_cast<int>(std::time(nullptr));
}

#if defined(__APPLE__) || defined(__linux__)

BasicValue Unix::Fork(std::list<BasicValue> &/*Args*/) {
  return ::fork();
}

BasicValue Ncurses::GetMaxY(std::list<BasicValue> &/*Args*/) {
  return getmaxy(stdscr);
}

BasicValue Ncurses::GetMaxX(std::list<BasicValue> &/*Args*/) {
  return getmaxx(stdscr);
}

BasicValue Ncurses::InitScreen(std::list<BasicValue> &/*Args*/) {
  ::initscr();
  return BasicValue();
}

BasicValue Ncurses::NoEcho(std::list<BasicValue> &/*Args*/) {
  return ::noecho();
}

BasicValue Ncurses::CursSet(std::list<BasicValue> &Args) {
  return ::curs_set(Args.empty() ? false : Args.front().toBool());
}

BasicValue Ncurses::Keypad(std::list<BasicValue> &Args) {
  return ::keypad(::stdscr, Args.empty() ? false : Args.front().toBool());
}

BasicValue Ncurses::Timeout(std::list<BasicValue> &Args) {
  ::timeout(Args.empty() ? -1 : Args.front().toInt());
  return BasicValue();
}

BasicValue Ncurses::GetChar(std::list<BasicValue> &/*Args*/) {
  return ::wgetch(stdscr);
}

BasicValue Ncurses::MoveAddChar(std::list<BasicValue> &Args) {
  if (Args.size() != 3)
    return BasicValue();

  auto Iterator = Args.cbegin();

  int Y = Iterator->toInt();
  ++Iterator;
  int X = Iterator->toInt();

  std::string S = Args.back().toString();
  char C = S.empty() ? ' ' : S.front();
  return mvaddch(Y, X, C);
}

BasicValue Ncurses::MoveAddString(std::list<BasicValue> &Args) {
  if (Args.size() != 3)
    return BasicValue();

  auto Iterator = Args.cbegin();

  int Y = Iterator->toInt();
  ++Iterator;
  int X = Iterator->toInt();

  const char *S = Args.back().toString().c_str();
  return mvaddstr(Y, X, S);
}

BasicValue Ncurses::EndWindow(std::list<BasicValue> &/*Args*/) {
  return ::endwin();
}

BasicValue Ncurses::InitPair(std::list<BasicValue> &Args) {
  if (Args.size() != 3)
    return ERR;

  auto Iterator = Args.cbegin();

  short PairNo = static_cast<short>(Iterator->toInt());
  ++Iterator;
  short FgColor = static_cast<short>(Iterator->toInt());
  short BgColor = static_cast<short>(Args.back().toInt());

  return ::init_pair(PairNo, FgColor, BgColor);
}

BasicValue Ncurses::StartColor(std::list<BasicValue> &/*Args*/) {
  return ::start_color();
}

BasicValue Ncurses::AttrOn(std::list<BasicValue> &Args) {
  if (Args.empty())
    return ERR;
  return attron(Args.front().toInt());
}

BasicValue Ncurses::AttrOff(std::list<BasicValue> &Args) {
  if (Args.empty())
    return ERR;
  return attroff(Args.front().toInt());
}

BasicValue Ncurses::ColorPair(std::list<BasicValue> &Args) {
  if (Args.empty())
    return 0;
  return static_cast<int>(COLOR_PAIR(Args.front().toInt()));
}

#endif // defined(__APPLE__) || defined(__linux__)
}
