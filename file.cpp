#include<iostream>
#include<stdlib.h>
#include<pthread.h>
#include<queue>

pthread_mutex_t mutexsum;

class Request
{
    private:
	int id;
	int ttl; //Time of request
	bool isBlocked;
    public:
	Request(int id, int ttl)
	{
	    this->id = id;
	    this->ttl = ttl;
	    isBlocked = false;
	}
	void block()
	{
	    isBlocked = false;
	}
	void wakeup()
	{
	    isBlocked = true;
	}
	bool getBlocked()
	{
	    return isBlocked;
	}
	int getID()
	{
	    return id;
	}
	int getTTL()
	{
	    return ttl;
	}
};

class Semaphore
{
    private:
	int count;
	std::queue<Request> q;
    public:
	Semaphore(int count)
	{
	    this->count = count;
	}
	void Lock()
	{
	    pthread_mutex_lock(&mutexsum);
	}
	void Unlock()
	{
	    pthread_mutex_unlock(&mutexsum);
	}
	void Wait(Request r)
	{
	    Lock();
	    count = count -1;
	    if(count<0)
	    {
		q.push(r);
		r.block();
	    }
	    Unlock();
	}
	void Signal()
	{
	    Lock();
	    count++;
	    if(count<=0)
	    {
		Request r = q.front();
		q.pop();
		r.wakeup();
	    }
	    Unlock();
	}
};

class BoundedBuffer
{
    private:
	Semaphore *mutex;
	Semaphore *empty;
	Semaphore *full;
    public:
	BoundedBuffer(int count)
	{
	    mutex = new Semaphore(1);
	    empty = new Semaphore(0);
	    full = new Semaphore(count);
	}
};



void *work(void *arg)
{
    pthread_mutex_lock(&mutexsum);
    int* passedIn = (int*)arg;
    std::cout << *passedIn << std::endl;
    pthread_mutex_unlock(&mutexsum);
    pthread_exit(NULL);
}

char* getTime()
{
    time_t timev;
    return asctime(localtime(&timev));
}

int main(int argc, char *argv[])
{
    int numThreads = atoi(argv[1]);
    std::cout << numThreads << std::endl;
    pthread_t threads[numThreads];
    pthread_mutex_init(&mutexsum,NULL);
    std::queue<Request*> requests;
    for(int i=0;i<numThreads;i++)
    {
	std::cout << "Creating thread " << i << " at time " << getTime();
	requests.push(new Request(i,0));
    }
    for(int i=0;i<numThreads;i++)
    {
	int id = requests.front()->getID();
	requests.pop();
    	pthread_create(&threads[i],NULL,work,(void *) &id);
    }
    for(int i=0;i<numThreads;i++)
    {
    pthread_join(threads[i],NULL);
    }
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
    return 0;
}



