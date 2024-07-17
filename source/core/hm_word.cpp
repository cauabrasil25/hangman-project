/*!
 * Hangman Word class
 * @file hm_word.cpp
 *
 * Class implementation.
 *
 * \author Selan
 * \date April 20th, 2022
 */

#include <cwctype>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <numeric>

#include "hm_word.h"

/**
 * @brief Construct a new HangmanWord object.
 * 
 * @param secret The secret word to guess.
 * @param show The characters to reveal initially.
 * @param mask The character used to mask unrevealed letters.
 */
HangmanWord::HangmanWord(std::wstring secret, std::wstring show, wchar_t mask)
    : m_secret_word(std::move(secret)),
      m_masked_word(m_secret_word),
      m_open_letters(std::move(show)),
      m_n_correct_guesses(0),
      m_n_wrong_guesses(0),
      m_mask_char(mask) {
    for (auto &ch : m_masked_word) {
        if (m_open_letters.find(ch) == std::wstring::npos) {
            ch = m_mask_char;
        }
    }
    m_wrong_guesses = {};
    m_correct_guesses = {};
}

/// Return the number of masked lettes in the secret word.
[[nodiscard]] size_t HangmanWord :: n_masked_ch() const{
    short j = 0;
    for (wchar_t c : m_masked_word){
        if (c == L'_'){j++;}
    }
    return j;
}

/// Reset the object to its original state and mask the secret word.
void HangmanWord::reset() {
    m_masked_word = m_secret_word;
    for (auto &ch : m_masked_word) {
        if (m_open_letters.find(ch) == std::wstring::npos) {ch = m_mask_char;}
    }
    m_wrong_guesses.clear();
    m_n_correct_guesses = 0;
    m_n_wrong_guesses = 0;
    m_correct_guesses.clear();
}
  
HangmanWord :: guess_e HangmanWord :: guess(wchar_t g){
    for (size_t i = 0; i < m_wrong_guesses.size(); i++){
        if (m_wrong_guesses[i] == g){return guess_e :: REPEATED;}
    }
    for (size_t i = 0; i < m_correct_guesses.size(); i++){
        if (m_correct_guesses[i] == g){return guess_e :: REPEATED;}
    }
    for (wchar_t c : m_secret_word){
        if (c == g){
            return guess_e :: CORRECT;
        }
    }
    return guess_e :: WRONG;
}

/// Add a guess to wrong guess list.
void HangmanWord :: add_wrong_guess(wchar_t guess){
    m_wrong_guesses.push_back(guess);
    add_n_wrong_guess();
}

/// Add a guess to correct guess list.
void HangmanWord :: add_correct_guess(wchar_t guess){
    m_correct_guesses.push_back(guess);
    add_n_correct_guess();
}

/// Return a the secret word with the unguessed letters masked.
void HangmanWord :: make_masked_word(){
    std :: wstring masked_word = L""; 
    for (wchar_t c : m_secret_word){
        if (c != L'\0'){masked_word = masked_word + L"_" + L" ";}
        else {masked_word = masked_word + L" ";}
    }
    m_masked_word = masked_word;
}

/// Reveal a masked char.
void HangmanWord :: unmasked_char(wchar_t g){
    for (size_t i = 0; i < m_secret_word.size(); i++){
        if (m_secret_word[i] == g){m_masked_word[i * 2] = g;}
    }
}

/// Check if all chars are revealed.
bool HangmanWord :: all_unmasked(){
    for(wchar_t c : m_masked_word){
        if (c == L'_'){return false;}
    }
    return true;
}

void HangmanWord :: reveal_part(){
    size_t t = m_secret_word.size();
    size_t n_reveals = static_cast<size_t>(t * 0.2);
    std::vector<size_t> indices(t); 
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
    for (size_t i = 0; i < n_reveals && i < indices.size(); ++i) {
        size_t idx = indices[i];
        m_masked_word[2 * idx] = m_secret_word[idx]; 
        m_open_letters.push_back(m_secret_word[idx]); 
    }
}