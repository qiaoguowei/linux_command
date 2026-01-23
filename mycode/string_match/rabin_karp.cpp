#include <iostream>
#include <string>

int rkMatch(const std::string& S, const std::string& P) {
    int lenS = S.size();
    int lenP = P.size();

    if (lenP == 0 || lenP > lenS) return -1;

    const long long Base = 911382629;
    const long long Mod = 1e9 + 7;

    // 步骤1：计算模式串的哈希值 hashP
    long long hashP = 0;
    for (char c : P) {
        hashP = (hashP * Base + static_cast<long long>(c)) % Mod;
    }

    // 步骤2：计算主串第一个窗口的哈希值 hashS
    long long hashS = 0;
    for (int i = 0; i < lenP; ++i) {
        hashS = (hashS * Base + static_cast<long long>(S[i])) % Mod;
    }

    // 预计算 BASE^(lenP-1) mod MOD（滑动哈希用）
    long long basePow = 1;
    for (int i = 0; i < lenP - 1; ++i) {
        basePow = (basePow * Base) % Mod;
    }

    // 步骤3：滑动窗口比对哈希
    for (int i = 0; i <= lenS - lenP; ++i) {
        // 哈希值相等时，逐字符验证（避免哈希冲突）
        if (hashS == hashP) {
            if (S.substr(i, lenP) == P) {
                return i;
            }
        }
        // 计算下一个窗口的哈希值（滑动优化）
        if (i < lenS - lenP) {
            // 公式：hash = (hash - 左字符*BASE^(lenP-1)) * BASE + 右字符
            hashS = (hashS - static_cast<long long>(S[i]) * basePow % Mod + Mod) % Mod;
            hashS = (hashS * Base + static_cast<long long>(S[i + lenP])) % Mod;
        }
    }

    return -1;
}

int main() {
    std::string S = "ABCABCDABABCDABCDABDE";
    std::string P = "ABCDABD";
    char *p = new char[5];

    std::cout << S << std::endl;
    std::cout << P << std::endl;
    std::cout << rkMatch(S, P) << std::endl;
}
