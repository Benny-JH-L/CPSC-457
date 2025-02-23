
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include<String>
#include<vector>

#include <fcntl.h>  // For open()
#include <cstring>  // For strerror()

#include<iostream> // for cout printing

using namespace std;

/// @brief maps the words occuring in the File `filePath` to `mapOccurances`.
/// @param mapOccurances a unordered_map<string, size_t>&, containing words and their number of occurances.
/// @param filePath a valid file path, a `std::string`.
/// @return an int. Return,
/// -1: error opening a file.
/// 0: something :).
/// 1: successfully opened and read file.
static int mapWordsFromFileINCLUDING_DIGITS(unordered_map<string, size_t>& mapOccurances, string filePath)
{
    int fd = open(filePath.c_str(), O_RDONLY);  // open the file

    if (fd <= 0)  // Error opening 
        return fd;
    
    unsigned char buffer[1024*1024]; // 1MB buffer
    // unsigned char buffer[10]; // TEST ONLY
    
    // Read a line in the file and store it in `buffer`
    string tmpWord = "";
    while(true)
    {
        int bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break;
        
        printf("Line read: `%s`\n", buffer);
        // splitAndCheck(buffer, bytesRead, mapOccurances);
        
        // Parse what was read into words
        for (size_t i = 0; i < bytesRead; i++)  // loop through `buffer` until we reach last element inside it (will reach '\0`) 
        {
            if (!isspace(buffer[i]))    // as long as the char is not a `space`/word separator, add it to tmp word
                tmpWord += buffer[i];
            else if (!tmpWord.empty())  // if the current character being checked is a `space`/word separator, and tmp word is not empty, add the word
            {
                if (tmpWord.length() < 5)   // check if the word is at least 5 characters long
                {
                    tmpWord = "";
                    continue;
                }
                printf("Word created: %s\n", tmpWord.c_str());
                mapOccurances[tmpWord]++;   // increment the number of occurances for this word.
                tmpWord = "";   // reset tmp word
            }
        }
    }
    // Last check for `tmpWord`: if the file doesn't end with a space or new line.
    if (!tmpWord.empty() && tmpWord.length() >= 5)
        mapOccurances[tmpWord]++;   // increment the number of occurances for this word.

    close(fd);   // close the file
    return 1;
}

/// @brief maps the words occuring in a ".txt" File of `filePath` to `mapOccurances`. (Only includes letters for words)
/// @param mapOccurances a unordered_map<string, size_t>&, containing words and their number of occurances.
/// @param filePath a valid file path, a `std::string`.
/// @return an int. Return,
/// -1: error opening a file.
/// 0: something :).
/// 1: successfully opened and read file.
static int mapWordsFromFile(unordered_map<string, size_t>& mapOccurances, string filePath)
{
    int fd = open(filePath.c_str(), O_RDONLY);  // open the file

    if (fd <= 0)  // Error opening 
        return fd;
    
    unsigned char buffer[1024*1024]; // 1MB buffer
    
    // Read a line in the file and store it in `buffer`
    string tmpWord = "";
    while(true)
    {
        // int bytesRead = read(fd, buffer, sizeof(buffer));
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break;
        
        printf("`Line read: %s`\n", buffer);
        // splitAndCheck(buffer, bytesRead, mapOccurances);
        
        // Parse what was read into words
        // for (size_t i = 0; i < bytesRead; i++)  // loop through `buffer` until we reach last element inside it (will reach '\0`) 
        for (ssize_t i = 0; i < bytesRead; i++)  // loop through `buffer` until we reach last element inside it (will reach '\0`) 
        {
            if (isalpha(buffer[i]))    // if the char is a letter, add it to tmp word
                tmpWord += tolower(buffer[i]);  // set the char at `buffer[i]` to lowercase before adding it to tmp word.
            else if (!tmpWord.empty())  // if the current character being checked is a `space`/word separator, and tmp word is not empty, add the word
            {
                if (tmpWord.length() < 5)   // check if the word is at least 5 characters long
                {
                    tmpWord = "";
                    continue;
                }
                printf("Word created: %s\n", tmpWord.c_str());
                mapOccurances[tmpWord]++;   // increment the number of occurances for this word.
                tmpWord = "";   // reset tmp word
            }
        }
    }
    // Last check for `tmpWord`: if the file doesn't end with a space or new line.
    if (!tmpWord.empty() && tmpWord.length() >= 5)
        mapOccurances[tmpWord]++;   // increment the number of occurances for this word.
    
    close(fd);   // close the file
    return 1;
}

int main()
{
    string path = "t1.txt";
    unordered_map<string, size_t> mapOccurances;
    mapWordsFromFileINCLUDING_DIGITS(mapOccurances, path);

    // debug
    cout << "\nPrinting occrances in map: " << endl;
    for (auto pair : mapOccurances)
    {
        auto [word, occurances] = pair;
        cout << word << ": " << occurances << endl;
    }

    return 0;
}



//OLD


// split string p_line into a vector of strings (words), and stores it in the vector entered in.
// the delimiters are 1 or more whitespaces, or whatever isspace() says it is
//    i.e. ' ', '\n', '\r', '\t', '\n', '\f'
// vector<std::string> split(const unsigned char* p_line, int buff_size)
// void splitAndCheck(const char* p_line, int buff_size, unordered_map<string, size_t>& mapOccurances)
// {
//     string tmpWord = "";
//     for (size_t i = 0; i < buff_size; i++)  // loop through `p_line` until we reach last element inside it (will reach '\0`) 
//     {
//         if (!isspace(p_line[i]))    // as long as the char is not a `space`/word separator, add it to tmp word
//             tmpWord += p_line[i];
//         else if (!tmpWord.empty())  // if the current character being checked is a `space`/word separator, and tmp word is not empty, add the word
//         {
//             printf("Word created: %s\n", tmpWord.c_str());
//             mapOccurances[tmpWord]++;
//             tmpWord = "";   // reset tmp word
//         }
//     }
// }

// void splitAndCheck(const unsigned char* p_line, int buff_size, unordered_map<string, size_t>& mapOccurances)
// {
//     string tmpWord = "";
//     for (size_t i = 0; i < buff_size; i++)  // loop through `p_line` until we reach last element inside it (will reach '\0`) 
//     {
//         if (!isspace(p_line[i]))    // as long as the char is not a `space`/word separator, add it to tmp word
//             tmpWord += p_line[i];
//         else if (!tmpWord.empty())  // if the current character being checked is a `space`/word separator, and tmp word is not empty, add the word
//         {
//             printf("Word created: %s\n", tmpWord.c_str());
//             mapOccurances[tmpWord]++;
//             tmpWord = "";   // reset tmp word
//         }
//     }
// }

/// @brief maps the words occuring in the File `filePath` to `mapOccurances`.
/// @param mapOccurances a unordered_map<string, size_t>&, containing words and their number of occurances.
/// @param filePath a valid file path, a `std::string`.
/// @return an int. Return,
/// 1: error opening a file.
/// 0: successfully opened and read file.
// static int getWordsFromFile(unordered_map<string, size_t>& mapOccurances, string filePath)
// {
//     FILE* file = fopen(filePath.c_str(), "r");  // open the file

//     if (!file)  // Error opening 
//         return 1;
    
//     char buffer[1024*1024]; // 1MB buffer
    
//     // Read a line in the file and store it in `buffer`
//     while(fgets(buffer, sizeof(buffer), file))
//     { will probably slower since i keep 
//         printf("Line read: %s", buffer);
//         splitAndCheck(buffer, strlen(buffer), mapOccurances);
//     }

//     fclose(file);   // close the file
//     return 0;
// }