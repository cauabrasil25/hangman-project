#ifndef _HANGMAN_GM_H_
#define _HANGMAN_GM_H_

#include <array>
#include <string> // std::string
#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include "hm_word.h"
#include "player.h"

/*!
 * This class represents the Game Controller which keeps track of player,
 * scores, and match total values, as well as determining when a match ends.
 */
class GameController {
private:
  //=== Structs
  //!< The game states.
  enum class game_state_e : short {
    STARTING = 0, //!< Beginning the game.
    WELCOME,      //!< Opening messasges.
    MAIN_MENU,    //!< Enter main menu mode, reading option from the player.
    PLAYING,      //!< Playing the game.
    SHOW_SCORE,   //!< Show top scores.
    SHOW_RULES,   //!< Asks user if s/he wants to read the rules.
    QUITTING,     //!< User might want to quit (before game finishes), need to
                  //!< confirm.
    DIFICULT,     //!< Enter dificult selection mode.
    ENDING,       //!< Closing the game (final message).
    NO_WORDS,     //!< Show the mensage that has no words to play.
  };

  //!< The menu options.
  enum class menu_e : short {
    PLAY = 1,  //!< Begin new game.
    RULES,     //!< Show rules of the game.
    SCORE,     //!< Show top scores.
    EXIT,      //!< Exit the game.
    DIFICULT,  //!< Difucult selection.
    UNDEFINED, //!< No option chosen.
  };

  //!< The match status.
  enum class match_e : short {
    ON = 1,      //!< Match still open and running.
    PLAYER_WON,  //!< Match ended and player WON.
    PLAYER_LOST, //!< Match ended and player LOST.
    UNDEFINED,   //!< Undefined match status.
  };

  //!< The dificult options
  enum class dificult_e : short {
    NORMAL = 0,   //!< Medium dificult with none letters reveal.
    EASY,     //!< Easiest dificult with some letters reveal.
    HARD,       //!< Hardest dificult with complex words.
  };

  //!< Struct with a word and its categories.
  struct Word {
    std :: wstring word;
    std :: vector<std :: wstring> categories;
  };

  //=== Data members
  game_state_e m_game_state; //!< Current game state.
  menu_e m_menu_option;      //!< Current menu option.
  dificult_e m_dificult = dificult_e::NORMAL;     //!< Current dificult.
  std::wstring m_system_msg; //!< Current system message displayed to user.
  bool m_asked_to_quit;      //!< Flag that indicates whether the user wants to end
                             //!< an ongoing game.
  bool m_match_ended;        //!< Flag that indicates whether the current match has
                             //!< ended or not.
  bool m_reveal_word;        //!< Flag that is active when user looses and we need to
                             //!< show the answer.
  bool m_asked_to_leave;     //!< Flag that is active when user wants to leave the match.
  bool m_repeated;           //!< Flag that is active when user insert a repeated word.
  bool m_digit;              //!< Flag that is active when user insert a digit.
  bool m_guess_all;          //!< Flag that is active when user wants to guess the entire word.
  
  //=== Game related members
  std::unordered_map<std::wstring, Player> m_players;         //!< List of players, indexed by name (must be unique).
  Player *m_curr_player;                                      //!< Reference to the current player.
  wchar_t m_ch_guess = 0;                                     //!< Latest player guessed letter.
  HangmanWord m_secret_word;                                  //!< Keeps track of the masked word, wrong guesses, etc.
  size_t m_max_mistakes = 6;                                  //!< Max number of mistakes allowed in a match.
  std::wstring m_user_name;                                   //!< Stores the user name provided in the Welcome state.
  size_t m_curr_word_idx;                                     //!< Index of the current secret word.
  match_e m_match;                                            //!< Current match state.
  std :: vector<Word> m_all_words;                            //!< List of all words and its categories.
  std :: vector<Word> m_easy_words;                           //!< List of easy words and its categories.
  std :: vector<Word> m_normal_words;                         //!< List of normal words and its categories.
  std :: vector<Word> m_hard_words;                           //!< List of hard words and its categories.
  std :: vector<std :: wstring> m_current_categories;         //!< List of current categories.

public:
  //=== Public interface
  GameController() = default;
  GameController(const GameController &) = delete;
  GameController(GameController &&) = delete;
  GameController &operator=(const GameController &) = delete;
  GameController &operator=(GameController &&) = delete;
  ~GameController() = default;

  //=== Common methods for the Game Loop design pattern.
  
  /**
   * @brief Renders the game to the user.
   */
  void render() const;

  /**
   * @brief Update the game based on the current game state.
   */
  void update();

  /**
   * @brief Process user input events, depending on the current game state.
   */
  void process_events();
 
  /**
   * @brief Check if the game is over.
   * @return true if the game is over, false otherwise.
   */
  bool game_over() const;

private:
  // === These read_xxx() methods are called in process_events()
  /// Reads the user command inside the action screen.
  
  /**
   * @brief Read the user's name from the input.
   * @return The user's name as a wide string.
   */
  std::wstring read_user_name();
  
  /**
   * @brief Read the user's confirmation (yes or no).
   * @return true if the user confirms, false otherwise.
   */
  bool read_user_confirmation() const;

  /**
   * @brief Read the user's input and wait for an enter key press.
   */
  void read_enter_to_proceed() const;
  
  /**
   * @brief Read the user's guess (single character).
   * @return The guessed character.
   */
  wchar_t read_user_guess();

  /**
   * @brief Read the user's full word guess.
   * @return The guessed word as a wide string.
   */
  std :: wstring read_user_word_guess();
  
  /**
   * @brief Read the user's menu option choice.
   * @return The chosen menu option as an enumerated type.
   */
  menu_e read_menu_option();

  /**
   * @brief Read the user's difficulty choice.
   * @return The chosen difficulty level as an enumerated type.
   */
  dificult_e read_dificult_option();

  // === These display_xxx() methods are called in render()
  
  /**
   * @brief Display the welcome screen.
   */
  void display_welcome() const;
 
  /**
   * @brief Display the main menu options.
   */
  void display_main_menu() const;
  
  /**
   * @brief Display the game screen during gameplay.
   */
  void display_play_screen() const;
 
  /**
   * @brief Display the exit confirmation screen.
   */
  void display_quitting() const;
  
  /**
   * @brief Display the game rules.
   */
  void display_rules() const;
 
  /**
   * @brief Display the endgame screen.
   */
  void display_endgame() const;
  
  /**
   * @brief Display the scoreboard.
   */
  void display_scoreboard() const;

  /**
   * @brief Display the difficulty selection menu.
   */
  void display_dificult() const;

  /// Show hangman.
  void display_hangman() const;

  /**
   * @brief Display the scoreboard.
   */
  void display_no_words() const; 

  /**
   * @brief Sort the players by score.
   * 
   * @return A vector of pairs containing player names and Player objects sorted by score.
   */
  std :: vector<std :: pair<std :: wstring, Player>> sort_players() const;

  // === These show_xxx() methods display common elements to every screen.
  /* All screens may have up to 4 components:
   *  (1) title,                  -> must have
   *  (2) main content,           -> optional
   *  (3) a system message,       -> optional
   *  (4) an interaction message. -> optional
   */
  
  /// Evaluates the match situation, whether it's over or if it still on.
  match_e match_status() const;
  
  /**
   * @brief Show the gallows with the hangman, whose body displayed depends on the
   * number of mistakes made (argument).
   * 
   * @param mistakes The number of mistakes made, determining the hangman's display.
   */
  void display_gallows() const;
  
  /**
   * @brief Reset the match to its initial state.
   */
  void reset_match();
  
  /**
   * @brief Choose a word for the current match.
   * @return The chosen word as a wide string.
   */
  std::wstring choose_word();

  /**
   * @brief Read words from the words file.
   */
  void read_words_file();

  /**
   * @brief Separate words into different categories or groups.
   */
  void separate_words();

};
#endif
