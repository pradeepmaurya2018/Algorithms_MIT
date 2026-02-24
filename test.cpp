#include<iostream>
#include <unordered_map>
using namespace std;
#include <map>

auto f() {
	return 0;
}
int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	cout << "Hello world";
	cout << "why us this simple and ";
	unordered_map<int, int> m;
	// m[3]="pradeep";
	// m[4]="maurya";
	// m[6]="bangalore";
	m[5]++;
	// return f();
	for (auto p:m) {
		cout<<p.first<<" "<<p.second<<endl;

	}
}