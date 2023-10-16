#include "ThreadPool.hpp"

#include <iostream>
#include <chrono>
#include <mutex>
#include <queue>

using namespace dpool;

std::mutex coutMtx;  // protect std::cout

void task(int taskId)
{    
    {
        std::lock_guard<std::mutex> guard(coutMtx);
        std::cout << "task-" << taskId << " begin!" << std::endl;                    
    }

    // executing task for 2 second
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    {
        std::lock_guard<std::mutex> guard(coutMtx);
        std::cout << "task-" << taskId << " end!" << std::endl;        
    }
}


void monitor(const ThreadPool &pool, int seconds)
{
    for (int i = 1; i < seconds * 10; ++i)
    {
        {
            std::lock_guard<std::mutex> guard(coutMtx);
            std::cout << "thread num: " << pool.threadsNum() << std::endl;                    
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));            
    }
}

int main_old(int argc, char *argv[])
{
    // max threads number is 100
    ThreadPool pool(100); 

    // monitoring threads number for 13 seconds    
    pool.submit(monitor, std::ref(pool), 4);
    
    // submit 100 tasks
    for (int taskId = 0; taskId < 10; ++taskId)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pool.submit(task, taskId);
    }
    
    return 0;
}



int compute(int a, int b)
{
	int r = rand()%1400+1000;
	std::cout<<"compute start "<< a<<"+"<<b <<",sleep = "<<r<<std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(r));
	std::cout<<"compute end "<< a<<"+"<<b<<std::endl;
    return a + b;
}

using namespace std; 

template <typename T>
class safe_queue {
public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    T pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        T value = queue_.front();
        queue_.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
};


safe_queue<std::shared_future<int>> q;

void my_sleep(int ms){
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void getter(){
	cout<<"start get "<<endl;
	while(true){
		
		while(q.empty()){
			my_sleep(100);
		}
		std::shared_future<int> f = q.pop();
		int v = f.get();
		std::cout<<"-----------------"<<v<<std::endl;
	}
}

int main()
{
    // 设置最大线程数为 10
    dpool::ThreadPool pool(30);
    
    pool.submit(getter);
    
    for(int i=0;i<100;i++){
    	my_sleep(30);
    	std::shared_future<int> fut = pool.submit(compute, i, 0);    
		q.push(fut);
	} 
	
    return 0;
}




