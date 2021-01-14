# Thread Pool

## Usage
Example:
```cpp
#include <iostream>
#include "thread_pool.h"

void mellow() { std::cout << "mellow mellow\n"; }

struct A
{
    void print() { std::cout << "hello hello\n"; }
};

int main()
{
    thread_pool pool(4);

    A a;
    auto hello = [&]() { a.print(); };

    pool.execute(hello);
    pool.execute(mellow);
    pool.execute(hello);
    pool.execute(mellow);

    std::cout.flush();
    return 0;
}
```

Possible output:
```
hello hello
hello hello
mellow mellow
mellow mellow
```

