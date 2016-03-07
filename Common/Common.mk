.PHONY: Common clean-Common

COMMON_OUT = $(BUILD_DIR)/libCoeusCommon.a
COMMON_SRC = $(shell find Common/ -type f -name '*.cpp')
COMMON_OBJ = $(addprefix $(OBJ_DIR)/, $(COMMON_SRC:.cpp=.o))

$(OBJ_DIR)/Common/%.o: Common/%.cpp Common/%.hpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) -g -c -o $@ $<

Common: $(COMMON_OUT)

$(COMMON_OUT): $(COMMON_OBJ)
	$(AR) rvs $(COMMON_OUT) $(COMMON_OBJ)

clean-Common:
	rm -f $(COMMON_OUT) $(COMMON_OBJ)
