#include<iostream>

class CircularArray
{
    private:
	int current;
	int size;
	int* ar;
    public:
	CircularArray(int size)
	{
	    current = 0;
	    ar = new int[size];
	    this->size = size;
	}
	void insert(int item)
	{
	    ar[current%size] = item;
	    current++;
	}
	int get(int index)
	{
	    return ar[index];
	}
	int pop()
	{
	    int ret = ar[0];
	    for(int i=1;i<size;i++)
	    {
		ar[i-1] = ar[i];
	    }
	    return ret;
	}
	void print()
	{
	    for(int i=0;i<size;i++)
	    {
		std::cout << ar[i] <<std::endl;
	    }
	}
};

int main()
{
    CircularArray ar(10);
    for(int i=0;i<20;i++)
    {
	ar.insert(i);
    }
    ar.insert(111);
    ar.insert(112);
    ar.print();
}

