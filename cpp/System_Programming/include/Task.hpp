#include <iostream>
#include <string>
using namespace std;
enum class TaskState { Created, Running, Waiting, Stopped };

class Task {

private:
  int tsk_ID{};
  std::string name;
  int priority{};
  TaskState taskState;
  int cpuTimeMs{};
  int memoryKb{};

public:
  Task(int id, const string name, int priority);
};
