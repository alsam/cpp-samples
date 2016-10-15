# HOWTO install libcurlpp
# yaourt -S aur/libcurlpp

CXXFLAGS += -g -pthread
LDFLAGS += -g -lcurlpp -lcurl -lpugixml -ltidy
all: curlpp_download_example curlpp_auth_example example04
curlpp_download_example : curlpp_download_example.cpp
curlpp_auth_example : curlpp_auth_example.cpp
example04 : example04.cpp
clean:
	rm curlpp_download_example
