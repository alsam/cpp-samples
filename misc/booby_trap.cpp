#include <cstdlib>

// see also https://github.com/nemequ/portable-snippets/blob/master/debug-trap/debug-trap.h

void ForceCrash()
{
  *((int volatile*)NULL) = 0;
}

int main()
{
    ForceCrash();
}

