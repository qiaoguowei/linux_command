#include <iostream>
#include <vector>
#include <algorithm>

int main() {

    std::vector<int> vec1 = {4, 3, 5, 6, 7, 3};
    std::vector<int> vec2 = {10, 23, 5, 1, 2};

    std::vector<int> result;

    sort(vec1.begin(), vec1.end());
    sort(vec2.begin(), vec2.end());


    int size1 = vec1.size();
    int size2 = vec2.size();

    int idx1 = 0;
    int idx2 = 0;
    while (true) {
        if (idx1 > size1 - 1 && idx2 > size2 - 1)
            break;
        if (idx1 > size1 - 1)
            result.push_back(vec2[idx2++]);
        else if (idx2 > size2 - 1)
            result.push_back(vec1[idx1++]);

        if (vec1[idx1] < vec2[idx2])
            result.push_back(vec1[idx1++]);
        else
            result.push_back(vec2[idx2++]);
    }

    for (auto& x : result)
    {
        std::cout << x << ", ";
    }
    std::cout << std::endl;
}
