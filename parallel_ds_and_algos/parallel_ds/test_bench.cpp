#include <iostream>
#include <vector>
#include <thread>
#include "stack_lock_free.hpp"

static size_t nthr = 0;

stack_lock_free<int> st;

void inc_thread(size_t id)
{
    if (id % 2 == 0) {
        st.push(id);
    } else {
        int nid = st.pop();
    }
}

auto main(int argc, char **argv) -> int
{
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <num of threads>\n";
    return EXIT_FAILURE;
  }

  std::vector<std::thread> threads;
  nthr = std::atoi(argv[1]);
  threads.resize(nthr);

  for (size_t i = 0; i < nthr; ++i) {
    threads[i] = std::thread(inc_thread, i);
  }

  for (size_t i = 0; i < nthr; ++i) {
    threads[i].join();
  }

    return EXIT_SUCCESS;
}
