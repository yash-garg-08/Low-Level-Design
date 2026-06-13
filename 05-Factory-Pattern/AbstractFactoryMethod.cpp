// We crate a factory(class) which creates objects
// We want to seperate our business logic from the object creation

// 3. Abstract Factory method
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

class GarlicBread{
    public:
        virtual void prepare() = 0;
        virtual ~GarlicBread() {}
};

class StandardGarlicBread : public GarlicBread{
    public:
        void prepare() override {
            cout << "Standard garlic bread ...." << endl;
        }
};

class PremiumGarlicBread : public GarlicBread {
    public:
        void prepare() override{
            cout << "Premium garlic bread ...." <<endl;
        }
};

class WheatStandardGarlicBread : public GarlicBread { 
    public:
        void prepare() override {
            cout << "Wheat standard garlic bread ...." << endl;
        }
};

class WheatPremiumGarlicBread : public GarlicBread {
    public:
        void prepare() override{
            cout << " Wheat premium garlic bread ...." <<endl;
        }
};


// =========================================================================================================================

class MealFactory {
    public:
        virtual Burger* CreateBurger(string& type) = 0;
        virtual GarlicBread* CreateGarlicBread(string& type) = 0;
};

class SinghBurger : public MealFactory{
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

        GarlicBread* CreateGarlicBread(string& type) override{
            if(type == "standard"){
                return new StandardGarlicBread();
            }else if(type == "premium"){
                return new PremiumGarlicBread();
            }else{
                cout << "Invalid burger type ...." << endl;
                return nullptr;
            }
        }
};

class KingBurger : public MealFactory{
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

        GarlicBread* CreateGarlicBread(string& type) override{
            if(type == "standard"){
                return new WheatStandardGarlicBread();
            }else if(type == "premium"){
                return new WheatStandardGarlicBread();
            }else{
                cout << "Invalid burger type ...." << endl;
                return nullptr;
            }
        }
};

// =========================================================================================================================

int main(){
    string type = "standard";

    MealFactory* mySinghBurgerFactory = new KingBurger();

    Burger* burger = mySinghBurgerFactory->CreateBurger(type);

    GarlicBread* garlicBread = mySinghBurgerFactory->CreateGarlicBread(type);

    burger->prepare();

    garlicBread->prepare();

    return 0;

}