#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <thread>
#include <cinttypes>

// Number of total lock/unlock pair.
constexpr size_t N_PAIR = 16'000'000;

constexpr size_t NCOUNTER = 1;
constexpr size_t CACHE_LINE = 64;

static __thread int8_t counter[CACHE_LINE*NCOUNTER];

static bool spinlock = false;

static size_t nthr = 0;

void spin_lock(bool *l)
{
  while (__atomic_test_and_set(l, __ATOMIC_ACQUIRE))
    ;
}

void spin_unlock(bool *l)
{
  *l = false;
}

void inc_thread(size_t id)
{
  size_t n = N_PAIR / nthr;
  assert(n * nthr == N_PAIR);
  std::cout << "thread " << id << " launched" << std::endl;

  spin_lock(&spinlock);
  for (size_t j = 0; j < NCOUNTER; ++j)
    ++counter[j*CACHE_LINE];
  spin_unlock(&spinlock);
}

int main(int argc, char **argv)
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
