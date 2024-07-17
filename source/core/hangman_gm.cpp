/*!
 * Game Controller implementation.
 *
 * \author Selan
 * \date March 23rd, 2024
 * \file hangman_gm.cpp
 */
#include <cassert>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <locale>
#include <random>
#include <cstdlib>
#include <iomanip>
#include <utility>

//#include "../utils/text_color.h"
#include "hangman_gm.h"
#include "hm_word.h"

// Fixed list of word.
static std::array<std::wstring, 14> list_of_words{
    L"mestre de obras", // TODO: support multiple words as secret guess
    L"advogado",        L"arquiteto",  L"carpinteiro", L"caçador",
    L"enfermeira",      L"engenheira", L"gari",        L"policial",
    L"porteiro",        L"professora", L"psicologo",   L"secretaria",
    L"soldado",
};

//=== Common methods for the Game Loop design pattern.
/// Renders the game to the user.
void GameController :: render() const{
    switch(m_game_state) {
        case game_state_e :: STARTING:
            break;
        case game_state_e :: WELCOME:
            display_welcome();
            break;
        case game_state_e :: MAIN_MENU:
            display_main_menu();
            break;
        case game_state_e :: SHOW_RULES:
            display_rules();
            break;
        case game_state_e :: QUITTING:
            display_quitting();
            break;
        case game_state_e :: DIFICULT:
            display_dificult();
            break;      
        case game_state_e :: ENDING:
            display_endgame();
            break;   
        case game_state_e :: PLAYING:
            display_play_screen();
            break;
        case game_state_e :: SHOW_SCORE:
            display_scoreboard();
            break;        
        case game_state_e :: NO_WORDS:
            display_no_words();
            break;   
    }
}

/// Update the game based on the current game state.
void GameController :: update(){
    switch(m_game_state) {
        case game_state_e :: STARTING:
            m_game_state = game_state_e :: WELCOME;
            read_words_file();
            separate_words();
            m_players = m_curr_player->get_players();
            break;
        case game_state_e :: WELCOME:
            m_game_state = game_state_e :: MAIN_MENU;  
            m_curr_player->find_player(m_players); 
            break;  
        case game_state_e :: MAIN_MENU:
            switch(m_menu_option) {
                case menu_e :: PLAY:
                    m_secret_word.get_word(choose_word());
                    if (m_secret_word.secret_word() == L""){
                        m_game_state = game_state_e :: NO_WORDS;
                        break;
                    }
                    reset_match();
                    m_game_state = game_state_e :: PLAYING;
                    m_match = match_e :: ON;
                    m_secret_word.make_masked_word();
                    m_curr_player->add_word(m_secret_word.secret_word());
                    if (m_dificult == dificult_e :: EASY){m_secret_word.reveal_part();}
                    break;
                case menu_e :: RULES:
                    m_game_state = game_state_e :: SHOW_RULES;
                    break;
                case menu_e :: SCORE:
                    m_game_state = game_state_e :: SHOW_SCORE;
                    break;
                case menu_e :: EXIT:
                    m_game_state = game_state_e :: QUITTING;
                    break;
                case menu_e :: DIFICULT:
                    m_game_state = game_state_e :: DIFICULT;
                    break;
                default:
                    m_game_state = game_state_e :: MAIN_MENU;
                    break;       
            }
            break;
        case game_state_e :: PLAYING:
            if (m_match == match_e :: ON) {
                if (m_secret_word.wrong_guesses() == 6) {
                    m_match = match_e :: PLAYER_LOST;
                } 
                else if (m_secret_word.all_unmasked() || m_guess_all){
                    m_match = match_e :: PLAYER_WON;
                }
                else if (m_asked_to_leave){
                    m_game_state = game_state_e :: QUITTING;
                }
            } 
            else if(m_match == match_e :: PLAYER_LOST){
                m_curr_player->add_n_words();
                m_curr_player->add_loses();
                m_match_ended = true;
                if (m_dificult == dificult_e::EASY){m_curr_player->decrease_score(500);}
                else if(m_dificult == dificult_e::NORMAL){m_curr_player->decrease_score(1000);}
                else if(m_dificult == dificult_e::HARD){m_curr_player->decrease_score(2000);}
            }
            else if(m_match == match_e :: PLAYER_WON){
                m_curr_player->add_n_words();
                m_curr_player->add_wins();
                m_match_ended = true;
                if(m_dificult == dificult_e::EASY){m_curr_player->increase_score(500);}
                else if(m_dificult == dificult_e::NORMAL){m_curr_player->increase_score(1000);}
                else if(m_dificult == dificult_e::HARD){m_curr_player->increase_score(2000);}
            }
            if (m_match_ended) {
                m_game_state = game_state_e::MAIN_MENU;
                switch(m_dificult){
                    case dificult_e :: EASY:
                        m_curr_player->add_easy_played();
                        break;
                    case dificult_e :: NORMAL:
                        m_curr_player->add_medium_played();
                         break;
                    case dificult_e :: HARD:
                        m_curr_player->add_hard_played();
                        break;        
                }
            }
            break;
        case game_state_e :: SHOW_RULES:
            m_game_state = game_state_e :: MAIN_MENU;
            break;
        case game_state_e :: DIFICULT:
            m_game_state = game_state_e :: MAIN_MENU;
            break;    
        case game_state_e :: QUITTING:
            if (m_match == match_e :: ON){
                m_game_state = game_state_e :: PLAYING;
                if (m_asked_to_leave){
                    m_match = match_e :: PLAYER_LOST;
                }
            }
            else if (game_over()){
                m_game_state = game_state_e :: ENDING;
                m_curr_player->update_player(m_players);
                m_curr_player->write_all(m_players);
            }
            else {m_game_state = game_state_e :: MAIN_MENU;}
            break;
        case game_state_e :: SHOW_SCORE:
            m_game_state = game_state_e :: MAIN_MENU;
            break;   
        case game_state_e :: ENDING:
            break;  
        case game_state_e :: NO_WORDS:
            m_game_state = game_state_e :: MAIN_MENU;
            break;
    }
}

/// Process user input events, depending on the current game state.
void GameController :: process_events(){
    switch(m_game_state){
        case game_state_e :: STARTING:
            break;
        case game_state_e :: WELCOME: {  
            m_user_name = read_user_name();
            auto it = m_players.find(m_user_name);
            if (it == m_players.end()) {
                Player new_player(m_user_name); 
                m_players[m_user_name] = new_player; 
                m_curr_player = &m_players[m_user_name]; 
            } 
            else {m_curr_player = &it->second;}
            break;
        }
        case game_state_e :: MAIN_MENU: {
            m_menu_option = read_menu_option();
            break;
        }
        case game_state_e :: SHOW_RULES:
            read_enter_to_proceed();
            break;
        case game_state_e :: QUITTING:{
            if (m_match == match_e :: ON){
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
                m_asked_to_leave = read_user_confirmation();
            }
            else {m_asked_to_quit = read_user_confirmation();}
            break;
        }
        case game_state_e :: DIFICULT:{
            m_dificult = read_dificult_option();
            break;
        }
        case game_state_e :: SHOW_SCORE:
            read_enter_to_proceed();
            break;
        case game_state_e :: PLAYING:
            if (m_match == match_e :: ON){
                m_digit = false;
                m_repeated = false;
                m_guess_all = false;
                m_ch_guess = read_user_guess();
                if (m_ch_guess == L'#'){m_asked_to_leave = true;}
                else if(iswdigit(m_ch_guess)){m_digit = true;}
                else if(m_ch_guess == L'&'){
                    std :: wstring guess = read_user_word_guess();
                    if (guess.compare(m_secret_word.secret_word()) == 0){
                        m_guess_all = true;
                        if(m_secret_word.secret_word().size()/2 <= m_secret_word.n_masked_ch()){
                            if (m_dificult == dificult_e::EASY){m_curr_player->increase_score(50);}
                            else if(m_dificult == dificult_e::NORMAL){m_curr_player->increase_score(100);}
                            else if(m_dificult == dificult_e::HARD){m_curr_player->increase_score(200);}
                        }
                        else{
                            if (m_dificult == dificult_e::EASY){m_curr_player->increase_score(5);}
                            else if(m_dificult == dificult_e::NORMAL){m_curr_player->increase_score(10);}
                            else if(m_dificult == dificult_e::HARD){m_curr_player->increase_score(20);}
                        }
                    }
                    else {
                        m_secret_word.add_n_wrong_guess();
                        if (m_dificult == dificult_e::EASY){m_curr_player->decrease_score(50);}
                        else if(m_dificult == dificult_e::NORMAL){m_curr_player->decrease_score(100);}
                        else if(m_dificult == dificult_e::HARD){m_curr_player->decrease_score(200);}
                    }
                }
                else {
                    HangmanWord :: guess_e current_guess;
                    current_guess = m_secret_word.guess(m_ch_guess);
                    switch(current_guess){
                        case HangmanWord :: guess_e :: REPEATED:
                            m_repeated = true;
                            break;
                        case HangmanWord :: guess_e :: WRONG:
                            m_secret_word.add_wrong_guess(m_ch_guess);
                            if (m_dificult == dificult_e::EASY){m_curr_player->decrease_score(5);}
                            else if(m_dificult == dificult_e::NORMAL){m_curr_player->decrease_score(10);}
                            else if(m_dificult == dificult_e::HARD){m_curr_player->decrease_score(20);}
                            break;
                        case HangmanWord :: guess_e :: CORRECT:
                            m_secret_word.add_correct_guess(m_ch_guess);
                            m_secret_word.unmasked_char(m_ch_guess);
                            if (m_dificult == dificult_e::EASY){m_curr_player->increase_score(5);}
                            else if(m_dificult == dificult_e::NORMAL){m_curr_player->increase_score(10);}
                            else if(m_dificult == dificult_e::HARD){m_curr_player->increase_score(20);}
                            break;
                    }
                }
            }
            else if (m_match == match_e :: PLAYER_LOST || m_match == match_e :: PLAYER_WON){
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
                read_enter_to_proceed();}
            break;
        case game_state_e :: ENDING:
            break;
        case game_state_e :: NO_WORDS:{
            bool clear = false;
            clear = read_user_confirmation();
            if (clear){m_curr_player->clear_word_list();}
            break;
        }
        default:
            m_game_state = game_state_e :: STARTING;
            break;  
    }
}

/// Returns true when the user wants to quit the game.
bool GameController :: game_over() const{
    if (m_asked_to_quit) {return true;}
    else {return false;}
}

// === These read_xxx() methods are called in process_events()
/// Read the user name at the beginning of the game.
std :: wstring GameController :: read_user_name(){
    std :: wstring name;
    getline(std :: wcin, name);
    return name;
}

/// Reads the user confirmation, Yes/No.
bool GameController :: read_user_confirmation() const{
    std :: wstring confirmation;
    getline(std :: wcin, confirmation);
    if (confirmation ==  L"YES" || confirmation == L"Yes" || confirmation == L"yes"){return true;}
    else if (confirmation == L"NO" || confirmation == L"No" || confirmation == L"no"){return false;}
    else {
      std :: wcout << L"Error: invalid input, please enter YES/NO." << std :: endl;
      return read_user_confirmation();
    }
}

/// Reads user menu choice.
GameController :: menu_e GameController :: read_menu_option(){
    std :: wstring option;
    getline(std :: wcin, option);
    if (option == L"1"){m_menu_option = menu_e :: PLAY;}
    else if (option == L"2"){m_menu_option = menu_e :: RULES;}
    else if (option == L"3"){m_menu_option = menu_e :: SCORE;}
    else if (option == L"4"){m_menu_option = menu_e :: DIFICULT;}
    else if (option == L"5"){m_menu_option = menu_e :: EXIT;}
    else {
        std :: wcout << L"Error: Invalid option, try again." << std :: endl;
        return read_menu_option();
    }
    return m_menu_option;
}

/// Reads a simple enter from the user. (aka a pause)
void GameController :: read_enter_to_proceed() const{
    std :: wstring temp;
    getline(std :: wcin, temp);
}

/// Reads user dificult choice.
GameController :: dificult_e GameController :: read_dificult_option(){
    std :: wstring option;
    getline(std :: wcin, option);
    if (option == L"1"){m_dificult = dificult_e :: EASY;}
    else if (option == L"2"){m_dificult = dificult_e :: NORMAL;}
    else if (option == L"3"){m_dificult = dificult_e :: HARD;}
    else {
        std :: wcout << L"Error: Invalid option, try again." << std :: endl;
        return read_dificult_option();
    }
    return m_dificult;
}

/// Reads user guess letter.
wchar_t GameController :: read_user_guess(){
    wchar_t guess;
    std :: wcin >> guess;
    return std :: towupper(guess);
}

/// Reads user word guess.
std :: wstring GameController :: read_user_word_guess(){
    std :: wstring guess;
    std :: wstring temp;
    std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
    std :: getline(std :: wcin, guess);
    for (size_t i = 0; i < guess.size(); i++){
        temp.push_back(towupper(guess[i]));
    }
    return temp;
}

// === These display_xxx() methods are called in render()

/// Show the welcome mesage.
void GameController :: display_welcome() const{
    std :: wcout << L" ---> Welcome to Hangman, v 1.0 <---" << std :: endl;
    std :: wcout << L"        -copyright UFRN 2024-" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Please, enter your name:" << std :: endl;
}

/// Show the main menu.
void GameController :: display_main_menu() const{
    std :: wcout << L"=----------------[ MAIN MENU ]----------------=" << std :: endl;
    std :: wcout << L"Please choose an option:" << std :: endl;
    std :: wcout << L"1 - Start a new challenge." << std :: endl;
    std :: wcout << L"2 - Show the game rules." << std :: endl;
    std :: wcout << L"3 - Show scoreboard." << std :: endl;
    std :: wcout << L"4 - Change difucult of the game." << std :: endl;
    std :: wcout << L"5 - Quit the game." << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Enter your option number and hit 'Enter'." << std :: endl;
    std :: wcout << L"=---------------------------------------------=" << std :: endl;
}

/// Show the gallows with the hangman, whose body displayed depends on the #
/// of mistakes made (argument).
void GameController :: display_gallows() const{
    switch(m_secret_word.wrong_guesses()) {
        case 0:
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl; 
            break;
        case 1:
            std :: wcout << L"    |               O" << std :: endl;
            std :: wcout << L"    |" << std :: endl;  
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl; 
            break;
        case 2:
            std :: wcout << L"    |               O" << std :: endl;
            std :: wcout << L"    |               |" << std :: endl;
            std :: wcout << L"    |               |" << std :: endl;  
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            break;
        case 3:
            std :: wcout << L"    |               O" << std :: endl;
            std :: wcout << L"    |              /|" << std :: endl;
            std :: wcout << L"    |             / |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            break;
        case 4:
            std :: wcout << L"    |               O" << std :: endl;
            std :: wcout << L"    |              /|\\" << std :: endl;
            std :: wcout << L"    |             / | \\" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            std :: wcout << L"    |" << std :: endl;
            break;
        case 5:
            std :: wcout << L"    |               O" << std :: endl;
            std :: wcout << L"    |              /|\\" << std :: endl;
            std :: wcout << L"    |             / | \\" << std :: endl;
            std :: wcout << L"    |              /" << std :: endl;
            std :: wcout << L"    |             /" << std :: endl;
            break;
        case 6:
            std :: wcout << L"    |               O" << std :: endl;
            std :: wcout << L"    |              /|\\" << std :: endl;
            std :: wcout << L"    |             / | \\" << std :: endl;
            std :: wcout << L"    |              / \\" << std :: endl;
            std :: wcout << L"    |             /   \\" << std :: endl;
            break;              
    }
}

/// Show main play screen (w/ the hagman)
void GameController :: display_play_screen() const{
    std :: wcout << L"=---------------------[ HANGMAN ]---------------------=" << std :: endl;
    std :: wcout << L"Categories: ";
    for (size_t i = 0; i < m_current_categories.size(); i++) {
        std::wcout << m_current_categories[i];
        if (i < m_current_categories.size() - 1) {
            std::wcout << L", ";
        }
    }
    std :: wcout << std :: endl;
    std :: wcout << L"Score: " << m_curr_player->score() << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"    _________________" << std :: endl;
    std :: wcout << L"    |               |" << std :: endl;
    std :: wcout << L"    |               |" << std :: endl;
    display_gallows();
    std :: wcout << L"    |" << std :: endl;
    std :: wcout << L"    |" << std :: endl;
    std :: wcout << L"    |" << std :: endl;
    std :: wcout << L"____|____" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Correct guesses so far: <";
    for (auto i : m_secret_word.correct_guesses_list()){
        std :: wcout << i << L", ";
    }
    std :: wcout << L"> number of correct guesses so far: " << m_secret_word.correct_guesses() << std :: endl; 
    std :: wcout << L"Wrong guesses so far: <";
    for (auto i : m_secret_word.wrong_guesses_list()){
        std :: wcout << i << L", ";
    }
    std :: wcout << L"> number of wrong guesses so far: " << m_secret_word.wrong_guesses() << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << std :: endl;
    if(m_match == match_e :: ON){std :: wcout << m_secret_word.masked_str() << std :: endl;}
    else {std :: wcout << L"The secret word is " <<m_secret_word.secret_word() << std :: endl;}
    std :: wcout << std :: endl;
    if (m_match == match_e :: PLAYER_LOST){std :: wcout << L"You lose, press Enter to continue" << std :: endl;}
    else if (m_match == match_e :: PLAYER_WON){std :: wcout << L"You win, press Enter to continue" << std :: endl;}
    else if (m_match == match_e :: ON){
        if (m_digit){
            std :: wcout << L"This is a digit, try again." << std :: endl;
            std :: wcout << std :: endl;
        }
        else if (m_repeated){
            std :: wcout << L"This letter has already been used, try again." << std :: endl;
            std :: wcout << std :: endl;
        }
        std :: wcout << L"Insert '&' and press 'Enter' if you want to guess the entire word." << std :: endl;
        std :: wcout << L"Insert '#' and press 'Enter' if you want to quit." << std :: endl;
        std :: wcout << L"Insert a guess and press 'ENTER':" << std :: endl;
    }
}

/// Show screen confirming user quitting a challenge.
void GameController :: display_quitting() const{
    std :: wcout << L"=----------------[ QUITTING ]----------------=" << std :: endl;
    std :: wcout << L"Are you sure you want to quit?" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Type YES/NO and press 'enter'." << std :: endl;
    std :: wcout << L"=--------------------------------------------=" << std :: endl;
}

/// Show the game rules.
void GameController :: display_rules() const{
    std :: wcout << L"=--------------------------[ Gameplay ]----------------------------=" << std :: endl;
    std :: wcout << L"Hi " << m_user_name << L" , here are the game rules:" << std :: endl;
    std :: wcout << L"[1] You need to guess the secret word or phrase the game has chosen" << std :: endl;
    std :: wcout << L"    by suggesting letters." << std :: endl;
    std :: wcout << L"[2] We will display a row of dashes, representing each letter of the" << std :: endl;
    std :: wcout << L"    the secret word/phrase you're trying to guess." << std :: endl;
    std :: wcout << L"[3] Each correct guess earns you 1 point." << std :: endl;
    std :: wcout << L"[4] Each wrong guess you loose 1 point and I draw on component of a" << std :: endl;
    std :: wcout << L"hanged stick figure (the hangman!)" << std :: endl;
    std :: wcout << L"[5] If you wrong guess 6 times you loose the challenge" << std :: endl;
    std :: wcout << L"[6] If you can guess the secret word/phrase before the hangman is" << std :: endl;
    std :: wcout << L"    complete you add 2 extra points to your overall score." << std :: endl;
    std :: wcout << L"[7] After a guessing round (challenge) is complete you may try another" << std :: endl;
    std :: wcout << L"    secret word/phrase or quit the game." << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Press 'Enter' to continue" << std :: endl;
    std :: wcout << L"=------------------------------------------------------------------=" << std :: endl;
}

/// Show farewell message displayed at the end of the game.
void GameController :: display_endgame() const{
    std :: wcout << L"=----------------[ Farewell ]------------------=" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Thank you for play hangman!" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"=----------------------------------------------=" << std :: endl; 
}

/// Show the dificults to play the game.
void GameController :: display_dificult() const{
    std :: wcout << L"=-------------------------------------[ DIFICULT ]-------------------------------------=" << std :: endl;
    std :: wcout << L"1 - Easy: The game starts with some letters revealed and easy words are selected." << std :: endl;
    std :: wcout << L"2 - Medium: Words with more than 8 letters are chosen and no letters are revealed." << std :: endl;
    std :: wcout << L"3 - Hard: Long words, with greater diversity of letters." << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"On easy difficulty the score achieved is divided in half." << std :: endl;
    std :: wcout << L"On medium difficulty the score will be normal." << std :: endl;
    std :: wcout << L"On hard difficulty the score achieved is doubled, but you lose more points if you lose." << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"Enter your option number and hit 'Enter'." << std :: endl;
    std :: wcout << L"----------------------------------------------------------------------------------------" << std :: endl;
}

/// Show the top 5 score board.
void GameController :: display_scoreboard() const{
    std::wcout << L"=-----------------------------------[ SCOREBOARD ]-----------------------------------=" << std::endl;
    std::wcout << std::endl;
    std::wcout << L"Player               Score      Easy    Normal    Hard    Words Played     Win/Lose" << std::endl;
    std::wcout << std::endl;

    std::vector<std::pair<std::wstring, Player>> players_vector = sort_players();
    size_t count = 0;
    for (const auto& pair : players_vector) {
        if (count >= 5) { break; }
        std::wcout << std::left << std::setw(20) << pair.first
                   << std::setw(10) << pair.second.score()
                   << std::setw(8) << pair.second.easy_played()
                   << std::setw(8) << pair.second.normal_played()
                   << std::setw(8) << pair.second.hard_played()
                   << std::setw(16) << pair.second.n_words()
                   << pair.second.n_wins() << L"/" << pair.second.n_loses()
                   << std::endl;
        ++count;
    }
    std::wcout << std::endl;
    std::wcout << L"Press 'Enter' to continue" << std::endl;
    std::wcout << std::endl;
    std::wcout << L"=------------------------------------------------------------------------------------=" << std::endl;
}

/// Show interface when there are no words left.
void GameController :: display_no_words() const{
    std :: wcout << L"=--------------------------------------------------------------------------------------------=" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"There are no words to play in this dificult." << std :: endl;
    std :: wcout << L"Please change the dificult or clean the list of played words." << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"If you want to clear the words, type 'Yes', if not, type 'No'. And press 'ENTER' to continue" << std :: endl;
    std :: wcout << std :: endl;
    std :: wcout << L"=--------------------------------------------------------------------------------------------=" << std :: endl;
};

/// Sort the players by score.
std :: vector<std :: pair<std :: wstring, Player>> GameController :: sort_players() const{
    std :: vector<std :: pair<std :: wstring, Player>> players_vector(m_players.begin(), m_players.end());
    std :: sort(players_vector.begin(), players_vector.end(), [](const auto& a, const auto& b){
        return a.second.score() > b.second.score();
    });
    return players_vector;
}

/// Reset a new match.
void GameController :: reset_match(){
    m_match_ended = false;
    m_secret_word.reset();
    m_asked_to_leave = false;
    m_guess_all = false;
}

/// Reads the csv file and stores the words and categories.
void GameController :: read_words_file(){
    std :: ifstream file("words.csv");
    std :: string line, word;
    std :: wstring wword;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    if (!file.is_open()){
        std :: wcerr << L"Unable to open the file!" << std :: endl;
        std :: exit(EXIT_FAILURE);
    }
    std :: getline(file, line);
    while (std :: getline(file, line)){
        std :: stringstream ss(line);
        std :: string palavra;
        std :: string categoria;
        std :: wstring wcategoria;
        std :: vector<std :: wstring> categorias;
        std :: getline(ss, palavra, ',');
        std::transform(palavra.begin(), palavra.end(), palavra.begin(), [](unsigned char c) { return std::toupper(c); });
        wword = converter.from_bytes(palavra);
        while (std :: getline(ss, categoria, ',')){
            std::transform(categoria.begin(), categoria.end(), categoria.begin(), [](unsigned char c) { return std::toupper(c); });
            wcategoria = converter.from_bytes(categoria);
            categorias.push_back(wcategoria);
        }
        m_all_words.push_back({wword, categorias});
    }
    file.close();
};

/// Separate the words by dificult.
void GameController :: separate_words(){
    for (size_t i = 0; i < m_all_words.size(); i++){
        if (m_all_words[i].word.size() > 6){m_hard_words.push_back(m_all_words[i]);}
        else if (m_all_words[i].word.size() > 4 && m_all_words[i].word.size() <= 6){m_normal_words.push_back(m_all_words[i]);}
        m_easy_words.push_back(m_all_words[i]);
    }
}

/// Choose a random word from a list that has not been played before.
std::wstring GameController :: choose_word(){
    std::wstring word;
    const std::set<std::wstring>& played_words = m_curr_player->get_played_words();
    std::vector<Word>* word_list = nullptr;
    switch (m_dificult) {
        case dificult_e::EASY:
            word_list = &m_easy_words;
            break;
        case dificult_e::NORMAL:
            word_list = &m_normal_words;
            break;
        case dificult_e::HARD:
            word_list = &m_hard_words;
            break;
        default:
            std::wcerr << L"Invalid difficulty level" << std::endl;
            return L"";
    }
    if (word_list->empty()) {
        std::wcerr << L"No words available for the selected difficulty" << std::endl;
        return L"";
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, word_list->size() - 1);
    bool found_word = false;
    int attempts = 0;
    const int max_attempts = word_list->size();
    while (!found_word && attempts < max_attempts) {
        int index = distr(gen);
        const std::wstring& candidate_word = (*word_list)[index].word;
        if (played_words.find(candidate_word) == played_words.end()) {
            word = candidate_word;
            m_current_categories = (*word_list)[index].categories;
            found_word = true;
        }
        attempts++;
    }
    if (!found_word) {return L"";}
    return word;
}
