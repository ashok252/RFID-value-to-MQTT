#include <WiFi.h>

#include <PubSubClient.h>
#include <MFRC522.h> // MFRC522 RFID module library.
#include <SPI.h> // SPI device communication library.
#include <EEPROM.h> // EEPROM (memory) library.
#define pinRST 22 // Defines pins for RST, SDA connections respectively.
#define pinSS 21
byte readCard[4]; // Array that will hold UID of the RFID card.
int successRead;
MFRC522 mfrc522(pinSS, pinRST); // Creates MFRC522 instance.
MFRC522::MIFARE_Key key; // Creates MIFARE key instance.
const char* ssid = " "; //Name of the Wi-Fi used.
const char* password = " "; // Password of the Wi-Fi used.
const char* subtopic = "inTopic";
const char* pubtopic = "outTopic";
const char* mqtt_user = " "; // MQTT Username.
const char* mqtt_pass = " "; // MQTT Password.
int mqtt_port =; //MQTT Port Address
const char* mqtt_server = " "; //MQTT server name
WiFiClient espClient;
PubSubClient client(espClient);
void setup() {
Serial.begin(9600);
setup_wifi();
client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
SPI.begin(); // Initiates SPI connection between RFID module and Arduino.
mfrc522.PCD_Init(); // Initiates MFRC522 RFID module.
Serial.println("RFID reading process initiated."); // Prints user commands.
Serial.println("Please scan your RFID card to the reader.");
}
void setup_wifi() {
delay(10);
// We start by connecting to a WiFi network
Serial.println();

Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(subtopic);
Serial.print("] ");
}
void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
Serial.println("connected");
// Once connected, publish an announcement..
client.subscribe(subtopic);
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println(" try again in 5 seconds");
// Wait 5 seconds before retrying
delay(50);
}
}
}
void loop() {

if (!client.connected()) {
reconnect();
}
client.loop();
getID();
}
void getID() // Function that will read and print the RFID cards UID.
{
if ( ! mfrc522.PICC_IsNewCardPresent()) // If statement that looks for new cards.
{
return ;
}
if ( ! mfrc522.PICC_ReadCardSerial()) // If statement that selects one of the cards.
{
return ;
}
for (int i = 0; i < mfrc522.uid.size; i++) {
readCard[i] = mfrc522.uid.uidByte[i]; // Reads RFID cards UID.
Serial.print(readCard[i], HEX); // Prints RFID cards UID to the serial monitor.
}
char msg[20];
msg[0]='\0';
array_to_string(readCard,4,msg);
client.publish(pubtopic,msg);
Serial.print("\n");
//client.publish(pubtopic,"\n" );
//
mfrc522.PICC_HaltA(); // Stops the reading process.
}
void array_to_string(byte a[],unsigned int len,char buffer[])
{
for(unsigned int i=0;i<len;i++)
{
byte nib1=(a[i]>>4)&0x0F;
byte nib2=(a[i]>>0)&0x0F;
buffer[i*2+0]=nib1 < 0x0A ? '0' + nib1 : 'A'+ nib1 - 0x0A;
buffer[i*2+1]=nib2 < 0x0A ? '0' + nib2 : 'A'+ nib2 - 0x0A;
}
buffer[len*2]='\0';
}
