#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <cstddef>
#include <set>
#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>

//#include "hangman_common.h"

/// Representing a single player.
class Player {
  //=== Private members.
private:
  
  std::wstring m_name;                   //!< The player's name
  size_t m_score{};                      //!< The player's total score.
  std::set<std::wstring> m_played_words; //!< List of words played in a game.
  size_t m_medium{};                     //!< Number of games played on medium.
  size_t m_easy{};                       //!< Number of games played on easy.
  size_t m_hard{};                       //!< Number of games played on hard.
  size_t m_words{};                      //!< Number of words played.
  size_t m_wins{};                       //!< Number of wins.
  size_t m_loses{};                      //!< Number of loses.

  //=== Public interface
public:
  /// Default Ctro.
  Player(std::wstring n = L"no_name") : m_name{std::move(n)} { /*empty*/  }
  
  /// Dtro
  ~Player() = default;
  
  /**
   * @brief Set the name of the player.
   * 
   * @param name The name to set for the player.
   
   */
  std :: wstring name() const{return m_name;};
  
  /**
   * @brief Add a score to the player's current score.
   * 
   * @param s The score to add.
   */
  void name(std::wstring name);
  
  
  [[nodiscard]] size_t score() const{return m_score;};
  
  /**
   * @brief Add a score to the player's current score.
   * 
   * @param s The score to add.
   */
  void add_score(size_t s);
 
  /**
   * @brief Add a word to the list of words played by the player.
   * 
   * @param w The word to add.
   */
  void add_word(const std::wstring & w);
  
  /**
   * @brief Check if the player has played a specific word before.
   * 
   * @param w The word to check.
   * @return true if the player has played the word before, false otherwise.
   */
  bool has_played(const std::wstring &) const;

  /**
   * @brief Clear the list of words played by the player.
   */
  void clear_word_list();

  /**
   * @brief Add a word to the number of played words.
   */
  void add_n_words(){m_words++;};

  /**
   * @brief Return the number of words played by the player.
   * 
   * @return The number of words played.
   */
  size_t n_words() const{return m_words;};

  /**
   * @brief Returns the player's accumulated easy mode played.
   * 
   * @return The number of easy mode games played.
   */
  size_t easy_played() const{return m_easy;};

  /**
   * @brief Returns the player's accumulated normal mode played.
   * 
   * @return The number of normal mode games played.
   */
  size_t normal_played() const{return m_medium;};

  /**
   * @brief Returns the player's accumulated hard mode played.
   * 
   * @return The number of hard mode games played.
   */
  size_t hard_played() const{return m_hard;};
  
  /**
   * @brief Returns the player's accumulated wins.
   * 
   * @return The number of wins.
   */
  size_t n_wins() const{return m_wins;};

  /**
   * @brief Returns the player's accumulated loses.
   * 
   * @return The number of loses.
   */
  size_t n_loses() const{return m_loses;};

  /**
   * @brief Returns the set of words played by the player.
   * 
   * @return The set of played words.
   */
  std :: set<std :: wstring> get_played_words() const{return m_played_words;};

  /**
   * @brief Increases the number of games played on easy difficulty.
   */
  void add_easy_played(){m_easy++;};
  
  /**
   * @brief Increases the number of games played on medium difficulty.
   */
  void add_medium_played(){m_medium++;};

  /**
   * @brief Increases the number of losses.
   */
  void add_hard_played(){m_hard++;};//done

  /**
   * @brief Increases the number of wins.
   */
  void add_wins(){m_wins++;};

  /**
   * @brief Increases the number of losses.
   */
  void add_loses(){m_loses++;};

  /**
   * @brief Read player data from a text file.
   * 
   * @param file The input file stream to read from.
   */
  void read_file(std::wifstream& file);

  /**
   * @brief Write player data to a text file.
   * 
   * @param file The output file stream to write to.
   */
  void write_file(std :: wofstream& file) const;

  /**
   * @brief Retrieve all players from a text file.
   * 
   * @return An unordered map containing player names as keys and Player objects as values.
   */
  std :: unordered_map<std :: wstring, Player> get_players();

  /**
   * @brief Find a player in a map of players and update the current player's data if found.
   * 
   * @param players The map of players to search within.
   */
  void find_player(const std :: unordered_map<std :: wstring, Player>&);

  /**
   * @brief Update a player's data in a map of players.
   * 
   * @param players The map of players to update.
   */
  void update_player(std :: unordered_map<std :: wstring, Player>&) const;

  /**
  * @brief Write all players from a map of players to a text file.
  * 
  * @param players The map of players to write.
  */
  void write_all(const std :: unordered_map<std :: wstring, Player>&) const;

  /**
   * @brief Increase the player's score by a specified amount.
   * 
   * @param amount The amount to decrease the score by.
   */
  void increase_score(size_t amount){m_score += amount;};

  /**
   * @brief Decrease the player's score by a specified amount.
   * 
   * @param amount The amount to decrease the score by.
   */
  void decrease_score(size_t amount);

};
#endif
