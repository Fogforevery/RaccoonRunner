CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
CPPFLAGS = -I./src -MMD -MP
LDFLAGS = 
LDLIBS = -lraylib

SRC = \
	src/app/app.cc \
	src/app/home_screen.cc \
	src/game/game.cc \
	src/game/player.cc \
	src/game/world.cc \
	src/main.cc \
	src/maths/camera.cc \
	src/maths/mat4.cc \
	src/maths/maths.cc \
	src/maths/transform.cc \
	src/maths/vec2.cc \
	src/maths/vec3.cc \
	src/rasterizer/mesh.cc \
	src/rasterizer/obj_loader.cc \
	src/rasterizer/rasterizer2d.cc \
	src/rasterizer/render_target.cc \
	src/rasterizer/scene.cc \
	src/rasterizer/scene_object.cc \
	src/rasterizer/shader.cc \
	src/rasterizer/texture.cc \

OBJ = $(SRC:.cc=.o)
DEP = $(OBJ:.o=.d)

TARGET = test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEP)

clean:
	$(RM) $(TARGET) $(OBJ) $(DEP)
