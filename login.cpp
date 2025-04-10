#include <iostream>
using namespace std;
string nama = "iyus";
int main(){
    string username = "admin123";
    string password = "pw123";
    string input_usn;
    string input_pw;
    bool login_status = false;

    cout << "=============== WELCOME ================" << endl;
    do {
        cout << "Insert username : "; cin >> input_usn;
        cout << "Insert password : "; cin >> input_pw;

        if (input_usn == username && input_pw == password){
            cout << "Login Success" << endl;
            login_status = true;
        } else {
            cout << "Login Failed, Check your username or password." << endl;
        }
    } while (login_status == false);

    return 0;
}