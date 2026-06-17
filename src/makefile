TARGET := ../iskelt-corax-v0.3.0.exe

CXX := clang++

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

CXXFLAGS := -std=c++20 \
            -O3 \
			-march=native \
			-mtune=native \
			--target=x86_64-pc-windows-msvc

LDFLAGS := --target=x86_64-pc-windows-msvc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)