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
    int64_t curr_time = 0;
    std::vector<int> rq, jq;

    int starts_recorded = 0;
    int starts_max = processes.size();

    // initialize rest of burst times
    std::vector<int64_t> remaining_bursts(processes.size(), -1);
    for (auto &p : processes) {
        jq.push_back(p.id);
        remaining_bursts[p.id] = p.burst;
    }

    // sort job queue by arrival time
    std::sort(jq.begin(), jq.end(), [&](const int a, const int b) {

        return processes[a].arrival < processes[b].arrival;
    });

    int curr_process = -1;

    while (true){

        // finish if done
        if (jq.empty() && rq.empty()){
            break;
        }

        // if job queue isn't empty but ready queue is
        if (!jq.empty() && rq.empty()){
            // get next process
            int next_process = jq[0];

            // idle state
            seq.push_back(-1);            

            // jump to arrival of next process
            curr_time = processes[next_process].arrival;
            processes[next_process].start_time = curr_time;

            // kick out of job queue, and add to ready queue
            curr_process = next_process;
            jq.erase(jq.begin());
            rq.push_back(curr_process);

        }

        // if jq is empty but rq isn't
        else if (jq.empty() && !rq.empty()){


            // optimization still can only be done when through max_seq_len
            if (max_seq_len <= seq.size() && starts_recorded == starts_max){
                // optimization 2, given from hint in tutorial
                // get minimum remaining burst, initialize at highest possible value
                int64_t min_remaining_burst = INT64_MAX;
                int rounds = rq.size();


                // iterate through every burst
                for (int id : rq){
                    int64_t cur = remaining_bursts[id];

                    min_remaining_burst = std::min(cur, min_remaining_burst);
                }

                // get largest value of n s.t. no processes will finish during this time
                int64_t n_val = min_remaining_burst / (rq.size() * quantum);
                
                // since there are no incoming processes at this time, do not need to account the value of n in accordance to processes arriving

                if (n_val > 0){
                    // increment curr_time accordingly
                    curr_time += (n_val * rounds * quantum);

                    // subtract time from each of the processes
                    for (int id: rq){
                        remaining_bursts[id] -= n_val * quantum;

                        // since we're subtracting multiple rounds, check for premature ending rounds here
                        if (remaining_bursts[id] < 0){

                            // correct for early ending by getting time
                            int diff = std::abs(remaining_bursts[id]);
                            curr_time -= diff;
                            processes[id].finish_time = curr_time;

                            rq.erase(std::remove(rq.begin(), rq.end(), id), rq.end());
                        }

                        // if ending at 0, then just remove
                        else if (remaining_bursts[id] == 0){
                            rq.erase(std::remove(rq.begin(), rq.end(), id), rq.end());
                        }

                    }
                }

            }


            // optimization 1 - only done when execution sequence completed and have all start processes recorded
            /*
            if (max_seq_len <= seq.size() && starts_recorded == starts_max){
                int rounds = rq.size();

                curr_time += (rounds * quantum);

                // subtract one quantum from each remaining burst
                for (int id : rq){
                    remaining_bursts[id] -= quantum;

                }

                // check for any premature ends
                int min_diff = quantum;

                for (int id : rq){
                    if (remaining_bursts[id] < 0){

                        // get time difference
                        int diff = abs(remaining_bursts[id]);
                        min_diff = std::min(min_diff, diff);

                        // set new finish time
                        processes[id].finish_time = curr_time + quantum - diff;
                        remaining_bursts[id] = 0;

                        // remove process from rq
                        rq.erase(std::remove(rq.begin(), rq.end(), id), rq.end());
                    }
                    
                    else if (remaining_bursts[id] == 0){
                        rq.erase(std::remove(rq.begin(), rq.end(), id), rq.end());
                    }

                }

                // subtract extra time added
                curr_time -= min_diff;
            }
            */
            
            else {
                // get next process
                curr_process = rq[0];
                rq.erase(rq.begin());

                if (processes[curr_process].start_time == -1){
                    processes[curr_process].start_time = curr_time;
                    starts_recorded++;
                }

                if ((seq.empty() || seq.back() != curr_process) && seq.size() < max_seq_len){
                    seq.push_back(curr_process);
                }

                // subtract time from remaining burst by quantum
                remaining_bursts[curr_process] -= quantum;

                // if result > 0, push back into rq
                if (remaining_bursts[curr_process] > 0){
                    rq.push_back(curr_process);
                    curr_time += quantum;
                }

                // if result is exactly equal to 0, keep going
                else if (remaining_bursts[curr_process] == 0){
                    curr_time += quantum;
                    processes[curr_process].finish_time = curr_time;

                }

                // if process ends prematurely, keep track of how early it ends and modify time accordingly
                else if (remaining_bursts[curr_process] < 0){
                    int diff = abs(remaining_bursts[curr_process]);

                    curr_time += (quantum - diff);
                    processes[curr_process].finish_time = curr_time;                
                }

            }
        }

        // last case - both queues aren't empty
        else if (!jq.empty() && !rq.empty()){

            // optimization 2
            if (max_seq_len <= seq.size() && starts_recorded == starts_max){

                // get minimum remaining burst, initialize at highest possible value
                int64_t min_remaining_burst = INT64_MAX;
                int rounds = rq.size();


                // iterate through every burst
                for (int id : rq){
                    int64_t cur = remaining_bursts[id];

                    min_remaining_burst = std::min(cur, min_remaining_burst);
                }

                // get largest value of n s.t. no processes will finish during this time
                int64_t n1_val = min_remaining_burst / (rq.size() * quantum);

                // get largest posible value of n s.t. no processes will arrive during this time
                // to do this, we can check the earliest arriving process time
                int64_t n2_val = (processes[jq[0]].arrival-curr_time)/((int)rq.size()*quantum);

                // get the minimum of the two, depending on whichever one (process or min burst) happens earlier
                int64_t n_val = std::min(n1_val, n2_val);

                // subtract
                if (n_val > 0){
                    // increment curr_time accordingly
                    curr_time += (n_val * rounds * quantum);

                    // subtract time from each of the processes
                    for (int id: rq){
                        remaining_bursts[id] -= n_val * quantum;

                        // since we're subtracting multiple rounds, check for premature ending rounds here
                        if (remaining_bursts[id] < 0){

                            // correct for early ending by getting time
                            int diff = std::abs(remaining_bursts[id]);
                            curr_time -= diff;
                            processes[id].finish_time = curr_time;

                            rq.erase(std::remove(rq.begin(), rq.end(), id), rq.end());
                        }

                        // if ending at 0, then just remove
                        else if (remaining_bursts[id] == 0){
                            rq.erase(std::remove(rq.begin(), rq.end(), id), rq.end());
                        }

                    }
                }
            
            }

            else{
                // get next process
                curr_process = rq[0];
                rq.erase(rq.begin());

                if (processes[curr_process].start_time == -1){
                    processes[curr_process].start_time = curr_time;
                    starts_recorded++;
                }

                if ((seq.empty() || seq.back() != curr_process) && seq.size() < max_seq_len){
                    seq.push_back(curr_process);
                }

                // subtract time from remaining burst by quantum
                remaining_bursts[curr_process] -= quantum;

                // if result > 0, push back into rq
                if (remaining_bursts[curr_process] > 0){
                    curr_time += quantum;

                    // "If your simulation detects that an existing process exceeds its time slice at the same time as a new process
                    // arrives, you need to insert the existing process into the ready queue before inserting the newly arriving process."

                    std::vector<int64_t> same_time;

                    // first, check if anything arrived within the current time frame
                    // check beginning and end, as we erase items in the queue as we go
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
                    
                    // THEN push back into rq
                    rq.push_back(curr_process);

                    // AND THEN push back all ongoing processes that arrive at same time as the current process
                    for (auto & a : same_time){
                        rq.push_back(a);
                    }

                }

                // if result is exactly equal to 0, keep going
                else if (remaining_bursts[curr_process] == 0){
                    // update time accordingly
                    curr_time += quantum;
                    processes[curr_process].finish_time = curr_time;

                    // check beginning and end, as we erase items in the queue as we go
                    for (auto id = jq.begin(); id != jq.end();) {
                        if (processes[*id].arrival <= curr_time) {
                            rq.push_back(*id);
                            id = jq.erase(id); // erase returns the next valid iterator
                        } else {
                            ++id; // only increment if not erasing
                        }
                    }

                }

                // if process ends prematurely, keep track of how early it ends and modify time accordingly
                else if (remaining_bursts[curr_process] < 0){
                    int diff = abs(remaining_bursts[curr_process]);
                    curr_time += (quantum - diff);

                    processes[curr_process].finish_time = curr_time;
                    

                    // check beginning and end, as we erase items in the queue as we go
                    for (auto id = jq.begin(); id != jq.end();) {
                        if (processes[*id].arrival <= curr_time) {
                            rq.push_back(*id);
                            id = jq.erase(id); // erase returns the next valid iterator
                        } else {
                            ++id; // only increment if not erasing
                        }
                    }
                    
                }

            }

        }

    }

}
