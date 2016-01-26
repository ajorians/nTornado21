GCC = nspire-g++
LD = nspire-ld
GENZEHN = genzehn

GCCFLAGS = -Os -Wall -W -marm -Wno-unused-parameter -I include
LDFLAGS = -L lib -lndls -lCards -lTornado -lHighScore
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = nTornado21-sdl
DISTDIR = ..

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
vpath %.tns $(DISTDIR)
vpath %.elf $(DISTDIR)

all: prepare $(EXE).prg.tns

#Because JGit does not do symbolic links and TeamCity uses JGit
prepare:
	cd HighScoreLib; make; cd ..
	cd CardLib; make; cd ..
	cd TornadoLib; make; cd ..
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

$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

$(EXE).tns: $(EXE).elf
	$(GENZEHN) --input $(DISTDIR)/$^ --output $(DISTDIR)/$@ $(ZEHNFLAGS)

$(EXE).prg.tns: $(EXE).tns
	make-prg $(DISTDIR)/$^ $(DISTDIR)/$@

clean:
	cd HighScoreLib; make clean; cd ..
	cd CardLib; make clean; cd ..
	cd TornadoLib; make clean; cd ..
	rm -rf include
	rm -rf lib
	rm -f *.o *.elf
	rm -f $(DISTDIR)/$(EXE).tns
	rm -f $(DISTDIR)/$(EXE).elf
	rm -f $(DISTDIR)/$(EXE).prg.tns
