LOGGER_IP ?= "127.0.0.1" # put log server IP in string
.PHONY: all clean

all:
	cmake --toolchain=cmake/toolchain.cmake -DLOGGER_IP=$(LOGGER_IP) -S . -B build && $(MAKE) -C build subsdk9_meta

clean:
	rm -r build || true

log: all
	python3.8 misc/scripts/tcpServer.py 0.0.0.0