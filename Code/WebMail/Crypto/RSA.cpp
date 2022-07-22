#include "RSA.h"

std::string RSA::Encrypt(std::string plain_text)
{
    int2048_t plain_num = HexToInt(StrToHex(plain_text, plain_text.size()));
    std::string e, n;
    char buffer[1024];

    FILE *f = fopen("RSA_Public_Key.txt", "r");

    fgets(buffer, sizeof(buffer), f);
    e = (std::string)buffer;
    e.pop_back(); //除去文件中\n换行符

    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), f);
    n = (std::string)buffer;
    fclose(f);

    int2048_t secret_num = FastExpMod(plain_num, int2048_t(e), int2048_t(n)); //加密操作

    return IntToHex(secret_num);
}

std::string RSA::Decrypt(std::string secret_text)
{
    int2048_t secret_num = HexToInt(secret_text);
    std::string d, n;
    char buffer[1024];

    FILE *f = fopen("RSA_Secret_Key.txt", "r");

    fgets(buffer, sizeof(buffer), f);
    d = (std::string)buffer;
    d.pop_back(); //除去文件中\n换行符

    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), f);
    n = (std::string)buffer;
    fclose(f);

    int2048_t plain_num = FastExpMod(secret_num, int2048_t(d), int2048_t(n)); //解密操作

    return HexToStr(IntToHex(plain_num));
}

std::string RSA::StrToHex(std::string str, int len)
{
    std::string ret;
    const char *hex = "0123456789ABCDEF";
    for (int i = 0; i < len; i++)
    {
        char c = str[i];
        ret.push_back(hex[(c >> 4) & 0xf]); //取二进制高四位
        ret.push_back(hex[c & 0xf]);        //取二进制低四位
    }
    return ret;
}

int2048_t RSA::HexToInt(std::string hex_str)
{
    int2048_t ret;

    for (int i = 0; i < hex_str.length(); i++)
    {
        int exp = hex_str.length() - i - 1;
        int base;
        if (hex_str[i] <= '9' && hex_str[i] >= '0')
            base = hex_str[i] - '0';
        else if (hex_str[i] <= 'Z' && hex_str[i] >= 'A')
            base = hex_str[i] - 'A' + 10;
        else
            base = hex_str[i] - 'a' + 10;
        ret = ret + int2048_t(base) * FastExpMod(16, exp, -1);
    }

    return ret;
}

std::string RSA::IntToHex(int2048_t num)
{
    std::string hex_str;
    const char *hex = "0123456789ABCDEF";

    while (num != 0)
    {
        hex_str.insert(hex_str.begin(), hex[int(num % 16)]);
        num = num / 16;
    }

    return hex_str;
}

std::string RSA::HexToStr(std::string hex_str)
{
    std::string str;
    for (int i = 0; i < hex_str.length() / 2; i++)
    {
        //两个hex字符一组，拼接成一个字节，并且转化成字符。
        char ch = int(HexToInt(hex_str.substr(i * 2, 2)));
        str.push_back(ch);
    }

    return str;
}

void RSA::Gen_Key()
{
    int2048_t p = Gen_Prime(1024);
    int2048_t q = Gen_Prime(1024);
    int2048_t n = p * q;
    int2048_t m = (p - 1) * (q - 1);

    int2048_t e; //公钥
    int2048_t d; //私钥
    do
    {
        e = int2048_t(ran());
        d = ExtendedEuclidean(e, m);
    } while (d == -1);

    FILE *fp = fopen("RSA_Public_Key.txt", "w");
    FILE *fs = fopen("RSA_Secret_Key.txt", "w");

    std::string p_key = e.str();
    std::string s_key = d.str();
    std::string cal = n.str();

    fprintf(fp, "%s\n%s", p_key.c_str(), cal.c_str());
    fprintf(fs, "%s\n%s", s_key.c_str(), cal.c_str());
}

int2048_t RSA::FastExpMod(int2048_t b, int2048_t exp, int2048_t mod)
{
    if (mod != -1)
    {
        b = b % mod;
        exp = exp % mod;
    }
    int4096_t res = 1;
    int4096_t base = int4096_t(b);
    while (exp != 0)
    {
        if (exp & 1 == 1)
        {
            res = res * base;
            if (mod != -1)
                res = res % mod;
        }
        exp >>= 1;
        base = base * base;
        if (mod != -1)
            base = base % mod;
    }
    return int2048_t(res);
};

int2048_t RSA::ExtendedEuclidean(int2048_t e, int2048_t m)
{
    if (e == 0)
        return -1;

    int2048_t tmp = m;
    int2048_t tmp1, tmp2;
    int2048_t y = 1, s1 = 1;
    int2048_t x = 0, s2 = 0;
    int2048_t q = m / e, r = m % e;

    while (r != 0)
    {
        tmp1 = x;
        tmp2 = y;
        x = s1 - x * q;
        y = s2 - y * q;
        s1 = tmp1;
        s2 = tmp2;

        m = e;
        e = r;
        q = m / e;
        r = m % e;
    }

    if (e != 1)
        return -1;
    if (y < 0)
        y = y + tmp;
    return y;
}

int RSA::Cal_Bit(unsigned int num)
{
    int cnt = 0;
    while (num != 0)
    {
        num /= 2;
        cnt++;
    }
    return cnt;
}

bool RSA::Miller_Rabin_Test(int2048_t num)
{
    if (miller_rabin_test(num, 25, ran))
    {
        return true;
    }
    return false;
}

int2048_t RSA::Gen_Random(int bit_len)
{
    int2048_t lowNum = int2048_t(1) << (bit_len - 1);
    int2048_t highNum = int2048_t(1) << bit_len;

    unsigned int ranom_num = ran();
    int shift = 1024 - Cal_Bit(ranom_num);

    int2048_t tmp = int2048_t(ranom_num) << shift;
    int2048_t n = lowNum + tmp % (highNum - lowNum);

    if (n % 2 == 0)
        n = n + 1;
    return n;
}

int2048_t RSA::Gen_Prime(int bit_len)
{
    int cnt = 1;
    int2048_t res;
    do
    {
        res = Gen_Random(bit_len);
        cnt++;
    } while (!Miller_Rabin_Test(res));

    return res;
}

// int main()
// {
//     using namespace std;
//     RSA rsa;

//     string res = rsa.Encrypt("adiugauifg大家爱弟弟aigfafafasfs");
//     cout << "secret: " << res << endl;
//     cout << rsa.Decrypt(res) << endl;
// }