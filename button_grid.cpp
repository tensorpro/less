#include "button_grid.hpp"
#include <QtWidgets>
#include <iostream>
#include <QtCore>
#include <QtGui>
#include <QPushButton>
#include <QGridLayout>
#include <QButtonGroup>
#include "chess.hpp"
#include <functional>
#include <string>     // std::string, std::to_string
char icons[2][10][4] = {{"♜","♞","♝","♛","♚","♟","♞","♟","♜"}, {"♖","♘","♗","♕","♔","♙","♘","♙","♖"}};

void set_color(QPushButton* b, QColor c){
  QPalette pal = b->palette();
  b->setStyleSheet("font-size: 60px;");
  b->setFlat(true);
  pal.setColor(QPalette::Button, c);
  b->setAutoFillBackground(true);
  b->setPalette(pal);
  b->update();
}
 
using ButtonFN = std::function<void(QPushButton*, Pos)>;
using Buttons = QPushButton*(*)[8];

void button_map(Buttons buttons, ButtonFN fn){
  for(int row=0; row<8;row++){
    for(int col=0; col<8;++col){
      QPushButton* b = buttons[row][col];
      fn(b, Pos{row,col});
    }
  }
}

void show_pieces(Buttons buttons, Game g){
  for(int row=0; row<8;row++){
    for(int col=0; col<8;++col){
      QPushButton* b = buttons[row][col];
      Piece* piece= g.board.get_piece(Pos{row,col});
      auto txt = " ";
      if(piece!=nullptr){
	txt = icons[piece->color][piece->name];
      }
      b->setText(txt);
    }
  }
}

void checker_board(Buttons buttons){
  for(int row=0; row<8;row++){
    for(int col=0; col<8;++col){
      bool gray = (row+col) % 2 == 0;
      QColor c = gray ? QColor(Qt::gray):QColor(Qt::white);
      set_color(buttons[row][col], c);
    }
  }
}


void show_set(Buttons buttons, MoveSet* s, Qt::GlobalColor color, Qt::GlobalColor dark){
  for(const Pos& pos : *(s)){
    QPushButton* b = buttons[pos.first][pos.second];
    b->update();
    auto final_color = (pos.first + pos.second) % 2 != 0 ? color : dark;
    set_color(b, QColor(final_color));
  }
}
void ButtonGrid::render(){
  checker_board(buttons);
  if(model.selected){
    auto [row, col] = model.selected_pos;
    QPushButton* start = buttons[row][col];
    set_color(start, QColor(Qt::green));
    for(const Pos& pos : *(model.selected_moves)){
      QPushButton* b = buttons[pos.first][pos.second];
      b->update();
      auto color = (pos.first + pos.second) % 2 != 0 ? Qt::cyan : Qt::darkCyan;
      set_color(b, QColor(color));
    }
  }
  else{
    if(model.get_help()){
      MoveSet* s = all_moves(model.game,model.game.get_turn());
      checker_board(buttons);
      show_set(buttons, s, Qt::yellow, Qt::darkYellow);
      delete s;
      s = all_moves(model.game,other_color(model.game.get_turn()));
      show_set(buttons, s, Qt::red, Qt::darkRed);
      delete s;
    }
  }
  auto player_1_string = QStringLiteral("Player 1: %1").arg(model.get_score(0));
  auto player_2_string = QStringLiteral("Player 2: %1").arg(model.get_score(1));
  player_1_score->setText(player_1_string);
  player_2_score->setText(player_2_string);
  if(model.is_new_game()) fairy->setText("Fairy");
  else fairy->setText("Help");
  fairy->update();

}



void ButtonGrid:: on_click(int i){
  int col = i%8;
  int row = i/8;
  Pos pos = Pos{row,col};
  model.update_game(pos);
  show_pieces(buttons, model.game);
  // auto txt = " ";
  Piece* piece = model.game.board.get_piece(pos);
  // if(piece!=nullptr){
    // txt = icons[piece->color][piece->name];
  // }
  QPushButton* b = buttons[row][col];
  set_color(b, QColor(Qt::green));
  render();
}

void ButtonGrid:: redo_slot(){
  model.redo();
  show_pieces(buttons, model.game);
  render();
}

void ButtonGrid:: undo_slot(){
  model.undo();
  show_pieces(buttons, model.game);
  render();
}

void ButtonGrid:: reset_slot(){
  model.reset();
  show_pieces(buttons, model.game);
  render();
}

void ButtonGrid:: resign_slot(){
  model.resign();
  show_pieces(buttons, model.game);
  render();
}

void ButtonGrid:: fairy_slot(){
  model.fairy();
  show_pieces(buttons, model.game);
  render();
}

ButtonGrid::  ButtonGrid(int rs, int cs):
  num_rows{rs}
  , num_cols{cs}
  , layout{new QGridLayout}
  , buttons{nullptr}
  , button_group{new QButtonGroup}
  , option_button_layout{new QHBoxLayout}
  , reset {new QPushButton{"Reset"}}
  , resign {new QPushButton{"Resign"}}
  , undo {new QPushButton{"Undo"}}
  , redo  {new QPushButton{"Redo"}}
  , fairy  {new QPushButton{"Fairy"}}
  , scores_layout {new QHBoxLayout}
  , player_1_score {new QLabel("Player 1:0")}
  , player_2_score {new QLabel("Player 2:0")}
  , main_layout {new QVBoxLayout}
  , model{}
{
    
  layout->setSpacing(0.1);
  for(int row=0; row<num_rows; ++row){
    for(int col=0; col<num_cols; ++col){
      auto *b = new QPushButton(" ");
      button_group->addButton(b,row*8+col);
      b->setStyleSheet("font-size: 60px;");
      b->setFlat(true);
      layout->addWidget(b,row,col,1,1);
      buttons[row][col]=b;
    }
  }
  checker_board(buttons);
  show_pieces(buttons, model.game);
  connect(button_group, SIGNAL(buttonReleased(int)), this, SLOT(on_click(int)));
  connect(undo, SIGNAL(released()), this, SLOT(undo_slot()));
  connect(redo, SIGNAL(released()), this, SLOT(redo_slot()));
  connect(fairy, SIGNAL(released()), this, SLOT(fairy_slot()));
  connect(reset, SIGNAL(released()), this, SLOT(reset_slot()));
  connect(resign, SIGNAL(released()), this, SLOT(resign_slot()));


  option_button_layout->addWidget(reset);
  option_button_layout->addWidget(resign);
  option_button_layout->addWidget(undo);
  option_button_layout->addWidget(redo);
  option_button_layout->addWidget(fairy);
  auto l = QStringLiteral("Player 2: %1").arg(1);
  auto l2 = QStringLiteral("Player 1: %1").arg(8);

  player_1_score = new QLabel(l);
  player_2_score = new QLabel(l2);
  scores_layout->addWidget(player_1_score);
  scores_layout->addWidget(player_2_score);
    
  main_layout->addLayout(layout);
  main_layout->addLayout(option_button_layout);
  main_layout->addLayout(scores_layout);

  render();
};
