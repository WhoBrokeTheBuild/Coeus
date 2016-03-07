.PHONY: CSL clean-CSL

CSL_OUT = $(BUILD_DIR)/libCSL.a
CSL_SRC = $(shell find CSL/ -type f -name '*.cpp')
CSL_OBJ = $(addprefix $(OBJ_DIR)/, $(CSL_SRC:.cpp=.o))

$(OBJ_DIR)/CSL/%.o: CSL/%.cpp CSL/%.hpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) -g -c -o $@ $<

CSL: $(CSL_OUT)

$(CSL_OUT): $(CSL_OBJ)
	$(AR) rvs $(CSL_OUT) $(CSL_OBJ)

clean-CSL:
	rm -f $(CSL_OUT) $(CSL_OBJ)
