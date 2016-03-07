.PHONY: FTP clean-FTP

FTP_OUT = $(BUILD_DIR)/FTPServer
FTP_SRC = $(shell find FTP/ -type f -name '*.cpp')
FTP_OBJ = $(addprefix $(OBJ_DIR)/, $(FTP_SRC:.cpp=.o))

$(OBJ_DIR)/FTP/%.o: FTP/%.cpp FTP/%.hpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) $(ASIO_CC_FLAGS) -g -c -o $@ $<

FTP: $(FTP_OUT)

$(FTP_OUT): $(FTP_OBJ) $(COMMON_OUT)
	$(LD) -o $(FTP_OUT) $(FTP_OBJ) $(LD_FLAGS) $(ASIO_LD_FLAGS)

clean-FTP:
	rm -f $(FTP_OUT) $(FTP_OBJ)

run-FTP:
	cd FTP; ../$(FTP_OUT) -f conf/main.cfg
