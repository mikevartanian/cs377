#include<semaphore.h>


int main(int argc, char* argv[])
{
    sem_t sem_name;
    sem_init(&sem_name,0,10);
    return 0;
}

