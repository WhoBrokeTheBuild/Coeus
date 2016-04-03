.PHONY: all clean rebuild test

LD = $(CXX)

OUT_DIR = Build
OBJ_DIR = $(OUT_DIR)/obj
OBJ_DIR_TEST = $(OBJ_DIR)/test

CPPFLAGS += -std=c++11 -I include/
LDFLAGS  += -L Build/ -L lib/
LDLIBS   +=

COMMON_CPPFLAGS = -I Common/src/
COMMON_LDFLAGS  =
COMMON_LDLIBS   = -l CoeusCommon

ASIO_CPPFLAGS = -D ASIO_STANDALONE
ASIO_LDFLAGS  =
ASIO_LDLIBS   = -l pthread

CSL_CPPFLAGS = -I CSL/src/
CSL_LDFLAGS  =
CSL_LDLIBS   = -l CSL

TEST_CPPFLAGS = -D COEUS_TESTING -I thirdparty/googletest/include
TEST_LDFLAGS  = -L thirdparty/googletest
TEST_LDLIBS   = -l gtest -l pthread

$(OUT_DIR)/%.a:
	$(AR) rvs $@ $^

$(OUT_DIR)/%.so:
	mkdir -p $(dir $@)
	$(LD) -o $@ $(LDFLAGS) -shared $(HTTP_MOD_LUA_OBJ) $(LDLIBS)

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -g -c -o $@ $<

$(OBJ_DIR_TEST)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -g -c -o $@ $<

all: Common CSL FTP HTTP Mail

clean: clean-Common clean-CSL clean-FTP clean-HTTP clean-Mail

test: gtest test-Common test-FTP

gtest:
	cd thirdparty/googletest; cmake .; make

include Templates.mk
include Docker.mk
include Common/*.mk
include CSL/*.mk
include FTP/*.mk
include HTTP/*.mk
include Mail/*.mk
