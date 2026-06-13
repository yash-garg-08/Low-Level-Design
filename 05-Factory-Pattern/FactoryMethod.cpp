// We crate a factory(class) which creates objects
// We want to seperate our business logic from the object creation

// 2. Factory method
#include<iostream>
using namespace std;

// =========================================================================================================================

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

class WheatStandardBurger : public Burger {
    public:
        void prepare() override{
            cout << "Standard wheat burger ...." << endl;
        }
};

class WheatPremiumBurger : public Burger {
    public:
        void prepare() override {
            cout << "Standard premium burger ...." << endl;
        }
};

// =========================================================================================================================

class BurgerFactory{
    public:
        virtual Burger* CreateBurger(string& type) = 0;
};

class SinghBurger : public BurgerFactory{
    public:
        Burger* CreateBurger(string& type) override{
            if(type == "standard"){
                return new StandardBurger();
            }else if(type == "premium"){
                return new PremiumBurger();
            }else{
                cout << "Invalid burger type ...." << endl;
                return nullptr;
            }
        }
};

class KingBurger : public BurgerFactory{
    public:
        Burger* CreateBurger(string& type) override{
            if(type == "standard"){
                return new WheatStandardBurger();
            }else if(type == "premium"){
                return new WheatPremiumBurger();
            }else{
                cout << "Invalid burger type ...." << endl;
                return nullptr;
            }
        }
};

// =========================================================================================================================

int main(){
    string type = "standard";

    BurgerFactory* mySinghBurgerFactory = new KingBurger();

    Burger* burger = mySinghBurgerFactory->CreateBurger(type);

    burger->prepare();

    return 0;

}