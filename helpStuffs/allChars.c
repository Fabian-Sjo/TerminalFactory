#include <stdio.h>

int main(int argc, char const *argv[])
{
    char c = '!'-1;
    while (c++)
    {
        printf("\033[38;2;255;255;255m%c ",c);
        if(c%16 == 0)
            printf("\n");
    }
    
    
    return 0;
}
