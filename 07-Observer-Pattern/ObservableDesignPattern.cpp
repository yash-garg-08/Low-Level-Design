#include<iostream>
using namespace std;

// =========================================================================================================================

// Abstract observer interface(subscriber)
class Isubscriber{
    public:
        virtual void update() = 0;
        virtual ~Isubscriber() {}
};

// =========================================================================================================================

// Abstract observable interface(yt channel)
class Ichannel{
    public:
        virtual void subscribe(Isubscriber* s) = 0;
        virtual void unSubscribe(Isubscriber* s) = 0;
        virtual void notifySubscribers() = 0;
        virtual ~Ichannel() {}
};

// =========================================================================================================================

// Concrete class of observale (youtube channel)
class Channel : public Ichannel{
    private:
        vector<Isubscriber*> subscribers;
        string name;
        string latestVideo;
    public:
        Channel(const string& name){
            this->name = name;
        }

        void subscribe(Isubscriber* subscriber) override{
            if(find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end()){
                subscribers.push_back(subscriber);
            }
        }

        void unSubscribe(Isubscriber* subscriber) override{
            auto it = find(subscribers.begin(), subscribers.end(), subscriber);
            if(it != subscribers.end()){
                subscribers.erase(it);
            }
        }

        void notifySubscribers() override{
            for(Isubscriber* sub : subscribers){
                sub->update();
            }
        }

        void  uploadVideo(const string& title){
            latestVideo = title;
            cout << "\n[" << name << " uploaded \"" << title << "\"]\n";
            notifySubscribers();
        }

        string getVideoData(){
            return "\nCHeckout our new Video : " + latestVideo + "\n";
        }
};

// =========================================================================================================================

// Concrete class of subscriber
class Subscriber : public Isubscriber{
    private:
        string name;
        Channel* channel;
    
        public:
            Subscriber(const string& name, Channel* c){
                this->name = name;
                this->channel = c;
            }
            
            // called by Channel, print noti message
            void update() override{
                cout << "Hey " << name << "," << this->channel->getVideoData() << endl;
            }
};

// =========================================================================================================================

int main(){
    // creating the channel
    Channel* channel = new Channel("WarLord");

    // creating two subscribers
    Subscriber* sub1 = new Subscriber("s1", channel);
    Subscriber* sub2 = new Subscriber("s2", channel);

    // subscribers subscribing the channel
    channel->subscribe(sub1);
    channel->subscribe(sub2);

    // channel is uploading the video
    channel->uploadVideo("This is my first video.");

    // sub1 unsubscribe
    channel->unSubscribe(sub1);

    // channel uploads 2nd video
    channel->uploadVideo("This is my second video");

    return 0;

}