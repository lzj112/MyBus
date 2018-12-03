#include <iostream>
using namespace std;


void test(int (*a)[20], int n) 
{
    printf("%ld\n", sizeof(a));
}

int main() 
{
    int a[20];
    test(&a, 20);    
}