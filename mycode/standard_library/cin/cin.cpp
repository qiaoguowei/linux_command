#include <iostream>

using std::cin;
using std::endl;
using std::cout;
//input: q w e a z
void test_cin() {

    char n, m, k, p, q;
    //std::cin >> std::noskipws >> n >> m >> k >> p >> q; //会读入空格
    std::cin >> n >> m >> k >> p >> q; //不会读入空格
    std::cout << n << std::endl;
    std::cout << m << std::endl;
    std::cout << k << std::endl;
    std::cout << p << std::endl;
    std::cout << q << std::endl;
}

//如果将例子中cin.getline()改为cin.getline(m,5,'a');
//当输入jlkjkljkI时输出jklj，输入jkaljkljkl时，输出jk
void test_cingetline() {
    char a;
    cin >> a; //cin 不吃掉换行符，在缓冲里面还有换行符
    char b[10];
    cin.getline(b, 10); //读到换行符才结束
    cout << a << endl;
    cout << b << endl;
}

//和cin.getline()类似，但是getline输入string流
void test_getline() {
    std::string str;
    getline(cin, str);
    cout << str << endl;
}

void test_cin_get() {

    char a[5];
    cin.get(a, 5);//实际只能读到4个字符，读到换行符结束或读到4个字节结束，会读入空格
    cout << a <<endl;

    //cin.get() 读一个字符返回
}

int main() {
//    test_cingetline();
    test_cin_get(); 
}