CC	?= gcc
CFLAGS	+= -pipe
ifeq (${BUILD_TYPE},production)
CFLAGS  += -O3
else
CFLAGS  += -O0 -g3 -DDIGGER_DEBUG
endif
RCFLAGS = -D_SDL -std=c11 -Wall -DNO_SND_FILTER #-DNO_SND_EFFECTS
OBJS	= main.o digger.o drawing.o sprite.o scores.o record.o sound.o \
		newsnd.o ini.o input.o monster.o bags.o alpha.o vgagrafx.o \
		title_gz.o icon.o sdl_kbd.o sdl_vid.o sdl_timer.o sdl_snd.o \
		digger_math.o monster_obj.o digger_obj.o bullet_obj.o \
		cgagrafx.o keyboard.o soundgen.o spinlock.o game.o

#ARCH	?= LINUX
ARCH	?= MINGW
#ARCH	?= FREEBSD
#ARCH	?= FooOS
SDL_VER ?= 2.0.9
ZLIB_VER ?= 1.2.11
ifneq ($(ARCH),MINGW)
MGW_PREF ?= i686-w64-mingw32
MINGW_DEPS_ROOT ?= ../
MGW64_PREF ?= x86_64-w64-mingw32
endif

ifdef CI_COVERAGE
CFLAGS += --coverage
LIBS += --coverage
endif

ifeq ($(ARCH),MINGW)
CC	=  ${MGW_PREF}-gcc
WINDRES	?=  windres
STRIP   ?= ${MGW_PREF}-strip
RCFLAGS	+= -DMINGW $(shell pkg-config sdl2 --static --cflags) -I${MINGW_DEPS_ROOT}/zlib-${ZLIB_VER}
LIBS	+= $(shell pkg-config sdl2 --static --libs) -lmingw32 -luser32 -lgdi32 -lwinmm -L${MINGW_DEPS_ROOT}/zlib-${ZLIB_VER} -lz -lm -static
ifeq (${BUILD_TYPE},debug)
LIBS    += -mconsole
endif
ESUFFIX	=  .exe
OBJS	+=  digger.res
VPATH   += ./pkg/windows
endif

ifeq ($(ARCH),FREEBSD)
OBJS	+= fbsd_sup.o	# strup()
RCFLAGS	+= -DFREEBSD $(shell pkg-config sdl2 --cflags)
LIBS	+= $(shell pkg-config sdl2 --libs) -lz -lm -lX11
ESUFFIX	=
endif

ifeq ($(ARCH),LINUX)
OBJS	+= fbsd_sup.o	# strup()
RCFLAGS	+= -DLINUX $(shell pkg-config sdl2 --cflags)
LIBS	+= $(shell pkg-config sdl2 --libs) -lz -lm -lX11
ESUFFIX	=
endif

ifeq ($(ARCH),FooOS)
OBJS	+=		# insert here the names of the files which contains various missing functions like strup() on Linux and FreeBSD
RCFLAGS	+= -DFooOS	# insert here additional compiler flags which required to find include files, trigger os-specific compiler behaviour etc.
LIBS	+= 		# insert here libs required to compile like zlib, SDL etc
ESUFFIX	=		# insert here suffix of the executable on your platform if any (like ".exe" on Win32)
endif

STRIP   ?= strip

all: digger$(ESUFFIX)

digger$(ESUFFIX): $(OBJS)
	$(CC) -o digger$(ESUFFIX) $(OBJS) $(LIBS)
ifeq (${BUILD_TYPE},production)
	$(STRIP) --strip-unneeded digger$(ESUFFIX)
endif

%.o : %.c
	$(CC) -c $(RCFLAGS) $(CFLAGS) $< -o $@

%.res : %.rc
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f $(OBJS) digger$(ESUFFIX) *.gcov *.gcda *.gcno

ifdef TEST_TYPE
TT_VAR=	TEST_TYPES=$(TEST_TYPE)
else
TT_VAR=
endif

do-test:
	sh -x ./scripts/do-test-cmmn.sh
	SDL_VER=${SDL_VER} ZLIB_VER=${ZLIB_VER} MGW_PREF="${MGW_PREF}" \
	  MGW64_PREF="${MGW64_PREF}" sh -x ./scripts/do-test.sh
	env ${TT_VAR} sh -x ./scripts/do-test-run.sh

do-test-cmake:
	sh -x ./scripts/do-test-cmmn.sh
	sh -x ./scripts/do-test-cmake.sh
	env ${TT_VAR} sh -x ./scripts/do-test-run.sh

coverage-report:
	for s in $(OBJS); \
	do \
	  gcov "$${s%.o}.c"; \
	done

coverage-report-html:
	rm -rf ./digger_lcov
	lcov --directory . --capture --output-file digger_lcov/digger.info
	genhtml -o digger_lcov digger_lcov/digger.info
