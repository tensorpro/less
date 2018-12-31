#ifndef CHESS_H
#define CHESS_H
#include <iostream>
// #include <tuple>
#include <unordered_set>
#include <functional>
#include <vector>
#include <stack>


class Game;
using Pos = std::pair<int,int>;
using Displacement = std::pair<int, int>;
// struct pair_hash;

/**
 * A function to hash a pair<int,int>
 * @param v a pair to hash.
 * @return A hash value
 */
struct pair_hash {
    inline std::size_t operator()(const std::pair<int,int> & v) const {
        return v.first*64+v.second;
    }
};


//https://stackoverflow.com/questions/33597127/creating-an-stdunordered-map-with-an-stdpair-as-key
template <class T, typename U>
struct PairEqual{
    bool operator()(const std::pair<T, U> &lhs, const std::pair<T, U> &rhs) const{
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};


using MoveSet = std::unordered_set<Pos, pair_hash, PairEqual<int,int>>;
using Movement = std::function<MoveSet*(Game, Pos)>;

enum Name {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN, PALADIN, COWARD, SAMURAI, NUM_PIECES};

enum Color {BLACK, WHITE};
enum GameState {NEW, MIDGAME, CHECKMATE};


MoveSet* all_moves(Game, Color);

/**
 *  A piece class. Used to describe the movements, color and type of a speceific piece on the board.

 */
class Piece{
public:
  Piece(Name, Color, Movement &);
  const Name name;
  const Color color;
  const Movement possible_moves;
};


/**
 * Used to generate pieces of the same type. For example, the types rook/queen will be instances of 
 * the PieceType class. This class can be used to generate specific `Pieces` of the PieceType.
 */
class PieceType {
public:
  PieceType(Name, Movement);
  PieceType(Name, Movement, Movement);
  Piece* create(Color);
private:
  Name name;
  Movement black_movement;
  Movement white_movement;
};

/**
 * A class for the board, that helps track whether positions are valid positions on the board.
 * It also handles the movement of specific pieces.
 */
class Board{
public:
  Board(bool fairy=false);
  Piece *get_piece(Pos);
  void move_piece(Pos, Pos);
  bool valid_pos(Pos);
private:
  Piece* board[8][8] {nullptr};
};


/**
 * Used to keep track of the state of the current game.
 * It will track the turn, as well as store an internal `Board`.
 */
class Game{
public:
  Game(bool fairy=false);
  // Piece get_piece(std::pair<int,int>);
  Color get_turn();
  void end_turn();
  Board board;
private:
  Color move;
};

class Model{
public:
  Model();
  Game game;

  bool selected;
  Pos selected_pos;
  MoveSet* selected_moves;
  void update_game(Pos);
  void select_piece(Pos, Piece*);
  void deselect_piece();
  void move_selected_piece(Pos);
  void undo();
  void redo();
  void resign();
  void reset();
  void fairy();
  int get_score(int);
  bool get_help();
  bool is_new_game();
private:
  void stack_shift(std::stack<Game> &, std::stack<Game> &);
  std::stack<Game> undo_history;
  std::stack<Game> redo_history;
  int scores[2];
  bool help;

};

using StopCondition = std::function<bool(Game, Pos)>;

bool capture_piece(Game, Pos);
Movement directional_movement(std::vector<Displacement>, int max_steps = -1);
MoveSet* move_direction(Game g, Pos p, std::vector<Displacement> ds, int max_steps,
			StopCondition should_stop=capture_piece);
bool has_possible_moves(Game g, Color c);
Color other_color(Color c);
std::vector<Pos>* convert_set(MoveSet);

// PieceType pawn = PieceType(PAWN, directional_movement(ALL));
extern PieceType king;
extern PieceType queen;
extern PieceType rook;
extern PieceType bishop;
extern PieceType knight;
extern PieceType pawn;
extern PieceType paladin;
extern PieceType coward;
extern PieceType samurai;


#endif
