#include "NativeFunctions.h"

#include "CMMParser.h"

#include <ctime>
#include <cstdlib>
#include <cmath>

#if defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#include <curses.h>
#endif

namespace cvm {

BasicValue Native::StrLen(std::list<BasicValue> &Args) {
  if (Args.empty())
    return 0;
  return static_cast<int>(Args.front().StrVal.size());
}

BasicValue Native::ReadInt(std::list<BasicValue> &/*Args*/) {
  int Res;
  std::cin >> Res;
  return Res;
}

BasicValue Native::ReadLn(std::list<BasicValue> &/*Args*/) {
  std::string Res;
  std::getline(std::cin, Res);
  return Res;
}

BasicValue Native::Read(std::list<BasicValue> &/*Args*/) {
  std::string Res;
  std::cin >> Res;
  return Res;
}

BasicValue Native::ToInt(std::list<BasicValue> &Args) {
  if (Args.size() != 1)
    return 0;
  return Args.front().toInt();
}

BasicValue Native::ToBool(std::list<BasicValue> &Args) {
  if (Args.size() != 1)
    return false;
  return Args.front().toBool();
}

BasicValue Native::ToString(std::list<BasicValue> &Args) {
  if (Args.size() != 1)
    return std::string();
  return Args.front().toString();
}

BasicValue Native::ToDouble(std::list<BasicValue> &Args) {
  if (Args.size() != 1)
    return 0.0;
  return Args.front().toDouble();
}

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

BasicValue Native::PrintLn(std::list<BasicValue> &Args) {
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

BasicValue Native::Sqrt(std::list<BasicValue> &Args) {
  if (Args.empty())
    return 0.0;
  return std::sqrt(Args.front().toDouble());
}

BasicValue Native::Pow(std::list<BasicValue> &Args) {
  if (Args.size() != 2)
    return 0.0;
  return std::pow(Args.front().toDouble(), Args.back().toDouble());
}

BasicValue Native::Exp(std::list<BasicValue> &Args) {
  if (Args.empty())
    return 0.0;
  return std::exp(Args.front().toDouble());
}

BasicValue Native::Log(std::list<BasicValue> &Args) {
  if (Args.empty())
    return 0.0;
  return std::log(Args.front().toDouble());
}

BasicValue Native::Log10(std::list<BasicValue> &Args) {
  if (Args.empty())
    return 0.0;
  return std::log10(Args.front().toDouble());
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

  char C = static_cast<char>(Args.back().IntVal);
  return mvaddch(Y, X, C);
}

BasicValue Ncurses::MoveAddString(std::list<BasicValue> &Args) {
  if (Args.size() != 3)
    return BasicValue();

  auto Iterator = Args.cbegin();

  int Y = Iterator->toInt();
  ++Iterator;
  int X = Iterator->toInt();

  const char *S = Args.back().StrVal.c_str();
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
