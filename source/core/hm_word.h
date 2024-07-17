#ifndef HM_WORD_H
#define HM_WORD_H
/*!
 * Hangman Word class
 * @file hm_word.h
 *
 * This class represents a Hangman Word, being responsible
 * for receiving a word, a (optional) list of letter tha
 * must be revealed, and creating the corresponding masked
 * string for display.
 *
 * This class also receives guesses, and keeps track of how
 * many of theses guesses were correct or wrong, so that at
 * the end we may retrieve this information for score
 * (performance) calculation.
 *
 * \author Selan
 * \date April 20th, 2022
 */

#include <string>
#include <vector>
#include <iostream>

class HangmanWord {
  //=== Data members or attributes.
private:
  std::wstring m_secret_word;               //!< The secret word to guess.
  std::wstring m_masked_word;               //!< The secret word with closed letters masked.
  std::wstring m_open_letters;              //!< List of letter to show at the start.
  long m_n_correct_guesses;                 //!< # of correct guesses made by the player.
  long m_n_wrong_guesses;                   //!< # of wrong guesses made by the player.
  wchar_t m_mask_char;                      //!< Char used as mask to hide letters in the secret word.
  std :: vector<wchar_t> m_wrong_guesses;   //!< List of wrong guesses made.
  std :: vector<wchar_t> m_correct_guesses; //!< List of correct guesses made.

  //=== Public types
public:
  /// Return types of the guess(char) method.
  enum class guess_e : short {
    CORRECT = 0, //!< Correct guess.
    WRONG,       //!< Wrong guess.
    REPEATED
  }; //!< This guess (wrong/correct) has been used before.

  //=== Public interface
  /// Default Ctro. Assumes input strings are all lower case.
  /*!
   * @param secret The secret word.
   * @param show List of letters in the secret word to show to player.
   * @param mask Char to use as mask.
   */
  HangmanWord(std::wstring secret = L"", std::wstring show = L"",
              wchar_t mask = L'_');
  /// Default Ctro.
  ~HangmanWord() = default;
  
  /// Initialize the object providing a (new) word, show letters and mask char.
  void initialize(const std::wstring &sw, const std::wstring &ol = L"",
                  wchar_t mch = L'_');
  
  /// Return a the secret word with the unguessed letters masked.
  [[nodiscard]] std :: wstring masked_str() const{return m_masked_word;};
  
  /**
  * @brief Check the guess and return the result.
  * 
  * @param g The guessed character.
  * @return guess_e The result of the guess.
  */
  guess_e guess(wchar_t g);

  /**
   * @brief Return the number of wrong guesses so far in the match.
   * 
   * @return The number of wrong guesses.
   */
  [[nodiscard]] size_t wrong_guesses() const{return m_n_wrong_guesses;};
  
  /**
   * @brief Return the number of correct guesses so far in the match.
   * 
   * @return The number of correct guesses.
   */
  [[nodiscard]] size_t correct_guesses() const{return m_n_correct_guesses;};
  
  /**
   * @brief Return the list of wrong guesses.
   * 
   * @return The list of wrong guesses.
   */
  [[nodiscard]] std :: vector<wchar_t> wrong_guesses_list() const{return m_wrong_guesses;};

  /**
   * @brief Return the list of correct guesses.
   * 
   * @return The list of correct guesses.
   */
  std :: vector<wchar_t> correct_guesses_list() const{return m_correct_guesses;};
  
  /**
   * @brief Return the number of masked letters in the secret word.
   * 
   * @return The number of masked characters.
   */
  [[nodiscard]] size_t n_masked_ch() const;
  
  /**
   * @brief Return the mask character.
   * 
   * @return The mask character.
   */
  [[nodiscard]] wchar_t mask_char() const;
  
  /**
   * @brief Returns the secret word.
   * 
   * @return The secret word.
   */
  std::wstring operator()() const{return m_secret_word;};
  
  /**
   * @brief Reset the object to its original state and mask the secret word.
   */
  void reset();
  
    /**
   * @brief Increment the number of correct guesses by one.
   */
  void add_n_correct_guess(){m_n_correct_guesses++;};
  
  /**
   * @brief Increment the number of wrong guesses by one.
   */
  void add_n_wrong_guess(){m_n_wrong_guesses++;};
  
  /**
   * @brief Add a guess to the wrong guess list.
   * 
   * @param guess The guessed character.
   */
  void add_wrong_guess(wchar_t );

  /**
   * @brief Add a guess to the correct guess list.
   * 
   * @param guess The guessed character.
   */
  void add_correct_guess(wchar_t);

  /**
   * @brief Generate the masked word by masking unguessed letters.
   */ 
  void make_masked_word();

  /// Get the choose word to be the secret word.
  void get_word(std :: wstring word){m_secret_word = word;};

  /**
   * @brief Reveal a masked character.
   * 
   * @param g The guessed character.
   */
  void unmasked_char(wchar_t);

  /// Return the secret word.
  std :: wstring secret_word() const{return m_secret_word;};

  /**
   * @brief Check if all characters are revealed.
   * 
   * @return true If all characters are revealed.
   * @return false If there are still masked characters.
   */
  bool all_unmasked();

  /**
   * @brief Reveal part of the secret word.
   */
  void reveal_part();

};

#endif
