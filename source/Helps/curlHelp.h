#pragma once

#include <string>
#include <nn/socket.hpp>
#include "curl/curl.h"
#include <string.h>
#include <math.h>
#include <nn/ssl.h>

#include "dataStream.h"

#include "logger/Logger.hpp"

class DataDownloader {
    static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
    static int CreateSslCtx(CURL *curl, void *ssl_ctx, void *clientp);
    static int progressCallback(void* userdata, double dltotal, double dlnow, double ultotal, double ulnow);
private:
    CURL* curl;
	CURLcode res;
    void curlReset();
public:
    DataDownloader();
    ~DataDownloader();
    bool Download(DataStream& dataStream, const char* url);
};