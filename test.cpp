#include <iostream>
using namespace std;

long long fivonach(int num);

int main(){
    long long a =0;
    cout << "please input fivonach number:";
    cin >> a;
    long long ans = 0;

    ans = fivonach(a);
    cout << "fivonach:" << a <<"\nnumber is:" << ans << endl;
}

long long fivonach(int num){
    if(num <= 1){
        return num;
    }

    return fivonach(num - 1) + fivonach(num - 2);
}