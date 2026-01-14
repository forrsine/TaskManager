#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(R"(
        QTableView { selection-background-color: #0078d7; selection-color: white; }
        QPushButton { padding: 5px 10px; }
        QLineEdit { padding: 3px; }
    )");

    MainWindow w;
    w.show();

    return a.exec();
}
