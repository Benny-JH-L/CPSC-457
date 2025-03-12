

void thread_function(int threadIndex, int n_threads, const std::vector<int64_t>& nums){
    while(true){
        // true until proven false
        isPrime[threadIndex] = true;

        // serial task
        // last number to get to barrier gets the next number
        if (barrier.wait()){
            calculationsOngoing = true;

            // increment index and get current number AND its square root instead of having to calculate it multiple times
            nextIndex++;
            currentNum = nums[nextIndex];
            currentMax = sqrt(currentNum.load());
            // no more numbers left
            if (nextIndex >= nums.size()){
                // signal no more work
                global_finished = true;
            }
            // assign work for each thread, dynamically
            else{
                if (currentNum < 2 or currentNum % 2 == 0 or currentNum % 3 == 0){
                    // base case found - no calculations needed
                    calculationsOngoing = false;
                    // if either 2 or 3 still true
                    if (currentNum != 2 and currentNum != 3){
                        isPrime[threadIndex] = false;
                    }
                }

                else{
                    int64_t numDivisors = (currentMax - 5) / 6 + 1;
                    int64_t portionSize = numDivisors / n_threads;
                    int64_t remainder = numDivisors % n_threads;
                    
                    int64_t currentDivisorIndex = 0;
                    for (int i = 0; i < n_threads; i++) {
                        int64_t startIndex = currentDivisorIndex;
                        int64_t endIndex = startIndex + portionSize - 1;
                    
                        // Spread the remainder evenly across the first threads
                        if (i < remainder) {
                            endIndex++;
                        }
                    
                        // Map the divisor indices back to actual divisor values
                        tasks[i].start_x = 5 + startIndex * 6;
                        tasks[i].end_x = 5 + endIndex * 6;
                    
                        // Ensure the last thread ends cleanly at currentMax
                        if (tasks[i].end_x > currentMax) {
                            tasks[i].end_x = currentMax;
                        }
                    
                        currentDivisorIndex = endIndex + 1;
                    }
                                    }
        
            }
        }

        barrier.wait();

        // parallel task
        // if all threads check and no more work, break
        if(global_finished) break;

        /*
        // check int
        int64_t i = 5 + 6 * threadIndex;
        int64_t localMax = currentMax;
        */
        // if base case, skip all calculations
        if (calculationsOngoing){
            // start, end indices
            int64_t i = tasks[threadIndex].start_x;
            int64_t localMax = tasks[threadIndex].end_x;
            // code given from hint in tutorial
            while (calculationsOngoing && i <= localMax) {
                if ((currentNum % i == 0 || currentNum % (i + 2) == 0) ) {
                    isPrime[threadIndex] = false;
                    calculationsOngoing = false;
                }
                i += 6;
            }    
        }

        // serial task to combine results
        if (barrier.wait()){
            bool allPrime = true;
            for (int i = 0; i < n_threads; i++){
                if (!isPrime[i]){
                    allPrime = false;
                    break;
                }
            }

            if (allPrime){
               result.push_back(currentNum); 
            }
        }

        barrier.wait();

    }
}
