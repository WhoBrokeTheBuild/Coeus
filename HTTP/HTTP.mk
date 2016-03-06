.PHONY: HTTP clean-HTTP

include HTTP/Server/*.mk
include HTTP/Modules/*.mk

HTTP: HTTP-Server HTTP-Modules

clean-HTTP: clean-HTTP-Server clean-HTTP-Modules
