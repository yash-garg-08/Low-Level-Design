#include<iostream>

using namespace std;

// Base case

class Singleton {
    private:

        static Singleton* instance;

        // setting constructor as private so nobody could access it
        Singleton(){
            cout << "Singleton Constructor called" << endl;
        }
    
    public:
        // a getter function to get the object of a singleton class
        static Singleton* getInstance(){ 
            if(instance == nullptr){
                instance = new Singleton();
            }
            return instance;
        }
};

// declaring the static variable
Singleton* Singleton::instance = nullptr;

// In case of eager initialisation we can do is, 
// drawback is if the class is heavy then it will always aloocated even if it is not used
// Singleton* Singleton::instance = new Singleton();

int main() {
    // calling a static funciton
    Singleton* s1 = Singleton::getInstance();
    Singleton* s2 = Singleton::getInstance();

    cout << (s1 == s2) << endl; 
} 