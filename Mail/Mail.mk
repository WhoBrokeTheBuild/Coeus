
$(eval $(call ExecutableTemplate,Mail,CoeusMail, \
	Mail/src, \
	$(ASIO_CPPFLAGS) $(COMMON_CPPFLAGS), \
	$(ASIO_LDFLAGS) $(COMMON_LDFLAGS), \
	$(ASIO_LDLIBS) $(COMMON_LDLIBS), \
	Common, \
	-f conf/main.cfg ) )
