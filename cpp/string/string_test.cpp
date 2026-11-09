#include "../header.h"

void func() {
    printf(" i am a thread");
}

int main(int argc,char*argv[]) {
    char *arr="this is an array";
    char char_arr[]="this is a char array";
    cout<<arr<<endl;;
    string s1{arr};
    string s2{char_arr};
    cout<<s1<<endl;
    cout<<s2<<endl;
    string sentense="32424234this1i13454325and aome of the $ and currect !#*&^%$";
    vector<char> alphanum;
    vector<char> alpha;
    vector<char> num;
    vector<char> special;


    for(int i=0;i<sentense.size();i++) {
        if (isalnum(sentense[i])) {
            alphanum.push_back(sentense[i]);
        }
        if(isalpha(sentense[i])) {
            alpha.push_back(sentense[i]);
        }
        if(isdigit(sentense[i]))
            num.push_back(sentense[i]);
        if(not isalnum(sentense[i]) and not isalpha(sentense[i] and not isdigit(sentense[i]))) {
            special.push_back(sentense[i]);
        }

    }
    print_one(alphanum);cout<<endl;
    print_one(alpha);cout<<endl;
    print_one(num);print("\n");
    print_one(special);print("\n");
    cout<<sentense.contains("3");
    cout<<sentense.substr(3,6);
    cout<<stod(sentense)<<endl;;
    cout<<stol(sentense)<<endl;;
    cout<<stoull(sentense)<<endl;;
    cout<<to_string(123344.087568765)<<endl;
    cout<<to_string(123344.45673457356)<<endl;
    cout<<int('4')<<endl;;
    cout<<char(68)<<endl;
    cout<<(sentense.find("aome"))<<endl;;
    sentense[7]='}';
    cout<<sentense<<endl;
    while(sentense.size()) {
        cout<<sentense.back()<< " ";
        sentense.pop_back();
    }
    cout<<string::npos<<endl;
    cout<<"This sentance is "<< sentense<<endl;

}

