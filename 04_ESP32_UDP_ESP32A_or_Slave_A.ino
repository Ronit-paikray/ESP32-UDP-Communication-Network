//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 04_ESP32_UDP_ESP32A_or_Slave_A
//----------------------------------------Including Libraries.
#include "WiFi.h"
#include "AsyncUDP.h"
//----------------------------------------



#define on_Board_LED  2
#define LED_1 13
#define LED_2 12



//----------------------------------------Variable declaration for your network credentials.
const char* ssid = "YOUR_WIFI_SSID";  //--> Your wifi name.
const char* password = "YOUR_WIFI_PASSWORD"; //--> Your wifi password.
//----------------------------------------

const int udpPort = 3333;



AsyncUDP udp;



//________________________________________________________________________________ void send_Message()
void send_Message(int IP_Addr_Des[], int udp_port, String msg) {
  if(WiFi.status()== WL_CONNECTED){
    if (udp.connect(IPAddress(IP_Addr_Des[0], IP_Addr_Des[1], IP_Addr_Des[2], IP_Addr_Des[3]), udp_port)) {
      Serial.println();
      Serial.println("-------------send_Message");
      Serial.println("UDP connected.");
      Serial.print("Messages to send : ");
      Serial.println(msg);
      Serial.print("Send to : ");
      for (byte i = 0; i < 4; i++) {
        Serial.print(IP_Addr_Des[i]);
        if (i > 2) break;
        Serial.print(".");
      }
      Serial.print(":");
      Serial.println(udp_port);
      Serial.println("Send Message.");
      udp.print(msg);
      Serial.println("-------------");

      receive_Message();
    }
  } else {
    Serial.println();
    Serial.println("-------------");
    Serial.println("WiFi disconnected !");
    Serial.println("-------------");
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ void receive_Message()
void receive_Message() {
  if(WiFi.status()== WL_CONNECTED){
    if (udp.listen(udpPort)) {
      udp.onPacket([](AsyncUDPPacket packet) {
        Serial.println();
        Serial.println("-------------receive_Message");
        Serial.print("UDP Packet Type : ");
        Serial.println(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
        Serial.print("From : ");
        Serial.print(packet.remoteIP());
        Serial.print(":");
        Serial.println(packet.remotePort());
        Serial.print("Length : ");
        Serial.println(packet.length());

        String msg_Received = "";
        for (int i = 0; i < packet.length(); i++) {
          char packet_Char = (char)packet.data()[i];
          msg_Received += packet_Char;
        }

        Serial.print("Data / Message : ");
        Serial.print(msg_Received);
        Serial.println();
        Serial.println("-------------");

        delay(10);

        //----------------------------------------
        int arr_destination_IP_Address[4];
        arr_destination_IP_Address[0] = packet.remoteIP()[0];
        arr_destination_IP_Address[1] = packet.remoteIP()[1];
        arr_destination_IP_Address[2] = packet.remoteIP()[2];
        arr_destination_IP_Address[3] = packet.remoteIP()[3];

        String myID = getValue(msg_Received, ',', 0);

        if (myID == "ESP32A") {
          Serial.println();
          Serial.println("Send a reply message");

          String cmd = getValue(msg_Received, ',', 1);

          if (cmd == "LED_1_ON") {
            digitalWrite(LED_1, HIGH);
  
            send_Message(arr_destination_IP_Address, udpPort, "ESP32A,LED_1_ON");
          } else if (cmd == "LED_1_OFF") {
            digitalWrite(LED_1, LOW);
  
            send_Message(arr_destination_IP_Address, udpPort, "ESP32A,LED_1_OFF");
          } else if (cmd == "LED_2_ON") {
            digitalWrite(LED_2, HIGH);
  
            send_Message(arr_destination_IP_Address, udpPort, "ESP32A,LED_2_ON");
          } else if (cmd == "LED_2_OFF") {
            digitalWrite(LED_2, LOW);
  
            send_Message(arr_destination_IP_Address, udpPort, "ESP32A,LED_2_OFF");
          } else if (cmd == "GSAL") {
            byte sta_LED_1, sta_LED_2;
  
            sta_LED_1 = digitalRead(LED_1);
            sta_LED_2 = digitalRead(LED_2);
  
            String send_MSG;
            send_MSG  = "ESP32A,GSAL,";
            send_MSG += String(sta_LED_1);
            send_MSG += ",";
            send_MSG += String(sta_LED_2);
  
            send_Message(arr_destination_IP_Address, udpPort, send_MSG);
          }
        }
        //----------------------------------------
      });
    }
  }
}
//________________________________________________________________________________ 



//________________________________________________________________________________ getValue()
// String function to process the data received
// I got this from : https://www.electroniclinic.com/reyax-lora-based-multiple-sensors-monitoring-using-arduino/
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;
  
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  delay(2000);
  Serial.begin(115200);
  Serial.println();

  pinMode(on_Board_LED, OUTPUT);
  digitalWrite(on_Board_LED, LOW);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);

  //----------------------------------------Set Wifi to STA mode.
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  WiFi.mode(WIFI_STA);
  Serial.println("-------------");
  delay(500);
  //---------------------------------------- 

  //----------------------------------------Connect to Wi-Fi (STA).
  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.disconnect(true);
  
  WiFi.begin(ssid, password);
  
  //:::::::::::::::::: The process of connecting ESP32 with WiFi Hotspot / WiFi Router.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.
  
  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(on_Board_LED, HIGH);
    delay(250);
    digitalWrite(on_Board_LED, LOW);
    delay(250);
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      Serial.println();
      Serial.println("Failed to connect to WiFi. The ESP32 will be restarted.");
      Serial.println("-------------");
      digitalWrite(on_Board_LED, LOW);
      delay(1000);
      ESP.restart();
    }
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("-------------");
  digitalWrite(on_Board_LED, LOW);
  //:::::::::::::::::: 
  delay(500);
  //---------------------------------------- 

  digitalWrite(on_Board_LED, HIGH);
  delay(1000);
  digitalWrite(on_Board_LED, LOW);
  delay(500);
  digitalWrite(on_Board_LED, HIGH);
  delay(1000);
  digitalWrite(on_Board_LED, LOW);

  // Calls the receive_Message() subroutine.
  receive_Message();
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<