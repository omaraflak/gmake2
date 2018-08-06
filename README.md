# gmake

`gmake` is a simple way to generate multiple makefiles for your projects.

# Use

Create a **.gmake** file at the root of your project or use `gmake init` to generate a template.
Different variables are available :

* compiler
* flags
* executable
* output

Inside this file you should also write down the folders where submakefiles should be generated.

Once done, simply run `gmake`.

# Example

```
compiler = g++-8
flags = -std=c++17 -lstdc++fs
executable = gmake
output = bin

./root
```
