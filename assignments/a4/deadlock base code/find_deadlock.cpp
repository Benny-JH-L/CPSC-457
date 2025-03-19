/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the ONLY file you should edit and submit to D2L

#include "find_deadlock.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

/// feel free to use the code below if you like:
/// ----------------------------------------------------------------
/// split(s) splits string s into vector of strings (words)
/// the delimiters are 1 or more whitespaces
static std::vector<std::string> split(const std::string & s)
{
    auto linec = s + " ";
    std::vector<std::string> res;
    bool in_str = false;
    std::string curr_word = "";
    for (auto c : linec) {
        if (isspace(c)) {
            if (in_str)
                res.push_back(curr_word);
            in_str = false;
            curr_word = "";
        } else {
            curr_word.push_back(c);
            in_str = true;
        }
    }
    return res;
}

/// feel free to use the code below if you like:
/// ----------------------------------------------------------------
/// Word2Int is a class that can convert words to unique integers
/// get(word) returns the same number given the same word
///           the numbers will start at 0, then 1, 2, ...
///
/// example:
///   Word2Int w2i;
///   w2i.get("hello") = 0
///   w2i.get("world") = 1
///   w2i.get("hello") = 0
///
/// Word2Int "may" help you get a bit more performance
/// in your cycle finding algorithm, as arrays are faster
/// than hash tables...
///
class Word2Int {
    int counter = 0;
    std::unordered_map<std::string, int> myset;

public:
    int get(const std::string & w)
    {
        auto f = myset.find(w);
        if (f == myset.end()) {
            myset[w] = counter;
            return counter++;
        }
        return f->second;
    }
};

/// this is the function you need to (re)implement
/// -------------------------------------------------------------------------
/// parameter edges[] contains a list of request- and assignment- edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge. As soon as you detect a deadlock, your function
/// needs to stop processing edges and return an instance of Result structure
/// with 'index' set to the index that caused the deadlock, and 'procs' set
/// to contain names of processes that are in the deadlock.
///
/// To indicate no deadlock was detected after processing all edges, you must
/// return Result with index=-1 and empty procs.
///
Result find_deadlock(const std::vector<std::string> & edges)
{
    // let's try to guess the results :)
    Result result;
    result.procs = split(" 12 7  7 ");
    result.index = 6;
    return result;
}
