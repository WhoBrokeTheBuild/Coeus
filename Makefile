.PHONY: all clean rebuild

ifndef CXX
CXX = clang++
endif

AR = ar
LD = $(CXX)

BUILD_DIR = Build
OBJ_DIR = $(BUILD_DIR)/obj

CC_FLAGS = -std=c++11 -I Common/ -I include/
LD_FLAGS = -L Build/ -L lib/ -l CoeusCommon

ASIO_CC_FLAGS = -D ASIO_STANDALONE
ASIO_LD_FLAGS = -l pthread

CSL_CC_FLAGS = -I CSL/
CSL_LD_FLAGS = -l CSL

all: Common CSL FTP HTTP Mail

clean: clean-Common clean-CSL clean-FTP clean-HTTP clean-Mail

include Common/*.mk
include CSL/*.mk
include FTP/*.mk
include HTTP/*.mk
include Mail/*.mk
include Test/*.mk
include Docker.mk
