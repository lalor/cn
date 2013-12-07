#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
    char d[] = {'h', 'e', 'l'};
    char *c = d;
    while ( *c )
    {
        cout << *c++ << endl;
    }
    return 0;
}
