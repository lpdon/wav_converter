# wav_converter

###About

Simple multi-threaded software to convert WAV files into MP3. The conversion uses reasonable default settings and the converted files are output in the same dir.

###Usage
```converter dirwithwavfiles```

###Install instructions

The program uses Google Test Framework for Unit Tests. Makefile generation is done with cmake.

To init the submodules after cloning the repo:

```git submodule init```

```git submodule update --recursive```

Last version of the LAME lib is included. To build it is necessary to install ncurses.

```sudo apt-get install libncurses5-dev cmake```

###Tests

In the test folder there are some tests for basic functionality. To run them after building enter:

```make test```

***

For info and suggestions, please contact me