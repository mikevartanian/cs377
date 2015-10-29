#include<iostream>
#include<unistd.h>

using namespace std;
int main()
{
    time_t t = time(0);
    cout << asctime(localtime(&t));
    sleep(5);
    t = time(0);
    cout << asctime(localtime(&t));
    return 0;
}


