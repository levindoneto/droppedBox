#include <string>
#include "../../client/headers/clientUser.hpp"
#include "../headers/dropboxUtils.hpp"

using namespace std;

class Process {
  private:
    int processId;
  public:
    Process() {};
    ~Process();
    void managerCommands(
      string command,
      string parameter,
      ClientUser* user
    );
    int getProcessId();
};
