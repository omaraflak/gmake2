# gmake

`gmake` is a simple way to generate multiple makefiles for your projects.

# Use

Create a **.gmake** file at the root of your project or use `gmake init` to generate a template.
Different variables are available :

* compiler
* flags
* libs
* output

Inside this file you should also write down the folders where submakefiles should be generated.

Once done, simply run `gmake`.

# Example

```
compiler = g++-8
flags = -std=c++17
libs = -lstdc++fs
gmake.cpp = gmake
output = bin
./root
```
