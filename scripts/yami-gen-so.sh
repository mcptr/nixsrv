cc -shared -o libyami4cored.so -Wl,--whole-archive libyamicore-fPIC.a -Wl,--no-whole-archive
cc -shared -o libyami4cppd.so -Wl,--whole-archive libyamicpp-fPIC.a -Wl,--no-whole-archive
