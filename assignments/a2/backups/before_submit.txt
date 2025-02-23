/// =========================================================================
/// Written by pfederl@ucalgary.ca in 2025, for CPSC457.
/// =========================================================================
/// You need to edit this file.
///
/// You can delete all contents of this file and start from scratch if
/// you wish, as long as you implement the analyzeDir() function as
/// defined in "analyzeDir.h".

#include "analyzeDir.h"

#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <fcntl.h>

#include <algorithm>
#include <iostream> // ONLY USED FOR DEBUGGING STATEMENTS (std::cout)

using namespace std;

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
    // FIRST ATTEMPT
    // // Get the `n` largest images
    // vector<pair<int, ImageInfo>> arrImg;  // Pair: 1st elem, image size (width x height)
    // for(auto& imageInfo : result.largest_images)
    //     arrImg.emplace_back(imageInfo.width * imageInfo.height, imageInfo);
    // // if we have more than N entries, we'll sort partially, since
    // // we only need the first N to be sorted

    // auto compareBySize = [](const std::pair<int, ImageInfo>& a, const std::pair<int, ImageInfo>& b) {
    //     return a.first > b.first;  // Descending order (largest first)
    // };

    // if (arrImg.size() > size_t(n))
    // {
    //     partial_sort(arrImg.begin(), arrImg.begin() + n, arrImg.end(), compareBySize);
    //     arrImg.resize(n);  // drop all entries after the first n
    // }
    // else
    // {
    //     sort(arrImg.begin(), arrImg.end());
    // }
    // Set the largest images to `result`
    // result.largest_images.clear();  // clear the vector before adding in the largest images.
    // for (auto& pair : arrImg)
    // {
    //     auto& [_, imgInfo] = pair;
    //     // remove the "./" in the path to the largest file to get the relative path to it.
    //     imgInfo.path.erase(0, 2);   // remove 2 characters staring from index 0 (1st character) of the string.
    //     result.largest_images.push_back(imgInfo);
    //     cout << "added largest image: " << result.largest_images.back().path << endl;
    // }



    // // The code below does a subset of the functionality you need to implement.
    // // It also contains several debugging printf() statements. When you submit
    // // your code for grading, please remove any dubugging pritf() statements.
    // std::string dir_name = ".";
    // auto dir = opendir(dir_name.c_str());
    // if (dir == NULL) error(-1, errno, "could not opendir '%s'", dir_name.c_str());
    // for (auto de = readdir(dir); de != nullptr; de = readdir(dir)) {
    //     std::string name = de->d_name;
    //     // skip . and .. entries
    //     if (name == "." or name == "..") continue;

    //     printf(" - \"%s\"\n", name.c_str());

    //     // check if this is directory
    //     if (is_dir(name)) {
    //         printf("    - is a directory\n");
    //         continue;
    //     }
    //     // make sure this is a file
    //     if (not is_file(name)) {
    //         printf("    - not a file and not a directory!!!\n");
    //         continue;
    //     }
    //     // report size of the file
    //     {
    //         struct stat buff;
    //         if (0 != stat(name.c_str(), &buff)) {
    //             printf("    - could not determine size of file\n");
    //         } else {
    //             printf("    - file size: %ld bytes\n", long(buff.st_size));
    //         }
    //     }
    //     // check if filename ends with .txt
    //     if (ends_with(name, ".txt")) {
    //         printf("    - is a .txt file\n");
    //     } else {
    //         printf("    - is not a .txt file\n");
    //     }

    //     // let's see if this is an image and whether we can manage to get image
    //     // info by calling an external utility 'identify'
    //     std::string cmd = "identify -format '%wx%h' " + name + " 2> /dev/null";
    //     std::string img_size;
    //     auto fp = popen(cmd.c_str(), "r");
    //     if (! fp) error(-1, errno, "could not call identify via popen");
    //     char buff[PATH_MAX];
    //     if (fgets(buff, PATH_MAX, fp) != NULL) { img_size = buff; }
    //     int status = pclose(fp);
    //     if (status != 0 or img_size[0] == '0') { img_size = ""; }
    //     if (img_size.empty()) {
    //         printf("    - not an image\n");
    //     } else {
    //         printf("    - image %s\n", img_size.c_str());
    //     }
    // }
    // closedir(dir);

    // // The results below are all hard-coded, to show you all the fields
    // // you need to calculate. You should delete all code below and
    // // replace it with your own code.
    // Results res;
    // res.largest_file_path = "some_dir/some_file.txt";
    // res.largest_file_size = 123;
    // res.n_files = 321;
    // res.n_dirs = 333;
    // res.all_files_size = 1000000;

    // res.most_common_words.push_back({ "hello", 3 });
    // res.most_common_words.push_back({ "world", 1 });

    // ImageInfo info1 { "img1", 640, 480 };
    // res.largest_images.push_back(info1);
    // res.largest_images.push_back({ "img2.png", 200, 300 });

    // res.vacant_dirs.push_back("path1/subdir1");
    // res.vacant_dirs.push_back("test2/xyz");
    // return res;
