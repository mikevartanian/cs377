#include<iostream>
#include<unistd.h>
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


class Circ
{
    private:
	Request* ar;
	int size;
	int current;
	int index;
    public:
	Circ(int size)
	{
	    this->size = size;
	    ar = (Request*)malloc(size*sizeof(Request));
	    current = 0;
	    index = 0;
	}
	void insert(Request item)
	{
	    ar[current%size] = item;
	    current++;
	}
	Request get()
	{
	    if(index==size)
	    {
		index = 0;
	    }
	    Request r = ar[index];
	    index++;
	    return r;
	}
};

char* getTime()
{
    time_t timev = time(0);
    return asctime(localtime(&timev));
}

class BoundedBuffer
{
      private:
	sem_t mutex;
	sem_t empty;
	sem_t full;
	Circ* buffer;
	int globalID;
	int maxTTL;

    public:
	BoundedBuffer(int N, int maxTTL)
	{
	    sem_init(&mutex,1,1);
	    sem_init(&empty,1,N); 
	    sem_init(&full,1,0);
	    buffer = new Circ(N);
	    globalID = 0;
	    this->maxTTL = maxTTL;
	}
	void Producer()
	{
	    while(true)
	    {
	    int ttl = (rand()%maxTTL)+1;
	    Request r(globalID,ttl);
	    sem_wait(&empty);
	    sem_wait(&mutex);
	    globalID++;
	    printf("Producer: produced request ID %d, length %d at\n at %s",r.getID(),r.getTTL(),getTime());
	    sleep(ttl);
            buffer->insert(r);
	    sem_post(&mutex);
	    sem_post(&full);
	    }
	}
	void Consumer()
	{
	   while(true)
	   {
	    sem_wait(&full);
	    sem_wait(&mutex);
	    Request r = buffer->get();
	    printf("Consumer processing request %d\n",r.getID());
	    printf("Consumer sleeping for the next %d seconds\n",r.getTTL());
	    sleep(r.getTTL());
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
void* masterWork(void* arg)
{
    BoundedBuffer* buffer = (BoundedBuffer*) arg;
    buffer->Producer();
    pthread_exit(NULL);
}
	    
int main(int argc, char* argv[])
{   
    int numThreads = atoi(argv[1]); 
    int maxTTL = atoi(argv[2]);
    BoundedBuffer* b = new BoundedBuffer(numThreads,maxTTL);
    pthread_t threads[numThreads]; //consumer threads
    pthread_t master; //producer thread
    for(int i=0;i<numThreads;i++)
    {
	pthread_create(&threads[i],NULL,performWork,(void*) b); //starts the consumers
    }
    pthread_create(&master,NULL,masterWork,(void*) b); //starts the producer
    pthread_exit(NULL);
    return 0;
}

