#include <iostream>
#include <string>
#include <vector>

void buildNext(const std::string& P, std::vector<int>& next) {
    int lenP = P.size();
    next.resize(lenP, 0);
    int j = 0; // 前缀指针（最长相等前后缀的长度）

    for (int i = 1; i < lenP; ++i) {
        // 前后缀不匹配，回退j到next[j-1]
        while (j > 0 && P[i] != P[j]) {
            j = next[j - 1];
        }
        // 前后缀匹配，j++
        if (P[i] == P[j]) {
            j++;
            next[i] = j;
        } else {
            next[i] = 0; // 无相等前后缀
        }
    }
}

int kmpMatch(const std::string& S, const std::string& P) {
    int lenS = S.size();
    int lenP = P.size();

    if (lenP == 0 || lenP > lenS) return -1;

    std::vector<int> next;
    buildNext(P, next);
    int j = 0; //P字符串指针，始终不回退到负数

    for (int i = 0; i < lenS; ++i) { //S串指针不回退
        // 匹配失败时，模式串指针回退到最优位置
        while (j > 0 && S[i] != P[j]) {
            j = next[j - 1];
        }

        // 字符匹配，模式串指针后移
        if (S[i] == P[j]) {
            j++;
        }

        // 匹配成功：模式串指针到达末尾
        if (j == lenP) {
            return i - lenP + 1;
        }
    }

    return -1;
}