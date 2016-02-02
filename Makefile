.PHONY: all clean rebuild \
	Common CSL FTP HTTP Mail SQL \
	RunFTP RunHTTP RunSQL RunMail

CC = g++
AR = ar
LD = g++

BUILD_DIR = Build
OBJ_DIR = $(BUILD_DIR)/obj

COMMON_OUT   = $(BUILD_DIR)/libCoeusCommon.a
CSL_OUT      = $(BUILD_DIR)/libCSL.a
FTP_SRV_OUT  = $(BUILD_DIR)/FTPServer
HTTP_SRV_OUT = $(BUILD_DIR)/HTTPServer
SQL_SRV_OUT  = $(BUILD_DIR)/SQLServer
MAIL_SRV_OUT = $(BUILD_DIR)/MailServer

DEFINES      = -D ASIO_STANDALONE
INCLUDE_DIRS = -I Common/ -I CSL/ -I include/
LIBRARY_DIRS = -L Build/ -L lib/
LIBRARIES    = -l CoeusCommon -l CSL -l pthread

CC_FLAGS = -std=c++11 $(INCLUDE_DIRS) $(DEFINES)
LD_FLAGS = $(LIBRARY_DIRS) $(LIBRARIES)

all: Common CSL FTP HTTP Mail SQL

clean:
	rm -rf $(OBJ_DIR) $(COMMON_OUT) $(CSL_OUT) \
	$(FTP_SRV_OUT) $(HTTP_SRV_OUT) $(SQL_SRV_OUT) $(MAIL_SRV_OUT)

rebuild: clean all

$(OBJ_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -g -c -o $@ $<

COMMON_SRC = $(shell find Common/ -type f -name '*.cpp')
COMMON_OBJ = $(addprefix $(OBJ_DIR)/, $(COMMON_SRC:.cpp=.o))

# Common Library

Common: $(COMMON_OBJ)
	$(AR) rvs $(COMMON_OUT) $(COMMON_OBJ)

# CSL Library

CSL_SRC = $(shell find CSL/ -type f -name '*.cpp')
CSL_OBJ = $(addprefix $(OBJ_DIR)/, $(CSL_SRC:.cpp=.o))

CSL: $(CSL_OBJ)
	$(AR) rvs $(CSL_OUT) $(CSL_OBJ)

# FTP Server

FTP_SRV_SRC = $(shell find FTP/ -type f -name '*.cpp')
FTP_SRV_OBJ = $(addprefix $(OBJ_DIR)/, $(FTP_SRV_SRC:.cpp=.o))

FTP: $(FTP_SRV_OBJ)
	$(LD) -o $(FTP_SRV_OUT) $(FTP_SRV_OBJ) $(LD_FLAGS)

run-FTP:
	cd FTP; ../Build/FTPServer

# HTTP Server

HTTP_SRV_SRC = $(shell find HTTP/ -type f -name '*.cpp')
HTTP_SRV_OBJ = $(addprefix $(OBJ_DIR)/, $(HTTP_SRV_SRC:.cpp=.o))

HTTP: $(HTTP_SRV_OBJ)
	$(LD) -o $(HTTP_SRV_OUT) $(HTTP_SRV_OBJ) $(LD_FLAGS)

run-HTTP:
	cd HTTP; ../Build/HTTPServer -f conf/main.cfg

# SQL Server

SQL_SRV_SRC = $(shell find SQL/ -type f -name '*.cpp')
SQL_SRV_OBJ = $(addprefix $(OBJ_DIR)/, $(SQL_SRV_SRC:.cpp=.o))

SQL: $(SQL_SRV_OBJ)
	$(LD) -o $(SQL_SRV_OUT) $(SQL_SRV_OBJ) $(LD_FLAGS)

run-SQL:
	cd SQL; ../Build/SQLServer

# Mail Server

MAIL_SRV_SRC = $(shell find Mail/ -type f -name '*.cpp')
MAIL_SRV_OBJ = $(addprefix $(OBJ_DIR)/, $(MAIL_SRV_SRC:.cpp=.o))

Mail: $(MAIL_SRV_OBJ)
	$(LD) -o $(MAIL_SRV_OUT) $(MAIL_SRV_OBJ) $(LD_FLAGS)

run-Mail:
	cd Mail; ../Build/MailServer
