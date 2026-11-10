CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

TARGET = appendix/emulator.exe
OBJDIR = appendix

OBJ = $(OBJDIR)/main.o \
      $(OBJDIR)/CPU.o \
      $(OBJDIR)/mem.o \
      $(OBJDIR)/parser.o

all: $(TARGET)

$(OBJDIR):
	if not exist $(OBJDIR) mkdir $(OBJDIR)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

$(OBJDIR)/main.o: main.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/CPU.o: src/CPU.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/mem.o: src/mem.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/parser.o: src/parser.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)

.PHONY: all clean
