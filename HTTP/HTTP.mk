
HTTP_MOD_CPPFLAGS = $(ASIO_CPPFLAGS) $(CSL_CPPFLAGS) $(COMMON_CPPFLAGS) -fPIC -I HTTP/src
HTTP_MOD_LDFLAGS  = $(ASIO_LDFLAGS) $(CSL_LDFLAGS) $(COMMON_LDFLAGS)
HTTP_MOD_LDLIBS   = $(ASIO_LDLIBS) $(CSL_LDLIBS) $(COMMON_LDLIBS)

$(eval $(call ExecutableTemplate,HTTP,CoeusHTTP, \
 	HTTP/src, \
	$(ASIO_CPPFLAGS) $(CSL_CPPFLAGS) $(COMMON_CPPFLAGS), \
	$(ASIO_LDFLAGS) $(CSL_LDFLAGS) $(COMMON_LDFLAGS), \
	$(ASIO_LDLIBS) $(CSL_LDLIBS) $(COMMON_LDLIBS) -ldl, \
	CSL Common, \
	-f conf/main.cfg ) )

HTTP-Modules: HTTP-mod_hello HTTP-mod_lua HTTP-mod_header
	mkdir -p HTTP/mods
	cp -f $(OUT_DIR)/HTTP/*.so HTTP/mods/

LUA_CPPFLAGS = $(shell pkg-config --silence-errors --cflags lua) $(shell pkg-config --silence-errors --cflags lua5.2)
LUA_LDFLAGS  = $(shell pkg-config --silence-errors --libs-only-L lua) $(shell pkg-config --silence-errors --libs-only-L lua5.2)
LUA_LDLIBS   = $(shell pkg-config --silence-errors --libs-only-l lua) $(shell pkg-config --silence-errors --libs-only-l lua5.2)

$(eval $(call LibraryTemplate,HTTP-mod_hello,HTTP/mod_hello.so, \
 	HTTP/Modules/hello, \
	$(HTTP_MOD_CPPFLAGS) $(LUA_CPPFLAGS), \
	$(HTTP_MOD_LDFLAGS) $(LUA_LDFLAGS), \
	$(HTTP_MOD_LDLIBS) $(LUA_LDLIBS), \
	CSL Common ) )

$(eval $(call LibraryTemplate,HTTP-mod_lua,HTTP/mod_lua.so, \
 	HTTP/Modules/lua, \
	$(HTTP_MOD_CPPFLAGS), \
	$(HTTP_MOD_LDFLAGS), \
	$(HTTP_MOD_LDLIBS), \
	CSL Common ) )

$(eval $(call LibraryTemplate,HTTP-mod_header,HTTP/mod_header.so, \
 	HTTP/Modules/header, \
	$(HTTP_MOD_CPPFLAGS), \
	$(HTTP_MOD_LDFLAGS), \
	$(HTTP_MOD_LDLIBS), \
	CSL Common ) )
