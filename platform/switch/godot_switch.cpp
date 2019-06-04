#include <vector>
#include "main/main.h"
#include "os_switch.h"

namespace nxlib {
#include <switch.h>
}

int main(int argc, char *argv[]) {

  #ifdef DEBUG_ENABLED  
  nxlib::socketInitializeDefault();
  nxlib::nxlinkStdio();
  #endif

  OS_Switch os;
  
  nxlib::romfsInit();

  // Hack so that we load the pack file from romfs
  std::vector<const char*> new_argv(argv, argv + argc);
  new_argv.push_back("--main-pack");
  new_argv.push_back("romfs:/main.pck");
  new_argv.push_back(nullptr);

  argv = const_cast<char**>(new_argv.data());
  argc = argc + 2;

  Error err = Main::setup(argv[0], argc - 1, &argv[1]);
  if (err != OK)
    return 255;

  if (Main::start())
    os.run();
  Main::cleanup();

  #ifdef DEBUG_ENABLED
  nxlib::socketExit();
  #endif

  return os.get_exit_code();
}

