#include <iostream>
#include <pthread.h>
#include <atomic>

using namespace std;

class bakeryLock
{
    private:

    atomic <bool> * choose;       // Using atomic operations to avoid race conditions
    atomic <int> * ticketNum;
    int threadNum;

    public:
    
    bakeryLock()
    {
        choose = nullptr;
        ticketNum = nullptr;
        threadNum = 0;
    }

    bakeryLock(int n)
    {
        this->threadNum = n;

        choose = new atomic <bool> [threadNum];  // boolean array to store whether a thread is currently choosing a number or not

        for(int i = 0; i < threadNum; i++)
        {
            choose[i] = false;
        }

        ticketNum = new atomic <int> [threadNum]; // integer array to store the threads assigned ticket number

        for(int i = 0; i < threadNum; i++)
        {
            ticketNum[i] = 0;
        }


    }

    void lock(int tid)
    {
        choose[tid] = true;    //  Marks the thread as true to indicate that it is currently in the process of selecting a number

        int max = 0;

        for(int i = 0; i < threadNum; i++)
        {
              if(ticketNum[i] > max)
              {
                max = ticketNum[i];      // Checking the maximum among all the threads
              }
        }

        ticketNum[tid] = max + 1;   // Assigning the ticket number to the current thread

        choose[tid] = false;       // Marks the current thread as done in choosing the number

        for(int i = 0; i < threadNum; i++)
        {
            while(choose[i])
            {
                //busy waiting
            }

            while(ticketNum[i] != 0 && (ticketNum[i] < ticketNum[tid] || (ticketNum[i] == ticketNum[tid] && i < tid)))
            {
                // busy waiting
            }
        }

    }

   
   void unlock(int tid)
   {
      ticketNum[tid] = 0;
   }

};

bakeryLock lock1;

void * criticalThread(void * arg)
{
    int tid = *(int*)arg;

    lock1.lock(tid);

    cout << "Thread No. " << tid << " is currently in the critical section." << endl;
    
    lock1.unlock(tid);

    pthread_exit(NULL);

}

int main()
{
    int num = 0;

    cout << "Enter number of threads to create: ";
    cin >> num;

    pthread_t threads[num];
    int tid[num];

    lock1 = bakeryLock(num);

    for(int i = 0; i < num; i++)
    {
        tid[i] = i;
        pthread_create(&threads[i], NULL, criticalThread ,&tid[i]);
    }

    
    for (int i = 0; i < num; i++)
    {
        pthread_join(threads[i], NULL);
    }

    
    return 0;
}