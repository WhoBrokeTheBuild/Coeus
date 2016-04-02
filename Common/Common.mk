
$(eval $(call LibraryTemplate,Common,libCoeusCommon.a, \
 	Common/src, \
	$(ASIO_CPPFLAGS), \
	$(ASIO_LDFLAGS), \
	$(ASIO_LDLIBS) ) )
