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
#include <stack>

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

    // split can sometimes get empty strings in by accident - added in to remove that issue
    if (!curr_word.empty()) {
        res.push_back(curr_word);
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

    // small method to extract myset from word2int class, used to get process names
    const std::unordered_map<std::string, int>& get_map() const {
        return myset;
    }

};

std::vector<std::string> readFile(){
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(std::cin, line)) {
        lines.push_back(line);
    }

    return lines;
}

// pseudocode didn't properly detect cycles, especially on example 2
bool topologicalSort(std::unordered_map<std::string, std::vector<std::string>> &graph, std::unordered_map<std::string, int> &outdegree, std::vector<std::string> &current_cycle){

    std::vector<std::string> result;
    std::stack<std::string> zeroes;

    // get all nodes with an outdegree of zero
    for (const auto &[node, degree] : outdegree) {
        if (degree == 0) {
            zeroes.push(node);
        }
    }

    // while len(s) > 0
    while (!zeroes.empty()){
        // remove top entry
        std::string current = zeroes.top();
        zeroes.pop();
        result.push_back(current);

        for (const auto &neighbor : graph[current]) {
            outdegree[neighbor]--;
            if (outdegree[neighbor] == 0) {
                zeroes.push(neighbor);
            }
        }
    }

    if (result.size() != outdegree.size()){
        for (const auto &[node, degree] : outdegree) {
            // get all numbers in cycle if unable to perform topological sort
            if (degree > 0 && node[0] == '$') {
                current_cycle.push_back(node.substr(1));
            }
        }
        return false;
    }

    return true;
}

bool topological_sort(std::vector<std::vector<int>> &graph, std::vector<int> &out_counts, std::vector<int> &current_cycle){
    std::vector<int> result;
    std::stack<int> zeroes;

    // get all nodes with outdegree of zero
    for (size_t i = 0; i < out_counts.size(); i++){
        if (out_counts[i] == 0){
            // push each index of the graph in zeroes
            zeroes.push(i);
        }
    }


    // iterate until empty
    while (!zeroes.empty()){
        int current = zeroes.top();
        zeroes.pop();
        result.push_back(current);

        // get each node in graph[current], and reduce all of their out counts
        for (size_t i = 0; i < graph[current].size(); i++){
            out_counts[graph[current][i]]--;

            // add each index that ends up having an out-degree of zero
            if (out_counts[graph[current][i]] == 0){
                zeroes.push(graph[current][i]);
            }

        }

    }

    // cycle detected case
    if (result.size() != out_counts.size()){
        for (size_t i = 0; i < out_counts.size(); i++){

            // push everything active node into the current cycle
            if (out_counts[i] > 0){
                current_cycle.push_back(i);
            }
        }

        return false;
    }

    return true;

}

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
Result find_deadlock(const std::vector<std::string> &edges) {

    // get indegree of each node, while pushing in their adjacent nodes

    std::vector<std::vector<int>> adj_list;
    std::vector<int> out_counts;
    Word2Int w2i;

    Result result;

    for (size_t i = 0; i < edges.size(); i++){

        // split vector to get process and resource
        std::vector<std::string> splitted = split(edges[i].c_str());

        // $ denotes process, # denotes resource
        int n1 = w2i.get('$' + splitted[0]);
        int n2 = w2i.get('#' + splitted[2]);

        // get max of both - + 1 to be inclusive of n1 or n2
        size_t to_resize = std::max(n1, n2) + 1;

        // resize accordingly
        if (adj_list.size() < to_resize){
            adj_list.resize(to_resize);
            out_counts.resize(to_resize, 0);
        }

        // direction cases
        if(splitted[1] == "<-"){

            adj_list[n1].push_back(n2);
            out_counts[n2]++;

        }

        else{

            adj_list[n2].push_back(n1);
            out_counts[n1]++;

        }

        std::vector<int> temp_out = out_counts;
        std::vector<int> current_cycle;

        
        // cycle detected case
        if (!topological_sort(adj_list, temp_out, current_cycle)) {

            // make a temporary variable to be stored into result.procs
            std::vector<std::string> result_procs;

            // get map from w2i
            std::unordered_map<std::string, int> copy_set = w2i.get_map();

            // for each number in the current cycle, look for the matching value and return the following key
            for (auto & num : current_cycle){
                for (auto & [key, value] : copy_set){
                    if (value == num && key[0] == '$'){
                        result_procs.push_back(key.substr(1));
                    }
                }
    
            }

            result.procs = result_procs;
            result.index = i;
            return result;
        }
        

    }

    /*
    std::unordered_map<std::string, std::vector<std::string>> graph;
    std::unordered_map<std::string, int> outdegree;

    for (size_t i = 0; i < edges.size(); i++) {
        std::vector<std::string> cur = split(edges[i].c_str());

        // process denoted as $, resource denoted as #
        std::string proc = '$' + cur[0];
        std::string res = '#' + cur[2];

        // an x in graph[x] shows where it points to
        if (cur[1] == "<-") {

            graph[proc].push_back(res);
            outdegree[res]++;
            if (outdegree.find(proc) == outdegree.end()){
                outdegree[proc] = 0;
            }

        } else {
            
            graph[res].push_back(proc);
            outdegree[proc]++;
            if (outdegree.find(res) == outdegree.end()){
                outdegree[res] = 0;
            }
        }

        std::unordered_map<std::string, int> temp_indegree = outdegree;
        std::vector<std::string> current_cycle;

        
        if (!topologicalSort(graph, temp_indegree, current_cycle)) {
            result.procs = current_cycle;
            result.index = i;
            return result;
        }
    }

    /*
    for (const auto& [node, neighbors] : graph) {
        std::cout << node << " -> ";
        if (neighbors.empty()) {
            std::cout << "(no outgoing edges)";
        } else {
            for (size_t i = 0; i < neighbors.size(); ++i) {
                std::cout << neighbors[i];
                if (i < neighbors.size() - 1) {
                    std::cout << ", ";
                }
            }
        }
        std::cout << "\n";
    }
    
    std::cout << "\nIn-degree counts:\n";
    std::cout << "----------------\n";
    for (const auto& [node, degree] : indegree) {
        std::cout << node << ": " << degree << "\n";
    }

    size_t totalEdges = 0;
    for (const auto& [node, neighbors] : graph) {
        totalEdges += neighbors.size();
    }
    
    std::cout << "\nSummary:\n";
    std::cout << "- Total nodes: " << indegree.size() << "\n";
    std::cout << "- Total edges: " << totalEdges << "\n\n";
    */

    result.index = -1;
    return result;
}
