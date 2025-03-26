/// =========================================================================
/// Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
/// All Rights Reserved. Do not distribute this file.
/// =========================================================================
/// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include <algorithm>
#include <cstdio>

// this is the function you should implement
//
// simulate_rr() implements a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//
void simulate_rr(
    int64_t quantum, 
    int64_t max_seq_len,
    std::vector<Process> & processes,
    std::vector<int> & seq
) 
{
    // stuff in the sequence for some reason, clear it
    seq.clear();

    int64_t curr_time = 0;
    std::vector<int> rq, jq;

    // used for optimization case
    int64_t starts_recorded = 0;
    int64_t starts_max = processes.size();

    // initialize rest of burst times
    std::vector<int64_t> remaining_bursts(processes.size(), -1);
    for (auto &p : processes) {
        jq.push_back(p.id);
        remaining_bursts[p.id] = p.burst;
    }

    // sort job queue by arrival time
    // didn't realize this was already done for us
    /*
    std::sort(jq.begin(), jq.end(), [&](const int a, const int b) {

        return processes[a].arrival < processes[b].arrival;
    });
    */

    int64_t curr_process = -1;

    while (true){


        // finish if done
        if (jq.empty() && rq.empty()){
            break;
        }

        if (!jq.empty() && rq.empty()){
            // get next process
            int64_t next_process = jq[0];

            // jump to arrival of next process, set start time
            // don't need to check if started or not here
            curr_time = processes[next_process].arrival;
            processes[next_process].start_time = curr_time;

            // increment starts_recorded
            starts_recorded++;

            // if starting after 0, push in -1 to indicate idle state
            if (curr_time > 0 && seq.size() < max_seq_len){
                seq.push_back(-1);            
            }

            // kick out of job queue, and add to ready queue
            jq.erase(jq.begin());
            rq.push_back(next_process);

        }

        // if jq is empty but rq isn't
        if (jq.empty() && !rq.empty()){
		
            // get next process
            curr_process = rq[0];
            
            // kick it out
            rq.erase(rq.begin());

            // if hasn't started yet
            if (processes[curr_process].start_time == -1){
                processes[curr_process].start_time = curr_time;
                starts_recorded++;
            }

            // if can increment by quuantum
            if (quantum < remaining_bursts[curr_process]){
                curr_time += quantum;
                remaining_bursts[curr_process] -= quantum;
            }

            // else can end prematurely
            else{
                curr_time += remaining_bursts[curr_process];
                remaining_bursts[curr_process] = 0;

            }

            // if finished, set end time
            if (remaining_bursts[curr_process] == 0){
                processes[curr_process].finish_time = curr_time;
            }

            // else push back
            else{
                rq.push_back(curr_process);
            }
            
            // push to seq
            if (seq.empty() || ((seq.back() != curr_process) && seq.size() < max_seq_len)){
                seq.push_back(curr_process);
            }

        }

        if (!jq.empty() && !rq.empty()){

            // get next process
            curr_process = rq[0];
            
            // kick it out (so i don't forget)
            rq.erase(rq.begin());

            // record start if haven't
            if (processes[curr_process].start_time == -1){
                //printf("%ld %ld\n", curr_process, curr_time);
                processes[curr_process].start_time = curr_time;
                starts_recorded++;
            }

            // printf("%ld\n", curr_time);

            // check for any processes arriving before end
            std::vector<int64_t> same_time;

            // if can increment by quuantum
            if (quantum < remaining_bursts[curr_process]){
                curr_time += quantum;
                remaining_bursts[curr_process] -= quantum;
            }

            // else can end prematurely
            else{
                curr_time += remaining_bursts[curr_process];
                remaining_bursts[curr_process] = 0;

            }

            // if finished, set finish time
            if (remaining_bursts[curr_process] == 0){
                processes[curr_process].finish_time = curr_time;
            }

            // pushing back into rq later

            // If your simulation detects that an existing process exceeds its time slice at the same time as a new process
            // arrives, you need to insert the existing process into the ready queue before inserting the newly arriving process.

            // check for processes arriving during the quantum
            for (auto id = jq.begin(); id != jq.end();) {
                if (processes[*id].arrival < curr_time) {
                    rq.push_back(*id);
                    id = jq.erase(id); // erase returns the next valid iterator
                } 
                // get everything that arrives at current time in separate array, to be added later
                else if (processes[*id].arrival == curr_time){
                    same_time.push_back(*id);
                    id = jq.erase(id);
                }
                else {
                    ++id; // only increment if not erasing
                }
            }


            // push back if not done
            if (remaining_bursts[curr_process] > 0){
                rq.push_back(curr_process);
            }

            // then push back everything that arrived at same time

            for (int id : same_time){
                rq.push_back(id);
            }

            // push to seq if a new prrocess
            if (seq.empty() || (seq.back() != curr_process) && seq.size() < max_seq_len){
                seq.push_back(curr_process);
            }

        }

        // optimization can be performed once ready queue isn't empty
        if (!rq.empty()){

            /*
            if (seq.size() == max_seq_len){
                printf("a");
            }

            if (rq.size() == 1 && seq.size() < max_seq_len){
                printf("b");
            }
            */

            /*
            if (starts_recorded == rq.size()){
                printf("%ld %ld\n", starts_recorded, rq.size());
            }
            */

            //printf("%ld %ld\n", starts_recorded, rq.size());
            //printf("%ld\n", curr_time);

            // only works if sequence is done, or if rq is only size 1 (need to jump to next)
            // only need to check if all starts are recorded amongst the rq (greater condition for those that finish)
            if (((seq.size() == max_seq_len) || (rq.size() == 1 && seq.size() < max_seq_len)) && starts_recorded >= rq.size()){

                // get minimum remaining burst, initialize at highest possible value
                int64_t min_remaining_burst = INT64_MAX;
                int64_t rounds = rq.size();

                // iterate through every burst
                for (int id : rq){
                    int64_t cur = remaining_bursts[id];

                    min_remaining_burst = std::min(cur, min_remaining_burst);
                }

                // n1 only possible if greater than minimum remaining burst
                // n2 possible if greater than next arrival time

                int64_t n1_val = INT64_MAX;
                int64_t n2_val = INT64_MAX;

                // if the quantum can fit into the minimum remaining burst, get the amount of times it can do so
                if (quantum < min_remaining_burst){
                    // idk why but -1 helps
                    n1_val = (min_remaining_burst / quantum) - 1;
                }

                // jq can't be empty for this case to happen
                // can "jump" to next available value
                if (!jq.empty()){
                    n2_val = (processes[jq[0]].arrival-curr_time)/(rounds * quantum);
                }

                int64_t n_val = std::min(n1_val, n2_val);

                // if the value actually changes, use it
                // if not, add/subtract regularly
                if (n_val != INT64_MAX){
                    curr_time += n_val * rounds * quantum;

                    for (int id : rq){
                        remaining_bursts[id] -= n_val * quantum;
                    }    
                }
            }
        }
        
    }
}