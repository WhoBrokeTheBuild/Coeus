.PHONY: Mail clean-Mail

MAIL_OUT = $(BUILD_DIR)/MailServer
MAIL_SRC = $(shell find Mail/ -type f -name '*.cpp')
MAIL_OBJ = $(addprefix $(OBJ_DIR)/, $(MAIL_SRC:.cpp=.o))

$(OBJ_DIR)/Mail/%.o: Mail/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) $(ASIO_CC_FLAGS) -g -c -o $@ $<

Mail: $(MAIL_OUT)

$(MAIL_OUT): $(MAIL_OBJ) $(COMMON_OUT)
	$(LD) -o $(MAIL_OUT) $(MAIL_OBJ) $(LD_FLAGS) $(ASIO_LD_FLAGS)

clean-Mail:
	rm -f $(MAIL_OUT) $(MAIL_OBJ)

run-Mail:
	cd Mail; ../$(MAIL_OUT) -f conf/main.cfg
