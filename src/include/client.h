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
  // Initialize game_map if needed
  if (game_map.empty() || game_map.size() != rows) {
    game_map.assign(rows, std::vector<char>(columns, '?'));
  }

  // Read the current map state
  for (int i = 0; i < rows; i++) {
    std::string row;
    std::cin >> row;
    for (int j = 0; j < columns; j++) {
      game_map[i][j] = row[j];
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
  // Simple strategy: Look for obvious moves first

  // 1. Check for cells where we can auto-explore (all mines marked)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_map[i][j] >= '1' && game_map[i][j] <= '8') {
        int num = game_map[i][j] - '0';
        int marked = 0;

        // Count marked mines around
        for (int di = -1; di <= 1; di++) {
          for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns && game_map[ni][nj] == '@') {
              marked++;
            }
          }
        }

        // If all mines are marked, auto-explore
        if (marked == num) {
          Execute(i, j, 2);
          return;
        }
      }
    }
  }

  // 2. Check for cells where we can mark mines (number equals unknown neighbors)
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_map[i][j] >= '1' && game_map[i][j] <= '8') {
        int num = game_map[i][j] - '0';
        int unknown = 0;
        int marked = 0;
        int first_unknown_r = -1, first_unknown_c = -1;

        // Count unknown and marked around
        for (int di = -1; di <= 1; di++) {
          for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (game_map[ni][nj] == '?') {
                unknown++;
                if (first_unknown_r == -1) {
                  first_unknown_r = ni;
                  first_unknown_c = nj;
                }
              } else if (game_map[ni][nj] == '@') {
                marked++;
              }
            }
          }
        }

        // If remaining mines equals unknowns, mark them
        if (num - marked == unknown && unknown > 0) {
          Execute(first_unknown_r, first_unknown_c, 1);
          return;
        }
      }
    }
  }

  // 3. Find any unknown cell adjacent to a visited cell
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_map[i][j] == '?') {
        // Check if adjacent to any visited cell
        for (int di = -1; di <= 1; di++) {
          for (int dj = -1; dj <= 1; dj++) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (game_map[ni][nj] >= '0' && game_map[ni][nj] <= '8') {
                // Adjacent to a number, visit this cell
                Execute(i, j, 0);
                return;
              }
            }
          }
        }
      }
    }
  }

  // 4. Visit any unknown cell
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_map[i][j] == '?') {
        Execute(i, j, 0);
        return;
      }
    }
  }

  // This should never happen, but just in case - visit a random cell
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      if (game_map[i][j] == '?') {
        Execute(i, j, 0);
        return;
      }
    }
  }

  // If we get here, something is wrong - just visit 0,0
  Execute(0, 0, 0);
}

#endif