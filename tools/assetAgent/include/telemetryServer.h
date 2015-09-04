#ifndef _ASSETS_TELEMETRYSERVER_H_
#define _ASSETS_TELEMETRYSERVER_H_

#include "civetweb.h"
#include "loom/common/utils/json.h"
#include "loom/common/assets/assetProtocol.h"

// Asset protocol listener for telemetry messages
class TelemetryListener : public AssetProtocolMessageListener
{
public:
    // Handle the asset protocol telemetry messages
    virtual bool handleMessage(int fourcc, AssetProtocolHandler *handler, NetworkBuffer& buffer);

    JSON tickValuesJSON;
    JSON tickRangesJSON;
    JSON tickMetricsJSON;

    // The main JSON string already serialized and ready for transmission
    static utString tickMetricsJSONString;

    void updateMetricsJSON();
};

class TelemetryServer
{
    static struct mg_callbacks callbacks;
    static struct mg_context *server;

    static utString clientRoot;

public:
    // Set the client root from the provided SDK location (it is appended with the preset SDK subdir)
    static void setClientRootFromSDK(const char *root);

    // Set the client root for the server - this is where static files will be served from
    static void setClientRoot(const char *root);


    // Start the server running on the default port and from the provided client root directory
    static void start();

    // Stop the server
    static void stop();

    // Returns true if the server is running, otherwise false
    static bool isRunning();

    // Send the provided message to all the connected clients
    static void sendAll(const char* msg);
};


#endif