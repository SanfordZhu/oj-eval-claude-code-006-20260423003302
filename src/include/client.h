#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.

// Global variables for game state
std::vector<std::vector<char>> game_map;  // Current state of the map
std::vector<std::vector<bool>> is_mine;   // Whether we think a cell is a mine
std::vector<std::vector<bool>> is_safe;   // Whether we know a cell is safe
std::set<std::pair<int, int>> unknown_cells;  // Cells we haven't explored yet

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // TODO (student): Initialize all your global variables!
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  // Initialize or resize game_map if needed
  if (game_map.empty() || game_map.size() != rows) {
    game_map.assign(rows, std::vector<char>(columns, '?'));
    is_mine.assign(rows, std::vector<bool>(columns, false));
    is_safe.assign(rows, std::vector<bool>(columns, false));
    unknown_cells.clear();
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < columns; j++) {
        unknown_cells.insert({i, j});
      }
    }
  }

  // Read the current map state
  for (int i = 0; i < rows; i++) {
    std::string row;
    std::cin >> row;
    for (int j = 0; j < columns; j++) {
      game_map[i][j] = row[j];
      // Update our knowledge
      if (row[j] >= '0' && row[j] <= '8') {
        // This cell is visited and has a number
        is_safe[i][j] = true;
        unknown_cells.erase({i, j});
      } else if (row[j] == '@') {
        // This cell is marked as a mine
        is_mine[i][j] = true;
        unknown_cells.erase({i, j});
      } else if (row[j] == '?') {
        // Still unknown
      }
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // First, try to find definite safe cells or mines
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_map[i][j] >= '0' && game_map[i][j] <= '8') {
        int num = game_map[i][j] - '0';

        // Count adjacent unknowns and marked mines
        int unknown_count = 0;
        int marked_count = 0;
        std::vector<std::pair<int, int>> unknown_neighbors;

        for (int di = -1; di <= 1; di++) {
          for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (game_map[ni][nj] == '?') {
                unknown_count++;
                unknown_neighbors.push_back({ni, nj});
              } else if (game_map[ni][nj] == '@') {
                marked_count++;
              }
            }
          }
        }

        // If all mines are marked, auto-explore
        if (marked_count == num && unknown_count > 0) {
          Execute(i, j, 2);  // Auto explore
          return;
        }

        // If number of unknowns equals remaining mines, mark them all
        int remaining_mines = num - marked_count;
        if (remaining_mines == unknown_count && unknown_count > 0) {
          // Mark the first unknown neighbor
          Execute(unknown_neighbors[0].first, unknown_neighbors[0].second, 1);
          return;
        }
      }
    }
  }

  // If no definite moves, try to find a safe cell to visit
  // Prefer cells adjacent to visited cells with low numbers
  std::vector<std::pair<int, int>> candidates;
  int best_score = -1;

  for (const auto& cell : unknown_cells) {
    int r = cell.first, c = cell.second;
    int score = 0;

    // Check all adjacent visited cells
    for (int di = -1; di <= 1; di++) {
      for (int dj = -1; dj <= 1; dj++) {
        if (di == 0 && dj == 0) continue;
        int ni = r + di, nj = c + dj;
        if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
          if (game_map[ni][nj] >= '0' && game_map[ni][nj] <= '8') {
            // Adjacent to a number - safer than random
            score += 10;
            // Lower numbers are safer
            score += (8 - (game_map[ni][nj] - '0'));
          }
        }
      }
    }

    // Prefer cells with higher scores
    if (score > best_score) {
      best_score = score;
      candidates.clear();
      candidates.push_back({r, c});
    } else if (score == best_score) {
      candidates.push_back({r, c});
    }
  }

  // If we found good candidates, pick one
  if (!candidates.empty()) {
    auto& choice = candidates[0];
    Execute(choice.first, choice.second, 0);
    return;
  }

  // Last resort: pick any unknown cell
  if (!unknown_cells.empty()) {
    auto& choice = *unknown_cells.begin();
    Execute(choice.first, choice.second, 0);
    return;
  }
}

#endif