// #include <boost/multiprecision/cpp_int.hpp>
// #include <random>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/multiprecision/miller_rabin.hpp> //素数
#include <iostream>

using namespace boost::multiprecision;
using int2048_t = number<cpp_int_backend<2048, 2048, signed_magnitude, unchecked, void>>;

class RSA
{
public:
    int2048_t Encrypt();
    int2048_t Decrypt();

private:
    int2048_t FastExpMod();        //平方乘算法
    int2048_t ExtendedEuclidean(); //扩展欧几里得算法
};
int main()
{
    boost::random::mt19937 gen(time(0));
    int2048_t lowNum = int2048_t(1) << 1022;
    int2048_t highNum = int2048_t(1) << 1024;
    boost::random::uniform_real_distribution<int2048_t> dist(lowNum, highNum);
    //测试产生随机数
    for (int i = 0; i < 10; ++i)
    {
        std::cout << dist(gen) << std::endl;
    }

    // time_t now = std::time(0);
    // std::cout << now << std::endl;
    // boost::random::mt19937 ran(now);

    // int1024_t lowNum = int1024_t(1) << 1022;
    // int1024_t highNum = int1024_t(1) << 1023;
    // boost::random::uniform_real_distribution<int1024_t> dist(lowNum, highNum);

    // for (int i = 0; i < 2; i++)
    // {
    //     std::cout << dist(ran) << std::endl;
    // }
}