# Guardian Bot

### What for?

Guardian Bot is a research project which is aimed to
research computer vision integration into robotic and
semi-automated systems. Hope you will enjoy looking
through this project or find something new or useful!

Feedback is very appreciated.

### All in all

This project uses OpenCV Net module (especially with
Caffee dnn framework) and has Serial port communication
support. It provides UI which shows the output of
neural network facial recognition and also provides
serial port communication interface.

### Build and run

First of all, current version of the application works
only with Intel RealSense camera. There will be
support for another cameras in future versions.

#### Build

Then, the build itself:
- First, do standard CMake setup:
```bash
$ mkdir build && cd build
$ cmake .. -D GB_SHARED_GL:BOOL=<TRUE|FALSE>
```
The `GB_SHARED_GL` determines whether OpenGL stuff
should be built as static or dynamic libraries.

- After CMake finishes build files generation, you
will see a warning message:

```
Whether librealsense2 or opencv built copies
were not found.
Make sure you've built it using
librealsense2-external and opencv-external targets
of the projecft first.
```

This message appears on the first build and it means
thst you have no local copy of librealsense SDK or
suitable OpenCV version installed. You have to type
2 commands after that:

```bash
$ cmake --build . -t librealsense2-external
$ cmake --build . -t opencv-external
```

- After this two targets are built, you can build
the main application:
```bash
$cmake --build . -t GuardianBotApp
```

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
directory. Of course, you can use your own but
consequences are unknown to me, it's your field for
researches.:)

Congratulations! You've successfully started my
little application, feel free to explore and upgrade
it.

### Contributions

Now there is only one contributor (it's me, of course)
but if you have an idea how to improve this project or
to make it a little more serious than it is - feel free
to contact me or to create your pull requests, any
help is appreciated.
