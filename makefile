CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude -Wall
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

SRCDIR = src
OBJDIR = build
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET = $(BINDIR)/snakevsblock

all: dirs $(TARGET)

dirs:
	mkdir -p $(OBJDIR) $(BINDIR)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean dirs
