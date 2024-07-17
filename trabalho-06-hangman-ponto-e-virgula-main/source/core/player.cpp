/*!
 * Player class implementation.
 *
 * \author Selan
 * \date April 21st, 2022
 * \file player.cpp
 */

#include <cstddef>
#include <vector>

#include "player.h"

// === Auxiliary functions to help user input

// === Regular methods of the Player class.

void Player :: name(std :: wstring name){m_name = name;}

void Player::add_score(size_t s) {m_score += s;}

void Player::add_word(const std::wstring &w) { m_played_words.insert(w); }

/// Check if this word has been played before.
bool Player::has_played(const std::wstring &w) const {
  return m_played_words.count(w) != 0;
}

void Player::clear_word_list() { m_played_words.clear();}

/// Reads the players txt file.
void Player :: read_file(std :: wifstream& file){
  std :: getline(file, m_name);
  file >> m_score;
  file >> m_easy;
  file >> m_medium;
  file >> m_hard;
  file >> m_words;
  file.ignore();
  m_played_words.clear();
  for (size_t i = 0; i < m_words; ++i) {
    std::wstring word;
    std::getline(file, word);
    m_played_words.insert(word);
  }
  file >> m_wins;
  file >> m_loses;
  file.ignore();
  std::wstring separator;
  std::getline(file, separator);
}

/// Write the players binary file.
void Player :: write_file(std :: wofstream& file) const{
  file << m_name << L'\n';
  file << m_score << L'\n';
  file << m_easy << L'\n';
  file << m_medium << L'\n';
  file << m_hard << L'\n';
  file << m_words << L'\n';
  for (const auto& word : m_played_words) {
    file << word << L'\n';
  }
  file << m_wins << L'\n';
  file << m_loses << L'\n';
  file << L"---" << L'\n';
}

std ::unordered_map<std :: wstring, Player> Player :: get_players(){
  std :: wifstream file("Players.txt");
  if (!file){ 
    std :: wcerr << L"Unable to open the file." << std :: endl;
    exit(1);
  }
  std :: unordered_map<std :: wstring, Player> player_list;
  while (file.peek() != WEOF){
    Player current_player;
    current_player.read_file(file);
    player_list.emplace(current_player.m_name, current_player); 
  }
  return player_list;
}

void Player :: find_player(const std :: unordered_map<std :: wstring, Player>& players){
  if (!players.empty()){
    auto it = players.find(m_name);
    if (it != players.end()){
      m_easy = it->second.m_easy;
      m_medium = it->second.m_medium;
      m_hard = it->second.m_hard;
      m_wins = it->second.m_wins;
      m_loses = it->second.m_loses;
      m_played_words = it->second.m_played_words;
      m_words = it->second.m_words;
      m_score = it->second.m_score;
    }
  }
}

void Player :: update_player(std :: unordered_map<std :: wstring, Player>& players) const{
  if (!players.empty()){
    auto it = players.find(m_name);
    if (it != players.end()){
      it->second.m_easy = m_easy;
      it->second.m_medium = m_medium;
      it->second.m_hard = m_hard;
      it->second.m_wins = m_wins;
      it->second.m_loses = m_loses;
      it->second.m_played_words = m_played_words;
      it->second.m_words = m_words;
      it->second.m_score = m_score;
    }
    else if(players.end() == it) {
      Player currentp;
      currentp.m_name = m_name;
      currentp.m_easy = m_easy;
      currentp.m_medium = m_medium;
      currentp.m_hard = m_hard;
      currentp.m_wins = m_wins;
      currentp.m_loses = m_loses;
      currentp.m_played_words = m_played_words;
      currentp.m_words = m_words;
      currentp.m_score = m_score;
      players.emplace(std :: make_pair(currentp.m_name, currentp));
    }
  }
}

void Player :: write_all(const std :: unordered_map<std :: wstring, Player>& players) const{
  std :: wofstream file("Players.txt", std::ios::trunc);
  if (!file) {
    std::wcerr << L"Unable to open the file." << std::endl;
    exit(1);
  }
  for (const auto& [name, player] : players){
    player.write_file(file);
  }
  file.close();
}

void Player :: decrease_score(size_t amount) {
  if (m_score < amount) {
    m_score = 0;
  } else {
    m_score -= amount;
  }
}