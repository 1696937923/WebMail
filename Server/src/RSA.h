#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>
#include <iostream>
#include <fstream>
#include <string>

using namespace boost::multiprecision;
using int2048_t = number<cpp_int_backend<2048, 2048, signed_magnitude, unchecked, void>>;
using int4096_t = number<cpp_int_backend<4096, 4096, signed_magnitude, unchecked, void>>;
boost::random::mt19937 ran(time(0));

class RSA
{
public:
    // rsa 2048bit encrypt
    std::string Encrypt(std::string);

    // rsa 2048bit decrypt
    std::string Decrypt(std::string);

    // gen rsa_public_key & rsa_secret_key in file
    void Gen_Key();

private:
    // return base^exp % mod
    int2048_t FastExpMod(int2048_t, int2048_t, int2048_t);

    /*
        扩展欧几里得算法
        return y, gcd(e,m)
        其中 (y*e) % m = 1
    */
    int2048_t ExtendedEuclidean(int2048_t, int2048_t);
    int Cal_Bit(unsigned int);

    //根据a^n-1 mod n = 1,其中如果n为素数
    bool Miller_Rabin_Test(int2048_t);
    int2048_t Gen_Random(int);
    int2048_t Gen_Prime(int);

    std::string StrToHex(std::string, int);
    int2048_t HexToInt(std::string);
    std::string IntToHex(int2048_t);
    std::string HexToStr(std::string);
};