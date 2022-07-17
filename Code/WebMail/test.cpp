#include <iostream>
#include <boost/random.hpp>
// #include <boost/random/random_device.hpp>

int main()
{
    // 1、随机数发生器:boost::random::mt19937,生成伪随机数

    {
        //实现了任意区间的实数分布随机数，类型是double类型
        boost::random::mt19937 gen(time(0));
        boost::random::uniform_real_distribution<> dist(1.0, 100.0);
        //测试产生随机数
        for (int i = 0; i < 10; ++i)
        {
            std::cout << dist(gen) << std::endl;
        }
    }

    return 1;
}