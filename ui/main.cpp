#include <QApplication>
#include <QHeaderView>
#include <QTableWidget>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QTableWidget table(10, 5);
  table.setWindowTitle("HC Simple");
  table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table.verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  for (std::size_t i = 0; i < 10; ++i) {
    for (std::size_t j = 0; j < 5; ++j) {
      QTableWidgetItem *item =
          new QTableWidgetItem(QString("Ce %1, %2").arg(i).arg(j));
      table.setItem(i, j, item);
    }
  }

  table.show();

  return app.exec();
}
