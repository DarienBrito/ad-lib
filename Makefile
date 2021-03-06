P ?= cosmos
profile ?= develop
platform ?= native
program := $(P)

files := misc
features := $(F)
defines :=
includes :=
link_flags :=

all_source :=
all_headers :=

################################################################################

ifeq ($(platform),native)
    ifeq ($(shell uname),Linux)
        platform := linux
    else ifeq ($(shell uname),Darwin)
        platform := darwin
    else ifeq ($(shell uname -o),Msys)
        platform := windows
    else
        $(error Could not determine native platform!)
    endif
endif

PLATFORM := $(shell echo $(platform) | tr a-z A-Z) 

################################################################################

ifeq ($(program),cosmos)

    files += \
             vecmat \
             SDL \
             glts \
             galaxy \
             util \
             bronze \
             old \
             core \
             state \
             socket \
             GL \
             hot \
             hot_player \
			 edge \

    features += SDLGL lua

    ifeq ($(filter hotremote,$(features)),)
        ifeq ($(platform),windows)
            features += hotlocal
        endif
        ifeq ($(platform),linux)
            features += hotlocal
        endif
    endif


else ifeq ($(program),nadal)

    files += socket hot hot_player hot_network
    features += hotremote

else ifeq ($(program),federer)

    files += socket hot hot_server hot_network
    ifeq ($(platform),windows)
        files += hot_network
        features += hotlocal
    endif

endif

################################################################################

files += $(program)
defines += $(PLATFORM)
includes += \
            math.h \
            float.h \
            stdio.h \
            errno.h \
            assert.h \
            stdlib.h \
            stdarg.h \
            assert.h \
            string.h \
            unistd.h \

link_flags += -lm

ifeq ($(platform),windows)
    exe_suffix := .exe
    includes += malloc.h
endif

ifneq ($(filter hotlocal,$(features)),)
    defines += HOTLOCAL
    files += watch
endif

ifneq ($(filter watch,$(files)),)
    ifeq ($(platform),windows)
        files += watchwindows
		defines += WIN32_LEAN_AND_MEAN
        includes += windows.h
    else ifeq ($(platform),linux)
        files += watchlinux
		includes += sys/inotify.h dirent.h
		defines += _BSD_SOURCE
    endif
endif

ifneq ($(filter hotremote,$(features)),)
    defines += HOTREMOTE
endif

ifneq ($(filter socket,$(files)),)
    ifeq ($(platform),windows)
        link_flags += -lwsock32 -mno-ms-bitfields
        includes += winsock.h
    else
        includes += \
                    sys/socket.h \
                    netinet/in.h \
                    arpa/inet.h \
                    sys/ioctl.h \

    endif
    ifeq ($(platform),linux)
        defines += _GNU_SOURCE # ip_mreq needs it
    endif
endif

ifeq ($(platform),windows)
    link_flags += -lmingw32
endif

ifneq ($(filter lua,$(features)),)
    includes += luajit-2.0/lauxlib.h luajit-2.0/lua.h luajit-2.0/lualib.h
    ifeq ($(platform),windows)
        link_flags += -llua51
        link_flags += -Wl,--export-all-symbols
    else
        link_flags += -lluajit-5.1
    endif
    ifeq ($(platform),darwin)
        link_flags += -pagezero_size 10000 -image_base 100000000
    endif
    ifeq ($(platform),linux)
        link_flags += -rdynamic
    endif
endif

ifneq ($(filter SDLGL,$(features)),)
    ifeq ($(platform),windows)
        link_flags += -lSDL2main
    endif
    # order is important here

    link_flags += -lSDL2
    link_flags += -lSDL2_image
    includes += SDL2/SDL.h SDL2/SDL_image.h

    ifeq ($(platform),ios)
        includes += OpenGLES/ES2/gl.h OpenGLES/ES2/glext.h
        defines += GLES
    else ifeq ($(platform),android)
        includes += GLES2/gl2.h
        defines += GLES
    else ifeq ($(platform),linux)
        includes += GL/glew.h
        defines += GLEW
        link_flags += -lGL -lGLEW
    else ifeq ($(platform),windows)
        includes += GL/glew.h
        defines += GLEW
        link_flags += -lopengl32 -lglew32
    else ifeq ($(platform),darwin)
        link_flags += -framework OpenGL
        includes += OpenGL/gl3.h
    endif
endif

ifeq ($(profile),develop)
    optimization := -g
else ifeq ($(profile),release)
    optimization := -Os -Werror
endif

all_source += $(shell find 2>/dev/null $(addprefix code/,$(addsuffix .c,$(files))))
all_headers += $(shell find 2>/dev/null $(addprefix code/,$(addsuffix .h,$(files))))

prefixed_defines := $(addprefix -D,$(defines))

base_dir := .build
platform_dir := $(base_dir)/$(platform)
output_dir := $(platform_dir)/output
package_dir := $(platform_dir)/package
config_dir := .config/$(platform)
config_include_dir := $(config_dir)/include
config_lib_dir := $(config_dir)/lib

api := API.h
superheader := $(output_dir)/super$(program).h
main := $(output_dir)/main$(program).c
exe := $(output_dir)/$(program)$(exe_suffix)
package_archive := $(platform_dir)/package.tar.bz2

include_flags := $(addprefix -include ,$(includes) $(all_headers) $(superheader))

cc := gcc
cflags := -std=c99 \
    -Wall \
    -Wextra \
    -Wno-missing-field-initializers \
    -Wno-missing-braces \
    -I$(config_include_dir) \
    -L$(config_lib_dir) \
    $(include_flags) \
    $(optimization) \
    $(link_flags) \
    $(prefixed_defines) \

################################################################################

.PHONY: all package clean run debug prepare

all: $(exe) $(api)

package: $(package_archive)

clean:
	rm -rf $(platform_dir)

run: $(exe) $(api)
	@./$(exe)

debug: $(exe) $(api)
	@gdb $(exe)

prepare: $(source_c) $(source_h) $(source_ext_h)

################################################################################

SHELL := bash
export

$(exe): $(all_headers) $(all_source) $(superheader) $(main) | $(output_dir)
	@echo "Making the executable..."
	$(cc) -o $(exe) $(all_source) $(main) $(cflags)

$(superheader): $(all_source) | $(output_dir)
	@echo "Making superheader..."
	@makeheaders -h $(all_source) > $(superheader)

$(api): $(superheader) code/*.h code/*.c
ifeq ($(program),cosmos)
	@echo "Making API.h..."
	@rm -f API.h
	@makeheaders -h code/vecmat.h code/vecmat.c | grep -v '#' >> API.h
	@makeheaders -h code/util.h code/util.c | grep -v '#' >> API.h
	@makeheaders -h code/glts.h code/glts.c | grep -v '#' >> API.h
	@makeheaders -h code/bronze.h | grep -v '#' >> API.h
	@makeheaders -h code/GL.h code/GL.c | grep -v '#' >> API.h
	@makeheaders -h code/SDL.h code/hot.h | grep -v '#' >> API.h
	@cat $(superheader) | grep API | grep -v '#' | sed 's/API //' >> API.h
endif

$(main): | $(output_dir)
	@echo "Making mainfile..."
	@echo "int main(int c,char*v[]) {return $(program)(c,v);}" > $(main)

$(package_archive): $(exe) | $(package_dir)
	@echo "Making '$(package_archive)'..."
	@cp -r $(exe) data mode art $(package_dir)
	@tar -cj $(package_dir) -f $(package_archive)

$(output_dir): | $(platform_dir)
	mkdir $(output_dir)

$(package_dir): | $(platform_dir)
	mkdir $(package_dir)

$(platform_dir): | $(base_dir)
	mkdir $(platform_dir)

$(base_dir):
	mkdir $(base_dir)
