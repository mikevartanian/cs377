#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>


class Request
{
    private:
	int id;
	int ttl;
    public:
	Request(int id, int ttl)
	{
	    this->id = id;
	    this->ttl = ttl;
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
char* getTime()
{
    time_t timev;
    return asctime(localtime(&timev));
}

class BoundedBuffer
{
      private:
	sem_t mutex;
	sem_t empty;
	sem_t full;
	Request* buffer;
	int globalID;
	int circularIndex;
	int N;

    public:
	BoundedBuffer(int N)
	{
	    sem_init(&mutex,1,1);
	    sem_init(&empty,1,N); 
	    sem_init(&full,1,0);
	    buffer = (Request*)malloc(N*sizeof(Request));
	    Request dummyRequest(-1,0);
	    for(int i=0;i<N;i++)
	    {
		buffer[i] = dummyRequest;
	    }
	    globalID = 0;
	    circularIndex = 0;
	    this->N = N;
	}
	void Producer()
	{
	    while(true)
	    {
	    circularIndex = 0;
	    Request r(globalID,5);
	    sem_wait(&empty);
	    sem_wait(&mutex);
	    globalID++;
	    printf("Producer: produced request ID %d, length %d at\n at %s",r.getID(),r.getTTL(),getTime());
	    while(buffer[circularIndex].getID()!=-1)
	    {
		circularIndex++;
	    }
	    buffer[circularIndex] = r;
	    sem_post(&mutex);
	    sem_post(&full);
	    }
	}
	void Consumer()
	{
	   while(true)
	   {
	    circularIndex = 0;
	    sem_wait(&full);
	    sem_wait(&mutex);
            while(buffer[circularIndex].getID()==-1)
	    {
		circularIndex++;
	    }
	    Request r = buffer[circularIndex];
	    printf("Consumer processing request %d\n",r.getID());
	    sleep(5);
	    Request dummyRequest(-1,0);
	    buffer[circularIndex] = dummyRequest;
	    sem_post(&mutex);
	    sem_post(&empty);
	   }
	}
};

void* performWork(void* arg)
{
    BoundedBuffer* buffer = (BoundedBuffer*)arg;
    buffer->Consumer();
    pthread_exit(NULL);
}
	    
int main(int argc, char* argv[])
{   
    int numThreads = atoi(argv[1]);
    int maxTTL = atoi(argv[2]);
    BoundedBuffer* b = new BoundedBuffer(numThreads);
    pthread_t threads[numThreads];
    while(true)
    {
    b->Producer();
    for(int i=0;i<numThreads;i++)
    {
	b->Consumer();
	pthread_create(&threads[i],NULL,performWork,(void*) b);
    }
    for(int i=0;i<numThreads;i++)
    {
	pthread_join(threads[i],NULL);
    }
    }
    pthread_exit(NULL);
    return 0;
}

