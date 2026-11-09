#include "../include/Task.hpp"

Task::Task(int id, const string name, int priority) {
    this->tsk_ID=id;
    this->name=name;
    this->priority=priority;
    cout<<"This is task implementation"<<endl;
}
