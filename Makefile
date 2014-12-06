CC      := clang
CXX     := clang++
LD      := clang++

INC     := -Isrc
CFLAGS  := -pedantic -std=c++11 -Wall -g
LDFLAGS := -g
LIBS    := -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
OUT     := maze

MODULES := 
SRC_DIR := src $(addprefix src/,$(MODULES))
OBJ_DIR := obj $(addprefix obj/,$(MODULES))
SRC     := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ     := $(patsubst src/%.cpp,obj/%.o,$(SRC))

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CXX) $(CFLAGS) $(INC) -MMD -c $$< -o $$@
endef

all: checkdirs maze

clean: 
	rm -f $(OUT)
	rm -rf obj/

maze: $(OBJ)
	$(LD) $^ -o $(OUT) $(LDFLAGS) $(LIBS)

checkdirs: $(OBJ_DIR)

$(OBJ_DIR):
	@mkdir -p $@

$(foreach bdir,$(OBJ_DIR),$(eval $(call make-goal,$(bdir))))

.PHONY: all checkdirs clean

-include $(OBJ:%.o=%.d)
