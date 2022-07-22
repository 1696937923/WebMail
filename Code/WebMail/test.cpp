#include <iostream>
using namespace std;

void test(char *a)
{
    snprintf(a, 5, "dadasd");
}

int main()
{
    char a[5];
    test(a);
    std::cout << a << std::endl;
    return 1;
}
