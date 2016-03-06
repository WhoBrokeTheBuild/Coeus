.PHONY: HTTP-Modules clean-HTTP-Modules

HTTP_MOD_OUT_DIR = HTTP/mods
HTTP_MOD_DIR = HTTP/Modules

HTTP_MOD_HELLO_OUT = $(HTTP_MOD_OUT_DIR)/mod_hello.so
HTTP_MOD_LUA_OUT   = $(HTTP_MOD_OUT_DIR)/mod_lua.so

HTTP-Modules: $(HTTP_MOD_HELLO_OUT) $(HTTP_MOD_LUA_OUT)

# Hello Module

HTTP_MOD_HELLO_DIR = $(HTTP_MOD_DIR)/hello
HTTP_MOD_HELLO_SRC = $(shell find $(HTTP_MOD_HELLO_DIR)/ -type f -name '*.cpp')
HTTP_MOD_HELLO_OBJ = $(addprefix $(OBJ_DIR)/, $(HTTP_MOD_HELLO_SRC:.cpp=.o))

$(OBJ_DIR)/$(HTTP_MOD_HELLO_DIR)/%.o: $(HTTP_MOD_HELLO_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) $(HTTP_SRV_CC_FLAGS) -fPIC -g -c -o $@ $<

$(HTTP_MOD_HELLO_OUT): $(HTTP_MOD_HELLO_OBJ)
	g++ -o $(HTTP_MOD_HELLO_OUT) $(LD_FLAGS) -shared $(HTTP_MOD_HELLO_OBJ)

# Lua Module

HTTP_MOD_LUA_DIR = $(HTTP_MOD_DIR)/lua
HTTP_MOD_LUA_SRC = $(shell find $(HTTP_MOD_LUA_DIR)/ -type f -name '*.cpp')
HTTP_MOD_LUA_OBJ = $(addprefix $(OBJ_DIR)/, $(HTTP_MOD_LUA_SRC:.cpp=.o))

$(OBJ_DIR)/$(HTTP_MOD_LUA_DIR)/%.o: $(HTTP_MOD_LUA_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CC_FLAGS) $(HTTP_SRV_CC_FLAGS) -fPIC -g -c -o $@ $<

$(HTTP_MOD_LUA_OUT): $(HTTP_MOD_LUA_OBJ)
	g++ -o $(HTTP_MOD_LUA_OUT) $(LD_FLAGS) -shared $(HTTP_MOD_LUA_OBJ)

clean-HTTP-Modules:
	rm -f \
		$(HTTP_MOD_HELLO_OUT) $(HTTP_MOD_HELLO_OBJ) \
		$(HTTP_MOD_LUA_OUT) $(HTTP_MOD_LUA_OBJ)
