OBJS = TimeoutSerial.o
DEFS = -DBOOST_ALL_DYN_LINK
CXXFLAGS += -I/opt/local/include -I../include $(DEFS)

libtimeout.a: $(OBJS)
	@echo "*** making $@..."
	$(AR) rcv $@ $(OBJS)
	ranlib $@

.cpp.o:
	@echo "*** Compile $<..."
	$(CXX) -I.. $(CXXFLAGS) -c $< -o $@

