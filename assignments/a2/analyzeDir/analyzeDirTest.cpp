
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <fcntl.h>
#include <algorithm>
#include <cctype>   // for tolower()
#include <iostream> // ONLY USED FOR DEBUGGING STATEMENTS (std::cout)

using namespace std;

#pragma once

#include <string>
#include <utility>
#include <vector>

struct ImageInfo 
{
    std::string path;
    long width, height;
};

struct Results 
{
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



/// @brief A struct that contains a pointer to a `Results` struct, and contains 
// an unordered_map<string, size_t> that keeps track of the number of occurances of a word from ".txt" files.
struct AnalyzeStruct{
    Results* result;                                // A ptr to `Results`
    unordered_map<string, size_t> mapOccurances;    // keeps track of the number of occurances of a word from ".txt" files.
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
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead <= 0)
            break;
        
        // printf("`Line read: %s`\n", buffer);
        
        // Parse what was read into words
        for (ssize_t i = 0; i < bytesRead; i++)  // loop through `buffer` until we reach last element inside it (will reach '\0`) 
        {
            if (isalpha(buffer[i]))       // if the char is a letter, add it to tmp word
                tmpWord += tolower(buffer[i]);  // set the char at `buffer[i]` to lowercase before adding it to tmp word.
            else if (!tmpWord.empty())  // if the current character being checked is a `space`/word separator, and tmp word is not empty, add the word
            {
                if (tmpWord.length() < 5)   // check if the word is at least 5 characters long
                {
                    tmpWord = "";
                    continue;
                }
                // printf("Word created: %s\n", tmpWord.c_str());  // debug
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

// bool analyzeRecur(Results& result, unordered_map<string, size_t>& mapOccurances, string dir_name)

/// @brief Recursive analyze.
/// @param analyzeStruct a AnalyzeStruct struct.
/// @param dir_name the path of a directory, string.
/// @param topLevelVacentDirs a vector<string>&, stores the top-level empty directories.
/// @return a boolean, of if a file was encountered in a directeory. Returns true if a file was found, false otherwise.
bool analyzeRecur(AnalyzeStruct& analyzeStruct, string dir_name, vector<string>& topLevelVacentDirs)
{
    // string dir_name = "."; // using chdir("..") will move up a directory
    DIR* dir = opendir(dir_name.c_str());
    
    struct dirent *dstruct;
    
    if(dir == NULL)
    {
        perror("open directory");
    }

    // Read all the entities in this directory
    Results& result = *analyzeStruct.result;    // dereference the pointer
    bool containsFile = false;        // Keeps track of if a child directory contains a file.
    vector<string> emptyChildDirs;    // Keeps track of empty child directories (paths) for the parent directory.
    while(true)
    {
        dstruct = readdir(dir);
        
        if(dstruct == NULL)
            break;
        
        string nameOfEntity = dstruct->d_name;

        if (nameOfEntity == ".")    // count current directory
        {
            // cout << "cur dir: " << nameOfEntity << endl; // debug
            result.n_dirs++;        // increment number of directories by 1.
            continue;
        }
        else if (nameOfEntity == "..") // Skip parent directory
        {
            // cout << "prev dir: " << nameOfEntity << endl; // debug
            continue; 
        }

        string pathOfEntity = dir_name + "/" + nameOfEntity; // Full path of the entity

        if (is_dir(pathOfEntity))       // check if the `pathOfEntity` is a directory
        {
            // cout<<"Name of the dir: "<< nameOfEntity <<endl;    // debug
            bool subHasFile = analyzeRecur(analyzeStruct, pathOfEntity, emptyChildDirs);  // Recursively call `analyzeRecur(...)` on a sub-directory
            // if the sub-directory contains a file, `containsFile` is set true, otherwise false.
            containsFile = containsFile || subHasFile;
        }
        else if (is_file(pathOfEntity)) // check if the `pathOfEntity` is a file
        {
            containsFile = true;
            // cout<<"Name of the file: "<< nameOfEntity <<endl; // debug
            result.n_files++;   // increment number of files

            struct stat fileStat;
            stat(pathOfEntity.c_str(), &fileStat);  // get the file's information

            long int& fileSize = fileStat.st_size;
            result.all_files_size += fileSize;      // add the size of the file to cumulative file size

            // cout << "Size of txt file (Bytes): " << fileSize << endl; // debug

            // Check if this file is the largest
            if (fileSize > result.largest_file_size)
            {
                // Set the new file size and path
                result.largest_file_path = pathOfEntity;
                result.largest_file_size = fileSize;
            }

            // check if the file is a ".txt" file
            if (ends_with(nameOfEntity, ".txt"))    
            {
                mapWordsFromFile(analyzeStruct.mapOccurances, pathOfEntity);  // count the words in the file. 
            }
            // Check if the file is an image (NOTE TO SELF, NEED TO TEST USING LAB COMPUTERS FOR THIS PART)
            else
            {
                // cout << "Checking potential picture path: " << pathOfEntity << endl; // debug
                string cmd = "identify -format '%wx%h' " + pathOfEntity + " 2> /dev/null";    // command for `popen()` (NOTE TO SELF, NEED TO TEST USING LAB COMPUTERS)
                // NOTE TO SELF: there's probably a Windows equivalent for ^
                FILE* fp = popen(cmd.c_str(), "r");
                
                if (fp)
                {
                    // cout << "Entered valid image checker" << endl; // debug
                    string imgSizeStr;
                    char buff[PATH_MAX];

                    if (fgets(buff, PATH_MAX, fp) != NULL)  // read line generated from the command using `cmd` in `popen(..)`
                        imgSizeStr = buff;                // store the dimensions of the image (width x height)
                    
                    // cout << "imgSizeStr = " << imgSizeStr << endl; // debug

                    int status = pclose(fp);
                    if (status != 0 || imgSizeStr[0] == '0')  // check if the exit status is 0, or if the image has no size.
                        imgSizeStr = "";                      // set the string empty
                    
                    // if the `image_size_str` is not empty, then parse the string into the width and length
                    if (!imgSizeStr.empty())
                    {
                        // cout << "     Valid image found" << endl; // debug

                        size_t posOfX = imgSizeStr.find('x');   // finds the position of `x` (will be between width and height)
                        // cout << "     pos of \'x\': " << posOfX << endl; // debug
                        long width = stoi(imgSizeStr.substr(0, posOfX));  // get the width (get the substring from index 0 to before `x`s position)
                        long height = stoi(imgSizeStr.substr(posOfX + 1, imgSizeStr.length() - posOfX - 1));   // get the height (get the substring from index [`x`+ 1] to the remaining characters in the string)
                        // Ex. 15x15: index `x` is 2, string len: 5, number of characters after `x` is 5 - 2 - 1 = 2
                        // cout << "     img width: " << width << endl; // debug
                        // cout << "     img height: " << height << endl; // debug
                        result.largest_images.emplace_back(ImageInfo{pathOfEntity, width, height});    // add this image's info to the std::vector

                        // ImageInfo imgInfo = {pathOfEntity, width, height};
                        // result.largest_images.emplace_back(imgInfo);    // add this image's info to the std::vector
                    }
                }
            }

        }
        // else
        //     cout<<"Name of the entity "<< nameOfEntity <<endl; // debug
    }

    // If none of its children directories have files, then add only itself to the vector containing top-level vacant directories. 
    if (!containsFile)  
        topLevelVacentDirs.emplace_back(dir_name);  // Add the parent dir's path
    // Otherwise, there is a file somwhere in its children dirs.
    else
    {
        // Add only the children directories who are top-level vacant with respect to that child (ie the child directory does not contain any files).
        for (string s : emptyChildDirs)
            topLevelVacentDirs.emplace_back(s);     // Add the child dir's path
    }

    closedir(dir);
    return containsFile;
}

void analyze(Results& result, int n)
{
    string dir_name = "."; // Note to self; using chdir("..") will move up a directory
    AnalyzeStruct analyzeStruct;
    analyzeStruct.result = &result; // Set the pointer in the `analyzeStruct`
    // analyzeRecur(result, mapOccurances, dir_name);
    // analyzeRecur(result, analyzeStruct, dir_name);
    analyzeRecur(analyzeStruct, dir_name, result.vacant_dirs);
    unordered_map<string, size_t>& mapOccurances = analyzeStruct.mapOccurances;


    // remove the "./" in the path to the largest file to get the relative path to it.
    result.largest_file_path.erase(0, 2);   // remove 2 characters staring from index 0 (1st character) of the string.

    // debug
    // cout << "\nWords occurances mapped form .txt files" << endl;
    // for (auto& pair : mapOccurances)
    // {
    //     auto& [str, occurances] = pair;
    //     cout << "\"" << str << "\" x " << occurances << endl; 
    // }
    // end debug

    // Get the `n` most common words in `mapOccurances`
    // vector<pair<string, int>> arr;  // Pair: 1st elem, word. 2nd elem, number of occurances for that word.
    // first we place the words and counts into a vector (with count negative to reverse the sort)
    vector<pair<int, string>> arrOccWords; // Pair: 1st number of occurances for that word., word. 2nd elem, word. 
    for(auto& pair : mapOccurances)
    {
        auto [word, occ] = pair;
        arrOccWords.emplace_back(-occ, word);
    }

    // if we have more than N entries, we'll sort partially, since
    // we only need the first N to be sorted
    if(arrOccWords.size() > size_t(n)) 
    {
        partial_sort(arrOccWords.begin(), arrOccWords.begin() + n, arrOccWords.end());
        arrOccWords.resize(n); // drop all entries after the first `n`
    } 
    else 
    {
        sort(arrOccWords.begin(), arrOccWords.end());
    }
    
    // Set the most common words into `result`
    for (auto& pair : arrOccWords)
    {
        auto [occ, word] = pair;
        result.most_common_words.emplace_back(word, -occ);
    }

    // debug
    // cout << "\nSorted `n` common words: " << endl;
    // for (auto pair : result.most_common_words)
    // {
    //     auto& [str, occurances] = pair;
    //     cout << "\"" << str << "\" x " << occurances << endl; 
    // }
    // end debug

    // Get the `n` largest images
    // if we have more than N entries, we'll sort partially, since
    // we only need the first N to be sorted
    
    // Used for `partial_sort(...)` as 4th arg. Tells it to sort `ImageInfo` by the number of pixels an image has. 
    auto compareByPixels = [](const ImageInfo& a, const ImageInfo& b){
        return a.width * a.height > b.width * b.height;         // Sort by greatest to smallest (Descending order).
    };

    vector<ImageInfo>& arrImg = result.largest_images;
    if (arrImg.size() > size_t(n))
    {
        partial_sort(arrImg.begin(), arrImg.begin() + n, arrImg.end(), compareByPixels);
        arrImg.resize(n);  // drop all entries after the first n
    }
    else
    {
        sort(arrImg.begin(), arrImg.end(), compareByPixels);
    }

    // remove the "./" in the path to the largest images to get the relative path to it.
    for (ImageInfo& info : arrImg)
        info.path.erase(0, 2);   // remove 2 characters staring from index 0 (1st character) of the string.
    
    // remove the "./" in the paths of vacant top level direcetories, to get the relative path to it.
    for (string& s: result.vacant_dirs)
        s.erase(0, 2);  // remove 2 characters staring from index 0 (1st character) of the string.
}

// ======================================================================
// You need to re-implement this function !!!!
// ======================================================================
//
// analyzeDir(n) computes stats about cuSrrent directory
//   n = how many words and images to report in restuls
//
// The code below is incomplate/incorrect. Feel free to re-use any of it
// in your implementation, or delete all of it.
Results analyzeDir(int n)
{
    // main changes current directory to the one entered as argv[2].

    // Initialize struct values
    vector<pair<string, int>> most_common_pairs;
    vector<ImageInfo> largest_images;
    vector<string> vacent_dirs;
    Results result = {"", -1, 0, 0, 0, most_common_pairs, largest_images, vacent_dirs};
    analyze(result, n);

    // debug
    // DIR* dir = opendir(".");
    // struct dirent *dstruct;
    // dstruct = readdir(dir);
    // cout << "\n\n" << dstruct->d_name << " as `root` directory: " << endl;
    // cout << "The largest file has the path: " << result.largest_file_path << endl; 
    // cout << "The largest file size (Bytes) is: " << result.largest_file_size << endl; 
    // cout << "Total number of files: " << result.n_files << endl; 
    // cout << "Total number of directories: " << result.n_dirs << endl; 
    // cout << "Cumulative file sizes (Bytes): " << result.all_files_size << endl; 

    // cout << "Most common words from .txt files: " << endl;
    // for (auto pair : result.most_common_words)
    // {
    //     auto [str, occurances] = pair;
    //     cout << "\"" << str << "\" x " << occurances << endl; 
    // }
    // cout << "Vacant directories: " << endl;
    // for (string s : result.vacant_dirs)
    //     cout << s << endl;
    
    // cout << "Largest images: " << endl;
    // for (ImageInfo info : result.largest_images)
    //     cout << "\"" << info.path << "\" " << info.width << "x" << info.height << endl;
    // end debug

    return result;
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

    // NOTE: NEED TO COMMENT OUT IMAGE CHECKING IF I'M RUNNING IT ON WINDOWS, SINCE 
    // `identify` is not a cmd for Windows!

    // Simulate `main` changing directory
    chdir(changeDirTo.c_str());
    cout << "Changed directory to: " << changeDirTo << endl;

    // Initialize struct values
    vector<pair<string, int>> most_common_pairs;
    vector<ImageInfo> largest_images;
    vector<string> vacent_dirs;
    Results result = {"", -1, 0, 0, 0, most_common_pairs, largest_images, vacent_dirs};
    analyze(result, 5);

    cout << "\n\n" << changeDirTo << " as `root` directory: " << endl;
    cout << "The largest file has the path: " << result.largest_file_path << endl; 
    cout << "The largest file size (Bytes) is: " << result.largest_file_size << endl; 
    cout << "Total number of files: " << result.n_files << endl; 
    cout << "Total number of directories: " << result.n_dirs << endl; 
    cout << "Cumulative file sizes (Bytes): " << result.all_files_size << endl; 

    cout << "Most common words from .txt files: " << endl;
    for (auto pair : result.most_common_words)
    {
        auto [str, occurances] = pair;
        cout << "\"" << str << "\" x " << occurances << endl; 
    }
    cout << "Vacant directories: " << endl;
    for (string s : result.vacant_dirs)
        cout << s << endl;
    
    cout << "Largest images: " << endl;
    for (ImageInfo info : result.largest_images)
        cout << "\"" << info.path << "\" " << info.width << "x" << info.height << endl;
    return 0;
}
