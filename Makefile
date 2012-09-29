PLATFORM =
EXE =
ifeq ($(shell uname),Linux)
	PLATFORM = LINUX
	LINK_FLAGS = -lGL -lGLEW
endif
ifeq ($(shell uname),Darwin)
	PLATFORM = MAC
	LINK_FLAGS = -framework OpenGL
endif
ifeq ($(shell uname -o),Msys)
	PLATFORM = MINGW
	LINK_FLAGS = -lopengl32 -lglew32 -lmingw32 -lSDL2main
	EXE = .exe
endif
ifeq ($(PLATFORM),)
	$(error Could not determine platform.)
endif

MAIN = Main$(EXE)

.PHONY: all clean
all: $(MAIN)

$(MAIN):
	gcc \
		-std=gnu99 \
		src/*.c \
		-D$(PLATFORM) \
		$(LINK_FLAGS) \
		-lSDL2 \
		-lm \
		-g \
		-o $(MAIN)

clean:
	rm $(MAIN)