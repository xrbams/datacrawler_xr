#include "./modules/module_d/scheduler.hpp"
#include "./modules/module_b/storage.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    cout << "This is my first go at Concurrent Web Crawler" << endl;

    Scheduler s;
    s.scheduler();

    return 0;
}
