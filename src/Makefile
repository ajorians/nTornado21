GCC = nspire-g++
LD = nspire-ld
GCCFLAGS = -Os -Wall -W -marm -Wno-unused-parameter -I include
LDFLAGS = -L lib -lndls -lCards -lTornado -lHighScore
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = nTornado21-sdl.tns
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
vpath %.tns $(DISTDIR)

all: prepare $(EXE)

prepare:
	mkdir include
	mkdir include/HighScoreLib
	mkdir include/CardLib
	mkdir include/TornadoLib
	mkdir lib
	cp -v HighScoreLib/include/HighScoreLib.h include/HighScoreLib
	cp -v HighScoreLib/libHighScore.a lib
	cp -v CardLib/include/CardLib.h include/CardLib
	cp -v CardLib/libCards.a lib
	cp -v TornadoLib/include/TornadoLib.h include/TornadoLib
	cp -v TornadoLib/libTornado.a lib

%.o: %.cpp
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $@

clean:
	rm -rf include
	rm -rf lib
	rm -f *.o *.elf
	rm -f $(EXE)
