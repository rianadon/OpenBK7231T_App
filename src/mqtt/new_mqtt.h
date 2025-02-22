#ifndef __NEW_MQTT_H__
#define __NEW_MQTT_H__

#include "../new_common.h"

#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#if PLATFORM_XR809
#include "my_lwip2_mqtt_replacement.h"
#else
#include "lwip/apps/mqtt.h"
#endif

extern ip_addr_t mqtt_ip;
extern mqtt_client_t* mqtt_client;

void MQTT_init();
int MQTT_RunQuickTick();
int MQTT_RunEverySecondUpdate();

enum OBK_Publish_Result_e {
	OBK_PUBLISH_OK,
	OBK_PUBLISH_MUTEX_FAIL,
	OBK_PUBLISH_WAS_DISCONNECTED,
	OBK_PUBLISH_WAS_NOT_REQUIRED,
	OBK_PUBLISH_MEM_FAIL,
};

#define OBK_PUBLISH_FLAG_MUTEX_SILENT			1
#define OBK_PUBLISH_FLAG_RETAIN					2
#define OBK_PUBLISH_FLAG_FORCE_REMOVE_GET		4

#include "new_mqtt_deduper.h"


// ability to register callbacks for MQTT data
typedef struct obk_mqtt_request_tag {
	const unsigned char* received; // note: NOT terminated, may be binary
	int receivedLen;
	char topic[128];
} obk_mqtt_request_t;

#define MQTT_PUBLISH_ITEM_TOPIC_LENGTH    64
#define MQTT_PUBLISH_ITEM_CHANNEL_LENGTH  64
#define MQTT_PUBLISH_ITEM_VALUE_LENGTH    1023

typedef enum PostPublishCommands_e {
	None,
	PublishAll,
	PublishChannels
} PostPublishCommands;


/// @brief Publish queue item
typedef struct MqttPublishItem
{
	char topic[MQTT_PUBLISH_ITEM_TOPIC_LENGTH];
	char channel[MQTT_PUBLISH_ITEM_CHANNEL_LENGTH];
	char value[MQTT_PUBLISH_ITEM_VALUE_LENGTH];
	int flags;
	struct MqttPublishItem* next;
	PostPublishCommands command;
} MqttPublishItem_t;

#define MQTT_COMMAND_PUBLISH			"publish"
#define MQTT_COMMAND_PUBLISH_ALL		"publishAll"
#define MQTT_COMMAND_PUBLISH_CHANNELS	"publishChannels"
#define MQTT_COMMAND_PUBLISH_BENCHMARK  "publishBenchmark"

// Maximum length to log data parameters
#define MQTT_MAX_DATA_LOG_LENGTH					12

// Count of queued items published at once.
#define MQTT_QUEUED_ITEMS_PUBLISHED_AT_ONCE	3
#define MQTT_MAX_QUEUE_SIZE	                7

// callback function for mqtt.
// return 0 to allow the incoming topic/data to be processed by others/channel set.
// return 1 to 'eat the packet and terminate further processing.
typedef int (*mqtt_callback_fn)(obk_mqtt_request_t* request);

// topics must be unique (i.e. you can't have /about and /aboutme or /about/me)
// ALL topics currently must start with main device topic root.
// ID is unique and non-zero - so that callbacks can be replaced....
int MQTT_GetConnectEvents(void);
const char* get_error_name(int err);
int MQTT_GetConnectResult(void);
char* MQTT_GetStatusMessage(void);
int MQTT_GetPublishEventCounter(void);
int MQTT_GetPublishErrorCounter(void);
int MQTT_GetReceivedEventCounter(void);

void MQTT_ClearCallbacks();
int MQTT_RegisterCallback(const char* basetopic, const char* subscriptiontopic, int ID, mqtt_callback_fn callback);
int MQTT_RemoveCallback(int ID);

// this is called from tcp_thread context to queue received mqtt,
// and then we'll retrieve them from our own thread for processing.
//
// NOTE: this function is now public, but only because my unit tests
// system can use it to spoof MQTT packets to check if MQTT commands
// are working...
int MQTT_Post_Received(const char *topic, int topiclen, const unsigned char *data, int datalen);
int MQTT_Post_Received_Str(const char *topic, const char *data);

void MQTT_GetStats(int* outUsed, int* outMax, int* outFreeMem);

OBK_Publish_Result MQTT_PublishMain_StringFloat(const char* sChannel, float f);
OBK_Publish_Result MQTT_PublishMain_StringInt(const char* sChannel, int val);
OBK_Publish_Result MQTT_PublishMain_StringString(const char* sChannel, const char* valueStr, int flags);
OBK_Publish_Result MQTT_ChannelChangeCallback(int channel, int iVal);
void MQTT_PublishOnlyDeviceChannelsIfPossible();
void MQTT_QueuePublish(char* topic, char* channel, char* value, int flags);
void MQTT_QueuePublishWithCommand(char* topic, char* channel, char* value, int flags, PostPublishCommands command);
OBK_Publish_Result MQTT_Publish(char* sTopic, char* sChannel, char* value, int flags);
bool MQTT_IsReady();

#endif // __NEW_MQTT_H__
