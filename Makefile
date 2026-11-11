CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -MMD -MP -Iinclude

TARGET := appendix/emulator.exe
OBJDIR := appendix
SRCDIR := src

VPATH := $(SRCDIR)

SRC := $(wildcard $(SRCDIR)/*.cpp) main.cpp
OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC:.cpp=.o)))
DEPS := $(OBJ:.o=.d)

all: $(TARGET)

$(OBJDIR):
	@if not exist $(OBJDIR) mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

-include $(DEPS)

clean:
	if exist $(OBJDIR) rmdir /S /Q $(OBJDIR)

.PHONY: all clean
