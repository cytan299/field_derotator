include Makefile.FLTK

EXENAME = derot
APPNAME = $(EXENAME).app
OBJS = main.o TCPClient.o SerialClient.o DeRotatorUI.o \
	DeRotatorGraphics.o DeRotatorConfig.o\
	MessageSink.o DeRotatorCMD.o
DEFS = -DBOOST_ALL_DYN_LINK
CXXFLAGS += -I./include -I/opt/local/include $(DEFS)
LINKFLTK_ALL += -L./lib -L/opt/local/lib -ltimeout -lboost_system-mt \
	-lboost_filesystem-mt -lboost_log-mt \
	-lboost_thread-mt -lboost_date_time-mt \
	-lboost_program_options-mt

DYLIBBUNDLER = ../macdylibbundler-master/dylibbundler

all:	version.hpp libtimeout.a $(EXENAME) $(APPNAME)

$(EXENAME): $(OBJS)
	@echo "*** Linking $@..."
	$(CXX) $(DEFS) $(OBJS)  $(LINKFLTK_ALL) -o $@

libtimeout.a: ./lib/TimeoutSerial.o
	(cd lib; make)

version.hpp: 
	$(shell echo "#ifndef PROJECT_VERSION" > version.hpp)
	$(shell echo "#define PROJECT_VERSION \""$(PROJECT_VERSION)-$(REVISION_COUNT)"\"" >> version.hpp)
	$(shell echo "#endif" >> version.hpp)

$(APPNAME): $(EXENAME).icns
	@echo "*** making $@ ..."
	@if [ ! -d "$@" ]; then \
           mkdir $@; \
           mkdir $@/Contents; \
           mkdir $@/Contents/Resources; \
	   mkdir $@/Contents/MacOS;\
        fi
	@echo APPLnone > $@/Contents/PkgInfo
	@cp $(EXENAME) $@/Contents/MacOS
	@chmod 755 $@/Contents/MacOS/$(EXENAME)
	@sed 's/PROJECT_VERSION/$(PROJECT_VERSION)/g' macosx/info.plist |\
		sed 's/REVISION_COUNT/$(REVISION_COUNT)/g' > $@/Contents/info.plist
	@cp macosx/$(EXENAME).icns $@/Contents/Resources
	@(cp -f *.png $@/Contents/Resources)
	@(cp -rf help $@/Contents/Resources)
	@$(DYLIBBUNDLER) -od -b -x ./derot.app/Contents/MacOS/derot -d ./derot.app/Contents/libs -p @executable_path/../libs/ > /dev/null

$(EXENAME).icns: macosx/derot_icon.png
	@echo "*** making icons ..."
	@rm -rf macosx/$(EXENAME).iconset
	@mkdir macosx/$(EXENAME).iconset
	@sips -z 16 16 $< --out macosx/$(EXENAME).iconset/icon_16x16.png > /dev/null
	@sips -z 32 32 $< --out macosx/$(EXENAME).iconset/icon_16x16@2x.png > /dev/null
	@sips -z 32 32 $< --out macosx/$(EXENAME).iconset/icon_32x32.png > /dev/null
	@sips -z 64 64 $< --out macosx/$(EXENAME).iconset/icon_32x32@2x.png > /dev/null
	@sips -z 128 128 $< --out macosx/$(EXENAME).iconset/icon_128x128.png > /dev/null
	@sips -z 256 256 $< --out macosx/$(EXENAME).iconset/icon_128x128@2x.png > /dev/null
	@sips -z 256 256 $< --out macosx/$(EXENAME).iconset/icon_256x256.png > /dev/null
	@sips -z 512 512 $< --out macosx/$(EXENAME).iconset/icon_256x256@2x.png > /dev/null
	@sips -z 512 512 $< --out macosx/$(EXENAME).iconset/icon_512x512.png > /dev/null
	@cp $< macosx/$(EXENAME).iconset/icon_512x512@2x.png
	@iconutil -c icns -o macosx/$(EXENAME).icns macosx/$(EXENAME).iconset
	@rm -r macosx/$(EXENAME).iconset



