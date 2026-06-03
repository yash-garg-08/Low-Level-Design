#include<iostream>
using namespace std;


// Strategy interface for walkable
class WalkableRobot {
    public:
        virtual void walk() = 0;
        virtual ~WalkableRobot() {}
};

// concrete strategies for walk
class NormalWalk : public WalkableRobot {
    public:
        void walk() override {
            cout << "Walking normally...." << endl;
        }
};

class NoWalk : public WalkableRobot{
    public:
        void walk() override {
            cout << "Not walking...." << endl;
        }
};

// Strategy interface for talkable
class TalkableRobot{
    public:
        virtual void talk() = 0;
        virtual ~TalkableRobot() {}
};

class NormalTalk : public TalkableRobot{
    public:
        void talk() override {
            cout << "Talking normally...." << endl;
        }
};

class NoTalk : public TalkableRobot{
    public:
        void talk() override {
            cout << "Not talking...." << endl;
        }
};

// Robot base class
class Robot{
    protected:
        WalkableRobot* walkableBehaviour;
        TalkableRobot* talkableBehaviour;

    public:
        Robot(WalkableRobot* w, TalkableRobot* t){
            this->walkableBehaviour = w;
            this->talkableBehaviour = t;
        }

        void walk(){
            walkableBehaviour->walk();
        }

        void talk(){
            talkableBehaviour->talk();
        }

        virtual void projection() = 0;

};

// Concrete robot types
class Companion : public Robot{
    public:
        Companion(WalkableRobot* w, TalkableRobot* t) : Robot(w, t){}

        void projection() override{
            cout << "Displaying companion robot...." << endl;
        }
};

class WorkerRobot : public Robot{
    public:
        WorkerRobot(WalkableRobot* w, TalkableRobot* t) : Robot(w, t){}

        void projection() override{
            cout << "Displaying worker robot...." << endl;
        }
};

// Main function
int main(){
    Robot* robot1 = new Companion(new NormalWalk(), new NormalTalk);
    robot1->walk();
    robot1->talk();
    robot1->projection();

    cout << "--------------------------------------" << endl;

    Robot* robot2 = new WorkerRobot(new NormalWalk(), new NoTalk());
    robot2->walk();
    robot2->talk();
    robot2->projection();
    return 0;
}