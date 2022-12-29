#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "WiFi.h"

#define CAMERA_MODEL_M5STACK_CAMERAF
#include "camera.h"

WiFiServer camServer(80);
WiFiServer motServer(81);
ESP32Camera camera;

const char* ssid = "test";
const char* password = "";

String hostname = "ESP32 Cam Server";

void setup() {
	WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 

	Serial.begin(115200);

	WiFi.config(INADDR_NONE, INADDR_NONE, 
		INADDR_NONE, INADDR_NONE);
	WiFi.setHostname(hostname.c_str()); //define hostname

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");

	camServer.begin();
	motServer.begin();

	Serial.print("Camera Ready! Use 'http://");
	Serial.print(WiFi.localIP());
	Serial.println(":80' to stream");

	Serial.print("Drive Ready! Use 'http://");
	Serial.print(WiFi.localIP());
	Serial.println(":81' to control");

	camera.begin(FRAMESIZE_QQVGA, PIXFORMAT_JPEG);

	xTaskCreate(
		motorTask, /* Task function. */
		"motor Task", /* name of task. */
		10000, /* Stack size of task */
		NULL, /* parameter of the task */
		1, /* priority of the task */
		NULL); /* Task handle to keep track of created task */
}

void loop() {
	WiFiClient client = camServer.available();

	if (client) {
		Serial.println("New Client for video streaming."); 
		while (client.connected()) {

			// capture camera data
			camera_fb_t *fb = esp_camera_fb_get();

			if(client.available()) {
				char cmd = client.read();

				if(cmd==12) {

					// send camera data
					client.write(
						(const uint8_t *)&fb->len, 
						sizeof(fb->len));
					client.write(
						(const uint8_t *)fb->buf, 
						fb->len);

				} 
			}     

			esp_camera_fb_return(fb);

		}
	}
}

const int rSCL = 4;
const int lSDA = 13;

void motorTask( void * parameter ) {

	pinMode(rSCL, OUTPUT);
	pinMode(lSDA, OUTPUT);
  
  digitalWrite(rSCL, HIGH);
  digitalWrite(lSDA, HIGH);

	for(;;) {
		WiFiClient client = motServer.available();

		if (client) {
			Serial.println("New Client for car driving.");
			while (client.connected()) {

				if(client.available()) {
					char rl = client.read();            
					// printf("%d\n", rl);
					digitalWrite(rSCL, (rl>>1)&1);
					digitalWrite(lSDA, rl&1);
				}
			}
		}
	}

}
