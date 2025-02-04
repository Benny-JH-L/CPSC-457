
/*
CPSC457 Winter 2025 | Assignment 1 | Benny Liang | 30192142
*/

#include <ctype.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include <tuple>

#include <iostream> // TEMP FOR DEBUG

using namespace std;

// split string p_line into a vector of strings (words), and stores it in the vector entered in.
// the delimiters are 1 or more whitespaces, or whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'
// vector<std::string> split(const unsigned char* p_line, int buff_size)
void split(const unsigned char* p_line, int buff_size, vector<string>& words)
{
    string tmpWord = "";
    for (size_t i = 0; i < buff_size; i++)  // loop through `p_line` until we reach last element inside it (will reach '\0`) 
    {
        if (!isspace(p_line[i]))    // as long as the char is not a `space`/word separator, add it to tmp word
            tmpWord += p_line[i];
        else if (!tmpWord.empty())  // if the current character being checked is a `space`/word separator, and tmp word is not empty, add the word
        {
            words.push_back(tmpWord);   // add the word
            tmpWord = "";   // reset tmp word
        }
    }

    if (!tmpWord.empty())   // if the file doesn't end in a space or new line character, add the tmp word as long as it's not empty 
        words.push_back(tmpWord);
}

// global variable used in stdin_readline()
unsigned char buffer[1024*1024];    // 1MB of storage to hold result of `read()`
int buff_size = 0;  // stores number of characters are in `buffer` 

tuple<vector<string>, int> stdin_readline()
{
    vector<string> words;

    // read the up to the size of `buffer` (1MB)
    buff_size = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (buff_size <= 0)
        return tuple(words, buff_size);

    split(buffer, buff_size, words);
    return tuple(words, buff_size);
}

// returns true if a word is palindrome
// palindrome is a string that reads the same forward and backward
//    after converting all characters to lower case
bool is_palindrome(const std::string & s)
{
    for (size_t i = 0; i < s.size() / 2; i++) {
        if (tolower(s[i]) != tolower(s[s.size() - i - 1])) {
            return false;
        }
    }
    return true;
}

// get_longest_palindrome() returns the longest palindrome on standard input.
// In case of ties for length, returns the first palindrome found.
//
// Algorithm:
// Input is broken into lines, each line into words, and each word
// is checked to see if it is palindrome, and if it is, whether it
// is longer than the largest palindrome encountered so far.
//
// A word is a sequence of characters separated by white space
// white space is whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'
std::string get_longest_palindrome()
{
    string max_pali = "";
    vector<string> wordsVector;
    while (true)
    {        
        auto [wordsVector, buff_size] = stdin_readline(); // get new words

        if (buff_size <= 0) // EOF, break
            break;

        // Find the longest palindrome from what was read.
        for (string word : wordsVector)
        {
            if (word.length() <= max_pali.length())
                continue;
            else if (is_palindrome(word))
                max_pali = word;
        }
    }

    return max_pali;
}

int main()
{
    std::string max_pali = get_longest_palindrome();
    printf("Longest palindrome: %s\n", max_pali.c_str());
    return 0;
}
