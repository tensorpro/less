#include "chess.hpp"
#include <unordered_map>
#include <iostream>
/*

From https://stackoverflow.com/questions/15160889/how-to-make-unordered-set-of-pairs-of-integers-in-c
 */

// Displacements are in form y, x or col, row
Displacement U =  Displacement(-1,0);
Displacement D =  Displacement(1,0);
Displacement L =  Displacement(0,-1);
Displacement R =  Displacement(0,1);
Displacement UL = Displacement(-1,-1);
Displacement UR = Displacement(-1,1);
Displacement DL = Displacement(1,-1);
Displacement DR = Displacement(1,1);


std::vector<Displacement> STRAIGHT = {U,D,L,R};
std::vector<Displacement> DIAGONAL = {UL,DL,UR,DR};
std::vector<Displacement> ALL = {UL,DL,UR,DR,U,D,L,R};
std::vector<Displacement> DOWN = {D};
std::vector<Displacement> UP = {U};
std::vector<Displacement> UPWARDS = {U, UL, UR};
std::vector<Displacement> DOWNWARDS = {D, DL, DR};

std::vector<Displacement> Ls = {Displacement(1,2),
				Displacement(1,-2),
				Displacement(-1,2),
				Displacement(-1,-2),
				Displacement(2,1),
				Displacement(2,-1),
				Displacement(-2,1),
				Displacement(-2,-1)};

PieceType :: PieceType(Name n, Movement m):
  name{n}
  , black_movement{m}
  , white_movement{m}
{};

/**
 * A function to return whether an integer is within a range.
 * @param x The number we want to check
 * @param lower The lower bound for x
 * @param upper The upper bound for x
 * @return whether lower <= x <= upper
 */
bool in_range(int x, int lower=0, int upper=7){
  return x >= lower && x<= upper;
}


/**
 * A function to return whether a position is in the board.
 * @param p the position we want to check
 * @return whether the position is on the board
 */
bool Board::valid_pos(Pos p){
  return in_range(p.first) && in_range(p.second);
}


/**
 * A function to move the piece at Pos start, to the Pos end
 * @param start the position of the piece that should be moved
 * @param end the position the piece shoudl be moved too
 */
void Board::move_piece(Pos start, Pos end){
  if(!valid_pos(start) || !valid_pos((end))) return;
  Piece* p1 = board[start.first][start.second];
  if(p1!=nullptr){
    board[end.first][end.second] = board[start.first][start.second];
    board[start.first][start.second] = nullptr;
  }
}

/**
 * Returns the Piece* at a specified location.
 * @param p the position we query for a piece, a pair of ints of the form <x,y>
 * @return the piece at the position
 */
Piece* Board::get_piece(Pos p){
  return board[p.first][p.second];
}

/**
 * A function to flip Color between black and white
 * @param c the color to be flipped
 * @return the flipped color
 */
Color other_color(Color c){
  return c == BLACK ? WHITE : BLACK;
}


Game::Game(bool fairy): board{fairy}, move{WHITE} {};
Color Game::get_turn(){return move;}
void Game::end_turn(){move = other_color(move);}

/**
 * A function to return all positions a player of given color can move pieces to.
 * @param g the game we are checking for movements in.
 * @param c the color of the player of interest
 * @return a set of all positions the player of color c can move pieces too
 */
MoveSet* all_moves(Game g, Color c){
  MoveSet* all_move_set = new MoveSet;
  bool flipping = c!=g.get_turn();
  if(flipping) g.end_turn();
  for(int i = 0; i < 8; ++i){
    for(int j = 0; j < 8; ++j){
      Pos pos {i,j};
      Piece* piece = g.board.get_piece(pos);
      if(piece!=nullptr && piece->color==c){
	MoveSet* moves = piece->possible_moves(g, pos);
	all_move_set->insert(moves->begin(), moves->end());
	delete moves;
      }
    }
  }
  if(flipping) g.end_turn();
  return all_move_set;
}



/**
 * A function to return true when the move will not endanger the king
 * @param g the current game
 * @param p1 the position of piece to be moved
 * @param p2 the position to be moved too
 */
bool safe_move(Game g, Pos p1, Pos p2){
  Color player = g.get_turn();
  Color opponent = other_color(player);
  g.board.move_piece(p1, p2);
  MoveSet* opponent_moves = all_moves(g, opponent);
  
  for(auto pos = opponent_moves->begin(); pos != opponent_moves->end(); ++pos){
    Piece* piece = g.board.get_piece(*pos);
    if(piece!=nullptr && piece->color==player && piece->name==KING){
      return false;
    }
  }
  return true;
}


/**
 * A function to return whether a color has any valid moves
 * @param g the current game
 * @param c the color being tested
 * @return whether c has any legal/safe moves
 */
bool has_possible_moves(Game g, Color c){
  for(int i=0; i<8;i++){
    for(int j=0;j<8;j++){
      Pos pos = Pos{i,j};
      Piece* piece = g.board.get_piece(pos);
      if(piece!=nullptr && piece->color==c){
	MoveSet* moves = piece->possible_moves(g, pos);
	for(auto pos2 = moves->begin(); pos2 != moves->end(); ++pos2){
	  if(safe_move(g, pos,*pos2)) return true;
	}
      }
    }
  }
  return false;
}


/**
 * A function to return whether the current player has any moves
 * @param g the current game
 * @return whether the current current player is in checkmate
 */
bool in_checkmate(Game g){
  Color c = g.get_turn();
  return !has_possible_moves(g, c) && has_possible_moves(g, other_color(c));
}

/**
 * A function to return whether the game is in a draw
 * @param g the current game
 * @return whether the game is in a draw
 */
bool in_draw(Game g){
  Color c = g.get_turn();
  return !has_possible_moves(g, c) && !has_possible_moves(g, other_color(c));
}

/**
 * A constructor for PieceTypes with different white/black movements
 * @param n name of the piece
 * @param wm A movement function for the white player
 * @param bm A movement function for the black player
 */
PieceType :: PieceType(Name n, Movement wm, Movement bm):
  name{n}, black_movement{bm}, white_movement{wm} {};

/**
 * A constructor for PieceTypes where both black/white have the same movement
 * @param n name of the piece
 * @param m A movement function for the piece
 */
Piece :: Piece(Name n, Color c, Movement& m):
  name{n}, color{c}, possible_moves{m} {};

/**
 * A member function for PieceTypes which makes a Piece of a given color
 * @param c color of the piece to be created
 */
Piece* PieceType :: create(Color c){
  Movement m = c== WHITE ? white_movement : black_movement;
  return new Piece(name, c, m);
}

/**
 * Returns whether there is a piece at pos in a given game
 * @param g a game
 * @param pos the position being tested
 */
bool pos_has_piece(Game g, Pos pos){
  return g.board.get_piece(pos)!=nullptr;
}

/**
 * Returns whether there the location at a pos is empty in a given game
 * @param g a game
 * @param pos the position being tested
 */
bool pos_is_empty(Game g, Pos pos){
  return !pos_has_piece(g,pos);
}

/**
 * Returns whether a pos has an enemy piece
 * It is used to determine when a move will result in a capture.
 * @param g a game
 * @param pos the position being tested
 */
bool capture_piece(Game g, Pos end){
  Color turn = g.get_turn();
  Piece* piece = g.board.get_piece(end);
  return (piece!=nullptr) && piece->color ==turn;
}

/**
 * A function that returns an unordered_set of positions that can be moved too with repeated
 * displacements specified in ds.
 * @param ds A vector of displacements
 * @param max_steps the number of times the displacement can be reapplied, by default its -1.
    With negative values, it will displace till it reaches end of the board.
 */
MoveSet* move_direction(Game g, Pos p, std::vector<Displacement> ds, int max_steps,
			StopCondition should_stop){
  MoveSet* s = new MoveSet;
  auto [original_x, original_y] = p;
  for(Displacement d:ds){
    Pos moving_to {original_x, original_y};
    auto [dx, dy] = d;
    for(int step=0; step!=max_steps; step++){
      auto [x, y] = moving_to;
      moving_to = Pos{x+dx, y+dy};
      if(!g.board.valid_pos(moving_to))	break;
      Piece* piece = g.board.get_piece(moving_to);
      if((piece!=nullptr && piece->color==g.get_turn()) || should_stop(g, moving_to)){
      	break;
      }
      if(auto [new_x, new_y] = moving_to; new_x!=original_x || new_y!=original_y){
      	s->insert(moving_to);
      }
      if(piece!=nullptr){
	break;
      }
    }
  }
  return s;
}


//https://stackoverflow.com/questions/21956119/add-stdpair-with-operator
std::pair<int, int> operator +(const std::pair<int, int>& x, const std::pair<int, int>& y) {
    return std::make_pair(x.first + y.first, x.second + y.second);
}

/*
 * Makes a function that gives pawn movement, given the start row and direction.

 */
auto pawn_movement(int start_row, Displacement direction){
  return [start_row, direction](Game g, Pos p){
    int steps = p.first==start_row ? 2 : 1;
    MoveSet* moves = move_direction(g, p, {direction}, steps, pos_has_piece);
    MoveSet* attacks = move_direction(g, p, {direction+L, direction+R}, 1, pos_is_empty);
    for(auto a: *attacks) moves->insert(a);
    delete attacks;
    return moves;
  };
}

Movement white_pawn_movement = pawn_movement(1, D);
Movement black_pawn_movement = pawn_movement(6, U);

Movement directional_movement(std::vector<Displacement> disps, int max_steps){
  return [disps, max_steps](Game g, Pos p){
    return move_direction(g, p, disps, max_steps);
  };
}

PieceType king = PieceType(KING, directional_movement(ALL, 1));
PieceType queen = PieceType(QUEEN, directional_movement(ALL));
PieceType rook = PieceType(ROOK, directional_movement(STRAIGHT));
PieceType bishop = PieceType(BISHOP, directional_movement(DIAGONAL));
PieceType knight = PieceType(KNIGHT, directional_movement(Ls, 1));
PieceType pawn = PieceType(PAWN, white_pawn_movement, black_pawn_movement);
// PieceType pawn = PieceType(PAWN, directional_movement(ALL, 8), black_pawn_movement);
PieceType paladin = PieceType(PALADIN, directional_movement(Ls, 2));
PieceType coward = PieceType(COWARD,
			     directional_movement(DOWNWARDS),
			     directional_movement(UPWARDS));
PieceType samurai = PieceType(SAMURAI,
			      directional_movement(DOWNWARDS),
			      directional_movement(UPWARDS));

using PieceMap = std::unordered_map<char,PieceType>;
char icons_[2][8][4] = {{"♜","♞","♝","♛","♚","♟"},
			{"♖","♘","♗","♕","♔","♙"}};

void show_board(Board board){
    for(int r=0;r<8;r++){
      for(int c=0;c<8;c++){
	Piece* piece = board.get_piece(Pos{r,c});
	if(piece==nullptr){
	  std::cout << " ";
	}
	else std::cout << icons_[piece->color][piece->name];
      }
      std::cout << "\n";
    }}

Board::Board(bool fairy){
  PieceType fairy_pieces[] = {samurai, paladin, bishop, queen, king, bishop, paladin, samurai};
  PieceType standard_pieces[] = {rook, knight, bishop, queen, king, bishop, knight, rook};
  PieceType pieces[8] = fairy? fairy_pieces : standard_pieces;
    for (int i=0;i<8;i++){
      board[0][i]=pieces[i].create(WHITE);
      board[7][i]=pieces[i].create(BLACK);
      board[1][i]=pawn.create(WHITE);
      board[6][i]=pawn.create(BLACK);
    }
    show_board(*this);
}

Pos invalid{-1,-1};
Model :: Model():
  game{}
  , selected{false}
  , selected_pos{invalid}
  , selected_moves{nullptr}
  , undo_history{}
  , redo_history{}
  , scores{0}
  , help{false}
{}


void Model :: deselect_piece(){
  
  selected_pos = Pos{invalid};
  delete (selected_moves);
  selected=false;
}

void Model :: select_piece(Pos pos, Piece* piece){
  if(selected) deselect_piece();
  selected_pos = Pos{pos};
  Movement m = piece->possible_moves;
  selected_moves = m(game, pos);
  selected=true;
}

void Model :: move_selected_piece(Pos pos){
  game.board.move_piece(selected_pos, pos);
  deselect_piece();
  show_board(game.board);
  game.end_turn();
}

void Model :: update_game(Pos pos){
  Piece* piece = game.board.get_piece(pos);
  if(selected){
    if(pos==selected_pos){
      deselect_piece();
      return;
    }
    bool valid_move = !selected_moves->empty() &&
      (selected_moves->find(pos)) != selected_moves->end();
    bool safe_and_valid_move = valid_move && safe_move(game, selected_pos, pos);
    if(safe_and_valid_move){
      Game snapshot {game};
      undo_history.push(snapshot);
      redo_history = std::stack<Game>{};
      move_selected_piece(pos);

      if(in_checkmate(game)){
	scores[other_color(game.get_turn())]+=1;
      }
      return;
    }
  }
  if(piece!=nullptr && piece->color == game.get_turn()){
    select_piece(pos, piece);
  }
}


void Model :: stack_shift(std::stack<Game> & shift_from, std::stack<Game> & shift_to){
  if(selected) deselect_piece();
  if(!shift_from.empty()){
    Game snapshot {shift_from.top()};
    shift_to.push(game);
    game = snapshot;
    shift_from.pop();
  }
}

void Model :: undo(){
  stack_shift(undo_history, redo_history);
}

void Model :: redo(){
  stack_shift(redo_history, undo_history);
}


void Model :: reset(){
  game = Game{};
  undo_history = std::stack<Game>{};
  redo_history = std::stack<Game>{};
}

void Model :: resign(){
  if(!in_checkmate(game))
  scores[other_color(game.get_turn())]+=1;
  reset();
}

int Model :: get_score(int player){
  if(!in_range(player,0,1)) return -1;
  return scores[player];
}

bool Model :: get_help(){
  return help;
}

bool Model :: is_new_game(){
  return undo_history.empty();
}

void Model :: fairy(){
  if(undo_history.empty()){
    game=Game{true};
  }
  else{
    help=!help;
  }
}
