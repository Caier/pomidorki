#include <vector>
#include <Arduino.h>

class Task {
    protected:
        unsigned long milis = millis(),
                      prevMilis = 0;
        void virtual exec() = 0;
        

    public: 
        Task();
        void render() {
            milis = millis();
            exec();
            prevMilis = milis;
        }
};

std::vector<Task*> tasks;

Task::Task() {
    tasks.push_back(this);
}