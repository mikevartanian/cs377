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
	    circularIndex = 0;
	    sem_wait(&empty);
	    sem_wait(&mutex);
	    Request r(globalID,5);
	    globalID++;
	    printf("Producer: produced request ID %d, length %d at\n",r.getID(),r.getTTL());
	    while(buffer[circularIndex].getID()!=-1)
	    {
		circularIndex++;
	    }
	    buffer[circularIndex] = r;
	    sem_post(&mutex);
	    sem_post(&full);
	}
	void Consumer()
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
	    Request dummyRequest(-1,0);
	    buffer[circularIndex] = dummyRequest;
	    sem_post(&mutex);
	    sem_post(&empty);
	}
};

	    
int main(int argc, char* argv[])
{
    BoundedBuffer b(10);
    while(1)
    {
	b.Producer();
	b.Consumer();
    }
    return 0;
}

