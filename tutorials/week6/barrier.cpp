
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <pthread.h>
class simple_barrier {
  std::mutex m_;
  std::condition_variable cv_;
  int n_remaining_, count_;
  bool coin_;

  public: 
  simple_barrier(int count = 1) { init(count); }
  void init(int count)
  {
    count_ = count;
    n_remaining_ = count_;
    coin_ = false;
  }
  bool wait()
  {
    if (count_ == 1) return true;
    std::unique_lock<std::mutex> lk(m_);
    if (n_remaining_ == 1) {
      coin_ = ! coin_;
      n_remaining_ = count_;
      cv_.notify_all();
      return true;
    }
    auto old_coin = coin_;
    n_remaining_--;
    cv_.wait(lk, [&]() { return old_coin != coin_; });
    return false;
  }
};

using namespace std;

simple_barrier barrier = simple_barrier();
mutex myMutex;
int64_t num;
bool readSuc;

void thread_task (int id)
{
    
    myMutex.lock();
    cout << "Thread #" << id << " working on number " << num << endl;
    myMutex.unlock();
}

bool read() {
    if( ! (std::cin >> num) )
        return false;
    else
        return true; 
}

void  thread_Function(int tid) {
    //bool readSuc = false;
    myMutex.lock();
    cout << "Thread #" <<  tid << " started!" << endl;
    myMutex.unlock();
    while(1) {
        // synchronize all threads
        
        if(barrier.wait())
        {
            cout << "Single thread - run serial work by " <<tid<< endl;
            myMutex.lock();
            readSuc = read();
            myMutex.unlock();
            cout << "finished reading: "<< readSuc << endl;
        }
     
        // synchronize again before starting parallel work
        barrier.wait();   

        // run thread_task in parallel 
        if (readSuc == true)        // if read was successful, run threads
        {
            thread_task(tid);
        }
        else
        {
            myMutex.lock();
            cout << "EXITING!" << endl;
            myMutex.unlock();

            break;
        }
    }


}

int main( )
{
    /// parse command line arguments
    int n_threads = 5;
    thread thread_pool[n_threads];
  
    barrier.init(n_threads);
    for(int i = 0; i < n_threads; i++){
       thread_pool[i] = thread(thread_Function, i);
    }

    for (int i = 0; i < n_threads; i++) {
       if (thread_pool[i].joinable()) thread_pool[i].join();
    }
    
    cout << "done!" << endl;
    return 0;
}

