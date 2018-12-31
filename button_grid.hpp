#ifndef BUTTON_GRID_H
#define BUTTON_GRID_H

#include <QObject>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel> 
#include "chess.hpp"
class ButtonGrid : public QObject
{
  Q_OBJECT

private slots:
  void on_click(int i);
  void undo_slot();
  void redo_slot();
  void resign_slot();
  void reset_slot();
  void fairy_slot();
  
public:
  const int num_rows;
  const int num_cols;
  QGridLayout* layout;
  // std::vector<std::vector<QPushButton*>> *buttons;
  QPushButton* buttons[8][8];
  explicit ButtonGrid(int rs, int cs);
  QButtonGroup* button_group;

  QHBoxLayout *option_button_layout;
  QPushButton *reset ;
  QPushButton *resign ;
  QPushButton *undo ;
  QPushButton *redo ;
  QPushButton *fairy ;
  QHBoxLayout *scores_layout;
  QLabel* player_1_score;
  QLabel* player_2_score;
  QVBoxLayout *main_layout;
  
private:
  Model model;
  void render();
  
};

#endif
