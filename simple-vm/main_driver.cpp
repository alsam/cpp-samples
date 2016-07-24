/*
 * =====================================================================================
 *
 *       Filename:  main_driver.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/17/2010 10:23:43 PM MSD
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexander Samoilov (Mr)
 *
 * =====================================================================================
 */

#include <vector>
#include "get_opt.hpp"
#include "vm.hpp"

using namespace std;

int main (int argc, char *argv[])
{
  try {
    get_opt& _ = get_opt::instance();

    _.parse_cmdline(argc, argv);

    VM vm;
    vm.load_image (_.inp_name);
    vm.interprete ();
  }

  catch (exception& e) {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
}

