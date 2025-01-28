#include "curlHelp.h"

int LastProgress = 0;
sead::Heap* mHeap;

DataDownloader::DataDownloader()
{
    mHeap = sead::HeapMgr::instance()->getCurrentHeap();
    curl = curl_easy_init();
	res = curl_global_init(CURL_GLOBAL_DEFAULT);
}

int DataDownloader::CreateSslCtx(CURL* curl, void* ssl_ctx, void* clientp) {
    auto *ctx = (nn::ssl::Context*)ssl_ctx;
    if(ctx->Create(nn::ssl::Context::SslVersion::UNK2).isSuccess()) {
        return CURLE_OK;
    }
    return -1;
}

int DataDownloader::progressCallback(void* userdata, double dltotal, double dlnow, double ultotal, double ulnow){
    //Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));
    int progress = (int)(dlnow/dltotal*100);
    if (dltotal != 0 && LastProgress != progress)
    {
        Logger::log("progress: %d%\n", progress);
        LastProgress = progress;
    }

    return 0;
}

bool DataDownloader::Download(DataStream& dataStream, const char* url)
{

	if(res != CURLcode::CURLE_OK) {
        Logger::log("init returned non ok! Val: %d\n", curl_easy_strerror(res));
        curlReset();
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_ALL);
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DataDownloader::writeMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) &dataStream);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, DataDownloader::CreateSslCtx);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DataDownloader::progressCallback);
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 2000000L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
     
	res = curl_easy_perform(curl);

	if(res != CURLE_OK)
    {
        Logger::log("CurlError : %s\n", curl_easy_strerror(res));
        curlReset();
	    return false;
    }
    Logger::log("Size : %i\n", dataStream.getSize());
    
	curlReset();
	return true;
}

DataDownloader::~DataDownloader()
{
    Logger::log("Cleared !\n");
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void DataDownloader::curlReset()
{
    Logger::log("Reseted !\n");
    curl_easy_reset(curl);
}

size_t DataDownloader::writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    Logger::log("Write Memory Callback !\n");
	auto *curlData = (DataStream*)userp;
    size_t dataSize = size * nmemb;
    return curlData->write(contents, dataSize);
}