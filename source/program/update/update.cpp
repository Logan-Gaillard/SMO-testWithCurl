#include "update.h"

#include "Helps/curlHelp.h"
#include "Helps/fsHelp.h"

#include "logger/Logger.hpp"
#include <string>

#include "heap/seadHeapMgr.h"
#include <stdlib.h>

SEAD_SINGLETON_DISPOSER_IMPL(Update)
Update::Update() = default;
Update::~Update() = default;

bool isFailed = false;

//Analyse du cJSON pour récupérer le tag.
void Update::analyseJson(cJSON* json, bool isBeta){
    if(isBeta){
        cJSON* releases = NULL;
        cJSON_ArrayForEach(releases, json) {
            cJSON* prerelease = cJSON_GetObjectItem(releases, "prerelease");
            if(prerelease->valueint == 1){
                mTag = strdup(cJSON_GetObjectItem(releases, "tag_name")->valuestring);
                break;
            }
        }
    }
    else
    {
        cJSON* release = cJSON_GetArrayItem(json, 0);
        mTag = strdup(cJSON_GetObjectItem(release, "tag_name")->valuestring);
    }
}

//Fonction principale pour savoir si une mise à jour du mod est disponible
void Update::checkUpdate(bool isBeta, const char* tagVersion)
{ 
    sead::ScopedCurrentHeapSetter heapSetter(mHeap);

    Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));
    
    DataStream dataStream = DataStream(30000);
    // Allouer l'objet DataDownloader dans le heap
    DataDownloader* download = new (mHeap) DataDownloader();

    if(download->Download(dataStream, "https://api.github.com/repos/Octoklingjs/Super-Mario-Odyssey-Experiment/releases") == false){
        Logger::log("Download failed !");
    }else{

        cJSON* data = cJSON_Parse((const char*)dataStream.getData());
        if (data == nullptr) {

            // Libérer la mémoire en cas d'erreur
            mTag = nullptr;
            Logger::log("Error : %s", cJSON_GetErrorPtr());

        }else{

            // Appeler la fonction d'analyse du JSON
            analyseJson(data, isBeta);

            Logger::log("Version : %s\n", getTag());
            Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));

        }
        // Libérer la mémoire allouée pour le JSON
        cJSON_Delete(data);
    }
}



// La fonction qui va servir à télécharger tout les fichiers !
bool Update::downloadAndInstallContents(const char* url, const char* path){
    sead::ScopedCurrentHeapSetter heapSetter(mHeap);
    
    Logger::log("Download %s in %s\n", url, path);
    DataStream fileStream = DataStream(0xFFFFF);
    Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));
    DataDownloader* download = new (mHeap) DataDownloader();

    //Télécharger et vérifier si le téléchargement à été correctement effectué
    if(download->Download(fileStream, url) == false){
        Logger::log("Download failed !\n");
        return false;
    }

    Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));

    Logger::log("Before\n");
    nn::fs::CreateFile(path, fileStream.getSize());
    Logger::log("Result: ");
    Logger::log("After");

    /*if(FsHelper::writeFileToPath(fileStream.getData(), fileStream.getSize(), path).isFailure()) {
        Logger::log("Failed to write file to SD card!\n");
        return false;
    }*/
    Logger::log("Correctly writed to the path\n");
    return true;
}

//Fonction télécharger la dernière version.
bool Update::downloadUpdate(){
    sead::ScopedCurrentHeapSetter heapSetter(mHeap);
    Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));

    //Super-Mario-Odyssey-Experiment's repo is deleted
    if(Update::downloadAndInstallContents("https://github.com/Octoklingjs/Super-Mario-Odyssey-Experiment/releases/download/V.0.2.0/subsdk9", "sd:/atmosphere/contents/0100000000010000/exefs/subsdk9") == false){
        Logger::log("Failed\n");
        isFailed = true;
        return true;
    }

    //Super-Mario-Odyssey-Experiment's repo is deleted
    Logger::log("Download main.npdm\n");
    if(Update::downloadAndInstallContents("https://github.com/Octoklingjs/Super-Mario-Odyssey-Experiment/releases/download/V.0.2.0/main.npdm", "sd:/atmosphere/contents/0100000000010000/exefs/main.npdm") == false){
        Logger::log("Failed\n");
        isFailed = true;
        return true;
    }

    return false;

    /*Logger::log("Used heap: %f\n", ((mHeap->getSize() * 0.001f) - (mHeap->getFreeSize() * 0.001f)));
    Logger::log("Download subsdk9\n");
    if(downloadAndInstallContents(("https://github.com/Octoklingjs/Super-Mario-Odyssey-Experiment/releases/download/" + std::string(mTag) + "/subsdk9").c_str(), "sd:/atmosphere/contents/0100000000010000/exefs/subsdk9") == false){
        Logger::log("Failed\n");
        isFailed = true;
        return;
    }*/
}