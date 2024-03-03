#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <ncurses.h>
#include <vector>

class Surface {
public:
  virtual void Refresh() = 0;
};

class Holder : public Surface {
public:
  explicit Holder(std::size_t top) : top_{top} {
    std::size_t sx = (COLS - width) / 2;
    window_ = newwin(height, width, top_, sx);
    box(window_, 0, 0);
  }

  void Refresh() { wrefresh(window_); }

  static const std::size_t height = 8, width = 70;

private:
  const std::size_t top_;
  WINDOW *window_;
};

class Compute : public Surface {
public:
  explicit Compute(std::size_t i, std::size_t sy, std::size_t sx) : i_{i} {
    window_ = newwin(side, side * 2, sy, sx);
    box(window_, 0, 0);

    int row, col;
    getmaxyx(window_, row, col);
    mvwprintw(window_, row / 2, (col - 1) / 2, "%d", static_cast<int>(i_));
  }

  void Refresh() { wrefresh(window_); }

  static const std::size_t side = 10;

private:
  const std::size_t i_;
  WINDOW *window_;
};

class Computes : public Surface {
public:
  explicit Computes(std::size_t size) : size_{size} {
    computes_.reserve(size_);

    const std::size_t sy = (LINES - Compute::side) / 2;
    const std::size_t bsx =
        (COLS / 2) - (Compute::side + space) * size_ + space;

    std::generate_n(std::back_inserter(computes_), size_,
                    [i = 0, sy, bsx]() mutable {
                      std::size_t sx = bsx + 2 * i * (Compute::side + space);
                      return Compute(i++, sy, sx);
                    });
  }

  void Refresh() {
    for (auto &compute : computes_) {
      compute.Refresh();
    }
  }

  static const std::size_t space = 4;

private:
  std::vector<Compute> computes_;
  const std::size_t size_;
};

int main() {
  initscr();
  curs_set(0);

  Holder main_holder(1), io_holder(LINES - (Holder::height + 5));

  Computes computes(4);

  refresh();
  main_holder.Refresh();
  io_holder.Refresh();
  computes.Refresh();

  getch();

  endwin();

  return EXIT_SUCCESS;
}
