#include <QtWidgets>
#include <iostream>
#include "MainWindow.hpp"
#include <QApplication>
#include <QtCore>
#include <QtGui>
#include <QPushButton>

#include "chess.hpp"
// #include <optional>
// #include "grid_button.hpp"
#include "button_grid.hpp"
// inspired by https://github.com/RoboJackets/qt-example

/*
Move: 
use get<0>=get<0>+dx repeatedly
 */

std::function<void()> show_clicked(int i, int j){
  std::function<void()> fn= [&i, &j]{
    std::cout << i << " " << j << "\n";
  };
  return fn;
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    // Create a widget
    QWidget *w = new QWidget();
    auto bg = new ButtonGrid(8,8);
    
    w->setLayout(bg->main_layout);

    w->setWindowTitle("CHESS");
    // Display
    w->show();

    // Event loop
    return app.exec();
}
