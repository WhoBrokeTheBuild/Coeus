
$(eval $(call ExecutableTemplate,FTP,CoeusFTP, \
	FTP/src, \
	$(ASIO_CPPFLAGS) $(COMMON_CPPFLAGS), \
	$(ASIO_LDFLAGS) $(COMMON_LDFLAGS), \
	$(ASIO_LDLIBS) $(COMMON_LDLIBS), \
	Common, \
	-f conf/main.cfg ) )
