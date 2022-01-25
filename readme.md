# Guardian Bot

### What for?

Guardian Bot is a research project aimed to
investigate computer vision integration into robotic and
semi-automated systems. Hope you will enjoy looking
through this project or find something new or useful!

Feedback is very appreciated.

### All in all

This project uses OpenCV Net module (especially with
Caffee dnn framework) and has Serial port communication
support. It provides UI which shows the output of
neural network facial recognition and also provides
serial port communication interface.

### Development notes

For a couple of months further my aim is to rewrite this
project to make it able to use different cameras, not only
RealSense devices (simply because I'm currently have no
access to device I used to have).

Also, current version of program has some weird behaviour
making program exit unexpectedly for no reason. If you have
ideas and/or experience on solving this kind of issues, please
contact me or create a PR.

### Build and run

Finally, support for simple cameras delivered so feel free to
experiment and if you find any issues using specific camera,
please contact me or create a PR.

#### Build

Then, the build itself:
- First of all, you should have Conan package manager installed on your machine so the project's
dependencies can be built. If you doesn't have it, visit [conan's official website](conan.io) and
install it.

- If you have conan installed on your computer, then run the following commands:
```bash
$ mkdir build
$ cd build
$ conan install .. --build=missing
```

P.S. For now it is highly recommended to use Visual Studio 16 toolchain because I did not experiment
with other toolchains and currently I'm really caring about Windows and Linux support so I'll definetely
test current build system with gcc toolchain (but not right now).

- After all dependencies were built and installed, then run standard CMake configuration sequence:
run `cmake ..` under build directory and `cmake --build .` after configuration is complete.

#### Run

To run the application you have to go to directory
containing fresh-baked executable and type the
following command:

```bash
./GuardianBotApp -p <path_to_prototxt> -m <path_to_caffee_file>
```

- The `-p` or `--prototxt` command line argument is
used to provide path to .prototxt file for dnn
model.
- The `-m` or `--model` command line argument is
used to provide path to Caffee model file itself.

Both files are placed in the repository's root
directory and you can use them as a default configuration.
Of course, you can use your own but consequences are unknown to me, it's your field for researches.:)

Congratulations! You've successfully started my
little application, feel free to explore and upgrade
it.

##### Important note!!!

Program uses GLSL shaders placed under `resources/` directory in the project's root
directory and also being installed under runtime output directory while build into
folder with the same name.
**Make sure** you're running application from project's root directory or
runtime output directory so the program can access shaders code!

### Contributions

Now there is only one contributor (it's me, of course)
but if you have an idea how to improve this project or
to make it a little more serious than it is - feel free
to contact me or to create your pull requests, any
help is appreciated.
