#include <SmingCore.h>
#include <ArduinoJson.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put your SSID and password here
#define WIFI_PWD "PleaseEnterPass"
#endif

HttpServer server;
int counter = 0;

void onIndex(HttpRequest& request, HttpResponse& response)
{
	counter++;
	DynamicJsonDocument doc(2048);
	JsonObject json = doc.to<JsonObject>();
	json["counter"] = String(counter);
	json["AP IP"] = WifiAccessPoint.getIP().toString();
	json["AP MAC"] = WifiAccessPoint.getMAC();
	json["Station IP"] = WifiStation.getIP().toString();
	json["Station MAC"] = WifiStation.getMAC();
	String body;
	serializeJson(doc, body);
	response.setContentType(MIME_JSON);
	response.sendString(body);
}

void startWebServer()
{
	server.listen(80);
	server.paths.set("/", onIndex);

	Serial.println("\r\n===  WEB SERVER STARTED ===");
	Serial.println(WifiAccessPoint.getIP());
	Serial.println("==============================\r\n");
}

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	Serial.println("Got station ip: " + ip.toString());
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files

	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("Sming access point", "", AUTH_OPEN);
	// Without that line AP and Station web server works:
	//WifiAccessPoint.setIP(IpAddress(192, 168, 1, 4));
	startWebServer();

	// Run our method when station was connected to AP
	WifiEvents.onStationGotIP(gotIP);
}
