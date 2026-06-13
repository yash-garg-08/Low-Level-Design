// We crate a factory(class) which creates objects
// We want to seperate our business logic from the object creation

// 1. Simple Factory
#include<iostream>
using namespace std;

class Burger{
    public:
        virtual void prepare() = 0;
        virtual ~Burger() {}
};

class StandardBurger : public Burger {
    public:
        void prepare() override{
            cout << "Standard burger ....." << endl;
        }
};

class PremiumBurger : public Burger {
    public:
        void prepare() override {
            cout << "PremiumBurger burger ....." << endl;
        }
};

class BurgerFactory{
    public:
        Burger* CreateBurger(string& type){
            if(type == "standard"){
                return new StandardBurger();
            }else if(type == "premium"){
                return new PremiumBurger();
            }else{
                cout << "Invalid Burger type ...." << endl;
                return nullptr;
            }
        }
};

int main(){
    string type = "standard";

    BurgerFactory* myBurgerFactory = new BurgerFactory();

    Burger* burger = myBurgerFactory->CreateBurger(type);

    burger->prepare();

    return 0;

}