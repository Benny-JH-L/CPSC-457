/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the ONLY file you should edit and submit to D2L

/*
CPSC457 Winter 2025 | Assignment 4 | Benny Liang | 30192142
*/

#include "find_deadlock.h"
#include <iostream>

#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <stack>

using namespace std;

const char processID = '$';
const char resourceID = '#'; 

/// feel free to use the code below if you like:
/// ----------------------------------------------------------------
/// split(s) splits string s into vector of strings (words)
/// the delimiters are 1 or more whitespaces
// static std::vector<std::string> split(const std::string & s)
static void split(const std::string & s, vector<string>& res)
{
    auto linec = s + " ";
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
}

// debugging
void print_vec(vector<int>& v)
{
    for (auto& e : v)
        cout << e << " ";
    cout << endl;
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
class Word2Int 
{
    int counter = 0;
    unordered_map<string, int> myset;
    vector<string> vec_reverse_myset;

public:
    int get(const string & w)
    {
        auto f = myset.find(w);
        if (f == myset.end()) {
            myset[w] = counter;
            vec_reverse_myset.emplace_back(w);
            return counter++;
        }

        return f->second;
    }

    // gets the string associated with the `int i`
    string& reverse_get(const int& i)
    {
        return vec_reverse_myset[i];
    }

    unordered_map<string, int> getMap()
    {
        return myset;
    }
};

// Graph class
class Graph
{
private:
    vector<vector<int>> graph;  // stores the index of nodes that are pointing to this node (an adjacency list)
    vector<int> vec_out_counts; // for index `i` (node `i`) has an out-degree count vec_out_counts[i]
    u_int64_t graph_size = 0;

public:
    Word2Int w2i;

    void addNode(const string& edge)
    {
        vector<string> edgeInfo;
        split(edge, edgeInfo);

        // debug
        // cout << "\nedge entered:\n";
        // for (auto& e : edgeInfo)
        //     cout << e;
        // cout << endl;
    
        string process = processID + edgeInfo[0];
        string arrow = edgeInfo[1];
        string resource = resourceID + edgeInfo[2];
    
        // debug
        // cout << "\n--altered:\nProcess: " << process << endl;
        // cout << "Arrow: " << arrow << endl;
        // cout << "Resource: " << resource << endl;
        // note: processes will be on the left side of the `arrow` and resources
        // will be on the right side of the `arrow`.
    
        int index_process = w2i.get(process);
        int index_resource = w2i.get(resource);
    
        // debug
        // cout << "index for process: " << process << " = " << index_process << endl;
        // cout << "index for resource: " << resource << " = " << index_resource << endl;
    
        int index_bigger = max(index_process, index_resource);

        // resize vectors if needed
        if (index_bigger >= vec_out_counts.size())  // note: vec_out_counts.size() == graph.size()
        {
            // cout << "Resizing vec_out... to size: " << index_bigger+1 << endl;  // debug
            // cout << "Resizing adj_list... to size: " << index_bigger+1 << endl;  // debug

            graph.resize(index_bigger + 1);                     // initialize all new indices with empty vectors.
            vec_out_counts.resize(index_bigger + 1, 0);         // initialize all new indices with 0.
        }
    
        // process requesting resource (P -> R)
        if (arrow == "->")
        {
            // add the process to adj list of process
            graph[index_resource].emplace_back(index_process);  // set that resource is being pointed by a process 
            
            // increment out degree of resource
            vec_out_counts[index_process]++;
            // printf("vec_out[%d] = %d\n", index_process, vec_out_counts[index_process]); // debug
        }
        // process receiving resource (P <- R)
        else
        {
            // add the resource to adj list of process
            graph[index_process].emplace_back(index_resource);   // set that process is being pointed by a resource 
            
            // increment out degree of resource
            vec_out_counts[index_resource]++;
            // printf("vec_out[%d] = %d\n", index_resource, vec_out_counts[index_resource]); // debug
        }
        graph_size = vec_out_counts.size(); // update graph size
    }

    // return's true if a cycle was detected, false otherwise, 
    
    // implmenting this: if a cycle was detected, adds the processes inside the cycle to `vec_proc_in_deadlock`
    bool topologicalSort(vector<int>& vec_proc_in_deadlock)
    // bool topologicalSort(vector<string>& vec_proc_in_deadlock)
    {    
        int removed_graph_size = 0;         // num nodes that are `removed` from the graph
        stack<int> stack_out_degree_0;      // stores the indices of nodes with out-degree 0.
        vector<int> copy_out_counts = vec_out_counts;

        // get all current nodes with in-degree of zero
        for (u_int64_t i = 0; i < copy_out_counts.size(); i++)
        {
            if (copy_out_counts[i] == 0)
                stack_out_degree_0.push(i);  // add index `i`, this node index has out-degree 0 
        }

        // iterate until there are no more nodes in the graph with out-degree 0
        while (!stack_out_degree_0.empty())
        {
            int node_with_out0 = stack_out_degree_0.top();
            stack_out_degree_0.pop();
            removed_graph_size++;

            // get each node in graph[node_with_out0], and reduce all of their out counts
            for (u_int64_t i = 0; i < graph[node_with_out0].size(); i++)
            {
                copy_out_counts[graph[node_with_out0][i]]--;    // dercrement the out-degree of the node whose pointing to node index `node_with_out0`

                // add each node index that ends up having an out-degree of zero
                if (copy_out_counts[graph[node_with_out0][i]] == 0)
                    stack_out_degree_0.push(graph[node_with_out0][i]);
            }

        }

        // cycle detected
        if (removed_graph_size != copy_out_counts.size())       // the graph is not empty (ie, num removed nodes != graph size)
        {
            for (u_int64_t i = 0; i < copy_out_counts.size(); i++)
            {
                // push everything active node into the current cycle
                if (copy_out_counts[i] > 0)
                    vec_proc_in_deadlock.emplace_back(i);
            }
            return true;    // cycle detected
        }

        // if (removed_graph_size != copy_out_counts.size())       // the graph is not empty (ie, num removed nodes != graph size)
        // {
        //     for (u_int64_t i = 0; i < copy_out_counts.size(); i++)
        //     {
        //         if (copy_out_counts[i] > 0)
        //         {
        //             const string& name = w2i.reverse_get(i);  // get the node name associated with node index
        //             if (name[0] == processID)                 // check if its a proccess
        //                 vec_proc_in_deadlock.emplace_back(name.substr(1)); // add it to `vec_proc_in_deadlock`, removing the proccessID delimiter

        //         }
        //     }
        //     // for each node index in the cycle, add proccesses that are in the cycle to the `result.procs`
        //     // for (auto & num : vec_proc_in_deadlock)
        //     // {
        //     //     const string& name = w2i.reverse_get(num);  // get the node name associated with node index
        //     //     if (name[0] == processID)                   // check if its a proccess
        //     //         vec_result_proc.emplace_back(name.substr(1));   // add it to `result.procs`, removing the proccessID delimiter
        //     // }
        //     return true;    // cycle detected
        // }



        return false;   // no cycle detected
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
    Result result;
    Word2Int w2i;
    Graph graph;

    int count = 0;

    for (auto& edge : edges)
    {
        graph.addNode(edge);    // add edge to graph
        vector<int> vec_proc_in_deadlock;

        if (graph.topologicalSort(vec_proc_in_deadlock))    // check is there a cycle
        // if (graph.topologicalSort(result.procs))    // check is there a cycle
        {
            Word2Int& w2i = graph.w2i;
            vector<string>& vec_result_proc = result.procs;

            // for each node index in the cycle, add proccesses that are in the cycle to the `result.procs`
            for (auto & num : vec_proc_in_deadlock)
            {
                const string& name = w2i.reverse_get(num);  // get the node name associated with node index
                if (name[0] == processID)                   // check if its a proccess
                    vec_result_proc.emplace_back(name.substr(1));   // add it to `result.procs`, removing the proccessID delimiter
            }

            result.index = count;   // index where cycle was found
            return result;
        }
        count++;
    }

    result.index = -1;
    return result;
}
