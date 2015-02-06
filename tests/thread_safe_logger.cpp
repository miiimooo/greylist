#include <fstream>
#include "thread_safe_log.h"
thread_safe_log get_app_log()
{
    static std::ofstream out( "out.txt" );
    static internal_thread_safe_log_ownthread log( out);
    return thread_safe_log( log);
}
int main(int argc, char *argv[])
{
  thread_safe_log applog = get_app_log();
  
  return 0;
}

