# chanzy_container
Some containers I used, like `flat_map` and `sorted_vector` etc.

**They have NOT been fully tested!!!**

# Usage
## xmake
For [xmake](https://xmake.io/) project, add my repository in YOUR `xmake.lua` 
and then you can require this package and add this package in the target you want like other xmake packages:
 
```lua
add_repositories("chanzy_repo https://github.com/CHanzyLazer/chanzy_repo.git")
add_requires("chanzy_container")
 
target("your target name")
     set_kind("binary")
     add_files("src/*.cpp")
     add_packages("chanzy_container")
```

For the xmake whose version is lower than `v2.7.2`, you need to set the branch manually:
```lua
add_repositories("chanzy_repo https://github.com/CHanzyLazer/chanzy_repo.git main")
```

Then you can include one of it on your source code like this:
```cpp
#include <chanzy/sorted_vector.hpp>
#include <iostream>

using chanzy::sorted_vector;

int main(int argc, char** argv) {
    sorted_vector<double> sv {1.0, 2.0, 1.0, 0.8, 3.0};
    for (auto& e : sv) std::cout << e << "; ";
    std::cout << std::endl;

    std::cout << "position of find(0.8): " << (sv.find(0.8)-sv.begin()) << std::endl;
    std::cout << "position of find(1.0): " << (sv.find(1.0)-sv.begin()) << std::endl;
    std::cout << "position of lower_bound(1.0): " << (sv.lower_bound(1.0)-sv.begin()) << std::endl;
    std::cout << "position of upper_bound(1.0): " << (sv.upper_bound(1.0)-sv.begin()) << std::endl;

    return 0;
}
```
The output is:
```
0.8; 1; 1; 2; 3;
position of find(0.8): 0
position of find(1.0): 1
position of lower_bound(1.0): 1
position of upper_bound(1.0): 3

```

## other
This package is head-only, so you can also simply copy the *.hpp files in `src/chanzy/` you need into your project.

## Debug
For xmake project, you can add this in YOUR `xmake.lua` to let these containers check some invalid input in debug mode:
```lua
if is_mode("debug") then
    add_defines("CHANZY_DEBUG")
else
    add_undefines("CHANZY_DEBUG")
end
```
 
# Code
This package is powered by [xmake](https://xmake.io/), 
you can find the way to build this from the [official documents](https://xmake.io/#/getting_started).
 
# License
This software is licensed under the [MIT License](LICENSE).
