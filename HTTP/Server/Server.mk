.PHONY: HTTP clean-HTTP

HTTP_SRV_DIR = HTTP/Server
HTTP_SRV_OUT = $(BUILD_DIR)/HTTPServer
HTTP_SRV_SRC = $(shell find $(HTTP_SRV_DIR)/ -type f -name '*.cpp')
HTTP_SRV_OBJ = $(addprefix $(OBJ_DIR)/, $(HTTP_SRV_SRC:.cpp=.o))

HTTP_SRV_CC_FLAGS = -I HTTP/Server/ $(ASIO_CC_FLAGS)

$(OBJ_DIR)/$(HTTP_SRV_DIR)/%.o: $(HTTP_SRV_DIR)/%.cpp $(HTTP_SRV_DIR)/%.hpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) $(HTTP_LNK_CC_FLAGS) $(CSL_CC_FLAGS) $(ASIO_CC_FLAGS) -g -c -o $@ $<

HTTP-Server: $(HTTP_SRV_OUT)

$(HTTP_SRV_OUT): $(HTTP_SRV_OBJ) $(COMMON_OUT)
	$(LD) -o $(HTTP_SRV_OUT) $(HTTP_SRV_OBJ) \
		-Wl,--export-dynamic \
		$(LD_FLAGS) $(HTTP_LNK_LD_FLAGS) $(CSL_LD_FLAGS) $(ASIO_LD_FLAGS) -l dl

clean-HTTP-Server:
	rm -f $(HTTP_SRV_OUT) $(HTTP_SRV_OBJ)

run-HTTP:
	cd HTTP; ../$(HTTP_SRV_OUT) -f conf/main.cfg
