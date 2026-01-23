#include <string>
#include <iostream>

int bfMatch(const std::string& S, const std::string& P) {
    int lenS = S.size();
    int lenP = P.size();

    if (lenP == 0 || lenP > lenS) return -1;

    int i = 0;
    int j = 0;
    while (i < lenS && j < lenP) {
        if (S[i] == P[j]) {
            i++;
            j++;
        } else {
            i = i - j + 1;
            j = 0;
        }
    }

    return (j == lenP) ? (i - j) : -1;
}

int main() {
    std::string S = "ABCABCDABABCDABCDABDE";
    std::string P = "ABCDABD";

    std::cout << S << std::endl;
    std::cout << P << std::endl;
    std::cout << bfMatch(S, P) << std::endl;
}