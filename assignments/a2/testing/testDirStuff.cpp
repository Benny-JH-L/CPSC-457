
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <fcntl.h>

#include <iostream> // ONLY USED FOR DEBUGGING STATEMENTS (std::cout)

using namespace std;

#pragma once

#include <string>
#include <utility>
#include <vector>

struct ImageInfo {
    std::string path;
    long width, height;
};

struct Results {
    // path of the largest file in the directory
    std::string largest_file_path;
    // size (in bytes) of the largest file
    long largest_file_size;
    // total number of files in the directory (recursive)
    long n_files;
    // total number of directories in the directory (recursive)
    long n_dirs;
    // cumulative size (in bytes) of all files
    long all_files_size;
    // most common words found in .txt files
    // word = sequence of 5 or more alphabetic characters, converted to lower case
    // sorted by frequency, reported with their counts
    std::vector<std::pair<std::string, int>> most_common_words;
    // largest (in pixels) images found in the directory,
    // sorted by their size (in pixels), reported with their width and height
    std::vector<ImageInfo> largest_images;
    // list of vacant directories
    // vacant directory is one that contains no files anywhere, including in subdirectories
    // (resursive) if a directory is reported vacant, none of its subdirectories should be reported
    // here
    std::vector<std::string> vacant_dirs;
};


/// check if path refers to a directory
static bool is_dir(const std::string & path)
{
    struct stat buff;
    if (0 != stat(path.c_str(), &buff)) { return false; }
    return S_ISDIR(buff.st_mode);
}

/// check if path refers to a file
static bool is_file(const std::string & path)
{
    struct stat buff;
    if (0 != stat(path.c_str(), &buff)) { return false; }
    return S_ISREG(buff.st_mode);
}

/// check if string ends with another string
static bool ends_with(const std::string & str, const std::string & suffix)
{
    if (str.size() < suffix.size()) {
        return false;
    } else {
        return 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
    }
}

/// @brief maps the words occuring in a ".txt" File of `filePath` to `mapOccurances`.
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
        int bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break;
        
        printf("`Line read: %s`\n", buffer);
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

    close(fd);   // close the file
    return 1;
}

void analyzeRecur(Results& result, string dir_name)
{
    // string dir_name = "."; // using chdir("..") will move up a directory
    DIR* dir = opendir(dir_name.c_str());
    
    struct dirent *dstruct;
    
    if(dir == NULL)
    {
        perror("open directory");
    }

    while(true)
    {
        dstruct = readdir(dir);
        
        if(dstruct == NULL)
            break;
        
        string nameOfEntity = dstruct->d_name;

        if (nameOfEntity == ".")    // count current directory
        {
            // cout << "cur dir: " << nameOfEntity << endl;
            result.n_dirs++;        // increment number of directories by 1.
            continue;
        }
        else if (nameOfEntity == "..") // Skip parent directory
        {
            // cout << "prev dir: " << nameOfEntity << endl;
            continue; 
        }

        string pathOfEntity = dir_name + "/" + nameOfEntity; // Full path of the entity

        if (is_dir(pathOfEntity))       // check if the `pathOfEntity` is a directory
        {
            cout<<"Name of the dir: "<< nameOfEntity <<endl;
            analyzeRecur(result, pathOfEntity);  // Recursively call `analyzeRecur(...)` on a sub-directory 
        }
        else if (is_file(pathOfEntity)) // check if the `pathOfEntity` is a file
        {
            cout<<"Name of the file: "<< nameOfEntity <<endl;
            result.n_files++;   // increment number of files
            
            // check if its a .txt
            // if it is then need to read it and count the words of at least length 5 (use `mapWordsFromFile()`)
            // and compare if it is the largest file, then increment the cumulative file sizes
        }
        else
            cout<<"Name of the entity "<< nameOfEntity <<endl;
    }

    closedir(dir);
}

void analyze(Results& result)
{
    string dir_name = "."; // using chdir("..") will move up a directory
    analyzeRecur(result, dir_name);
}

int main()
{
    string changeDirTo = "test1";
    /*
    Expected results for directory `test1`:
        Largest file:      "dir1/down.png"
        Largest file size: 202
        Number of files:   7
        Number of dirs:    10
        Total file size:   253
        Most common words from .txt files:
            - "contents" x 2
        Vacant directories:
        Largest images:
            - "dir1/down.png" 16x16
    */

    // string changeDirTo = "test2";
    /*
    Expected results for directory `test2`:
        Largest file:      "a.txt"
        Largest file size: 0
        Number of files:   2
        Number of dirs:    2
        Total file size:   0
        Most common words from .txt files:
        Vacant directories:
        Largest images:
    */

    // Simulate `main` changing directory
    chdir(changeDirTo.c_str());
    cout << "Changed directory to: " << changeDirTo << endl;

    // Initialize struct values
    vector<pair<string, int>> most_common_pairs;
    vector<ImageInfo> largest_images;
    vector<string> vacent_dirs;
    Results result = {"", -1, 0, 0, 0, most_common_pairs, largest_images, vacent_dirs};
    analyze(result);

    cout << "\n\n" << changeDirTo << " as `root` directory: " << endl;
    cout << "The largest file has the path: " << result.largest_file_path << endl; 
    cout << "The largest file size (Bytes) is: " << result.largest_file_size << endl; 
    cout << "Total number of files: " << result.n_files << endl; 
    cout << "Total number of directories: " << result.n_dirs << endl; 
    cout << "Cumulative file sizes (Bytes): " << result.all_files_size << endl; 

    return 0;
}
