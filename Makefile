# WAV Player - MinGW Makefile
# 使い方: mingw32-make  または  make

TARGET  = WavPlayer.exe
SRC     = src/main.cpp
CXXFLAGS = -std=c++03 -Wall -mwindows
LDFLAGS  = -lwinmm -lcomdlg32

$(TARGET): $(SRC)
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean
