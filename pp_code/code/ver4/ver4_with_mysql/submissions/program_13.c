#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>

int main()
{
    // Randomly generate n to print sequence
    srand(time(0));
    int n = (rand() % (10 - 0 + 1)) + 0;
    // int n = 10;
    for(int i = 1; i <= n; i++)
    {
        printf(" %d", i);
    }
    // Uncomment below two lines for runtime error check 
    // int x = 10, y = 0;
    // int result = x / y;
    return 0;
}