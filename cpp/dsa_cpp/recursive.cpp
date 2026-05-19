// C++ program to swap two
// nibbles in a byte
#include <sys/socket.h>

#include "../header.h"
using namespace std;
void bin(int n) {
    cout<<n<<"      "<<bitset<8>(n)<<endl;
}
enum class Types {
    first,
    second,
    third,
    fourth,
    fifth
} FIRST, SECOND, THIRD, FOURTH;

int swapNibbles(int x)
{
    int b1=(x&0x0F);
    bin(b1);
    int b=(b1<<4);
    int b2=(x&0xF0);
    bin(b2);
    int c=(b2) >> 4;

    int a=b|c;
    bin(b);
    bin(c);
    bin(a);

}

// Driver code
int main()
{
    cout<<typeid(FIRST).name()<<endl;
    // cout<<(int)SECOND<<endl;
    // cout<<(int)THIRD<<endl;
    return 0;
}

//This code is contributed by Shivi_Aggarwal