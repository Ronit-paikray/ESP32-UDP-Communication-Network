
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 03_ESP32_UDP_Web_Server_Master
// I made the Master send a broadcast message on the same Port that the master uses.
// I made the Master send a broadcast message, instead of sending a message using the Slave's IP address,
// because if using a router usually the IP address will change at certain times.
// So I think it's better to send a message with a broadcast and the slave is programmed to know if the message it receives is intended for it or not.

//----------------------------------------Including Libraries.
#include "WiFi.h"
#include "AsyncUDP.h"
#include <WebServer.h>
#include "PageIndex.h"
//----------------------------------------



#define on_Board_LED  2



//----------------------------------------Variable declaration for your network credentials.
const char* ssid = "YOUR_WIFI_SSID";  //--> Your wifi name.
const char* password = "YOUR_WIFI_PASSWORD"; //--> Your wifi password.
//----------------------------------------

const int udpPort = 3333;

String ID;
String incoming_Commands;

byte ESP32A_LED_1_Sta, ESP32A_LED_2_Sta;
byte ESP32B_LED_1_Sta, ESP32B_LED_2_Sta;


// Server on port 80.
WebServer server(80);

// Declaring the "AsyncUDP" object as "udp".
AsyncUDP udp;



//________________________________________________________________________________ handleRoot()
// This routine is executed when you open ESP32 IP Address in browser.
void handleRoot() {
  server.send(200, "text/html", MAIN_page); //--> Send web page.
}
//________________________________________________________________________________



//________________________________________________________________________________ handleSettings().
// Subroutines for handling handle settings from clients.
void handleCommands() {
  ID = "";
  incoming_Commands = "";

  ID = server.arg("id");
  incoming_Commands = server.arg("cmd");

  Serial.println();
  Serial.println("-------------incoming_Commands");
  Serial.print("ID : ");
  Serial.println(ID);
  Serial.print("incoming_Commands : ");
  Serial.println(incoming_Commands);
  Serial.println("-------------");

  if (incoming_Commands == "GSAL") {  //--> Gets the status of all LEDs from all Slaves.
    send_Message(udpPort, "ESP32A,GSAL");
    delay(100);
    send_Message(udpPort, "ESP32B,GSAL");
    delay(100);
  } else if (incoming_Commands == "GLLS") { //--> Sends the status of all LEDs received from all Slaves to be displayed on the web page.
    Serial.println();
    Serial.println("-------------Sending replies to the client");
    
    char resp[10];
    sprintf(resp, "%d|%d|%d|%d", ESP32A_LED_1_Sta, ESP32A_LED_2_Sta, ESP32B_LED_1_Sta, ESP32B_LED_2_Sta);
    
    Serial.print("resp : ");
    Serial.println(resp);
    Serial.println("-------------");
    
    server.send(200, "text/plane", resp); //--> Sending replies to the client.
  } else {
    // Send commands to the slave to control the LED.
    send_Message(udpPort, ID + "," + incoming_Commands);
  }

  server.send(200, "text/plane", "+OK"); //--> Sending replies to the client.
  delay(100);
}
//________________________________________________________________________________ 



//________________________________________________________________________________ void send_Message()
void send_Message(int udp_port, String msg) {
  if(WiFi.status()== WL_CONNECTED){
    // Send broadcast on port 3333 (look at the "udpPort" variable).
    Serial.println();
    Serial.println("-------------Send_broadcast_messages");
    Serial.print("Send a broadcast message on port : ");
    Serial.println(udp_port);
    Serial.print("Messages : ");
    Serial.println(msg);
    Serial.println("Send Message.");   
    udp.broadcastTo(msg.c_str(), udp_port);
    Serial.println("-------------");
    
    receive_Message();
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

        //----------------------------------------
        // When the command sent to the slave is "GSAL" (a command to get the LED status), then the response from the slave is as follows:
        // example of the command sent : ESP32A,GSAL (get the LEDs status on ESP32A or Slave A).
        // then the response from ESP32A or Slave A : ESP32A,GSAL,0,1
        // 0-> ESP32A = ID.
        // 1-> GSAL = command to get the LED status.
        // 2-> 0 = LED 1 status (0 = off, 1 = on).
        // 3-> 1 = LED 2 status (0 = off, 1 = on).
        //
        // String rcv_ID = getValue(msg_Received, ',', 0);
        // String GSAL = getValue(msg_Received, ',', 1);
        // ESP32A_LED_1_Sta = getValue(msg_Received, ',', 2).toInt();
        // ESP32A_LED_2_Sta = getValue(msg_Received, ',', 3).toInt();
        
        String rcv_ID = getValue(msg_Received, ',', 0);
        String GSAL = getValue(msg_Received, ',', 1);

        if (GSAL == "GSAL") {
          if (rcv_ID == "ESP32A") {
            ESP32A_LED_1_Sta = getValue(msg_Received, ',', 2).toInt();
            ESP32A_LED_2_Sta = getValue(msg_Received, ',', 3).toInt();
          }
          if (rcv_ID == "ESP32B") {
            ESP32B_LED_1_Sta = getValue(msg_Received, ',', 2).toInt();
            ESP32B_LED_2_Sta = getValue(msg_Received, ',', 3).toInt();
          }

          return;
        }
        //----------------------------------------

        //----------------------------------------
        // example of a command sent to control the LED : ESP32A,LED_1_ON (command to turn on LED 1 on ESP32A or Slave A, see "onclick" in the "PageIndex.h" code).
        // then the response from ESP32A or Slave A: ESP32A,LED_1_ON
        // 0-> ESP32A = ID.
        // 1-> LED_1_ON = LED 1 status.
        //
        // String rcv_ID = getValue(msg_Received, ',', 0);
        // String rcv_Sta = getValue(msg_Received, ',', 1);
        
        if (rcv_ID == "ESP32A") {
          String rcv_Sta = getValue(msg_Received, ',', 1);

          if (rcv_Sta == "LED_1_ON") {
            ESP32A_LED_1_Sta = 1;
          } else if (rcv_Sta == "LED_1_OFF") {
            ESP32A_LED_1_Sta = 0;
          } else if (rcv_Sta == "LED_2_ON") {
            ESP32A_LED_2_Sta = 1;
          } else if (rcv_Sta == "LED_2_OFF") {
            ESP32A_LED_2_Sta = 0;
          }
        }
        //----------------------------------------

        //----------------------------------------
        if (rcv_ID == "ESP32B") {
          String rcv_Sta = getValue(msg_Received, ',', 1);

          if (rcv_Sta == "LED_1_ON") {
            ESP32B_LED_1_Sta = 1;
          } else if (rcv_Sta == "LED_1_OFF") {
            ESP32B_LED_1_Sta = 0;
          } else if (rcv_Sta == "LED_2_ON") {
            ESP32B_LED_2_Sta = 1;
          } else if (rcv_Sta == "LED_2_OFF") {
            ESP32B_LED_2_Sta = 0;
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
  Serial.println("-------------");
  digitalWrite(on_Board_LED, LOW);
  //:::::::::::::::::: 
  delay(500);
  //---------------------------------------- 

  //----------------------------------------Setting the server.
  server.on("/", handleRoot); 
  server.on("/commands", handleCommands);
  delay(500);
  //----------------------------------------

  //----------------------------------------Start server.
  server.begin(); 
  Serial.println();
  Serial.println("HTTP server started");
  delay(500);
  //----------------------------------------

  //----------------------------------------Open the IP address in your browser to open the interface page.
  // Make sure that your computer / mobile device and ESP32 are connected to the same WiFi network.
  Serial.println();
  Serial.print("IP address : ");
  Serial.println(WiFi.localIP());
  Serial.println("Open the IP address in your browser to open the interface page.");
  Serial.println("Make sure that your computer / mobile device and");
  Serial.println("ESP32 are connected to the same WiFi network.");
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

  // Handle client requests.
  server.handleClient();
  delay(10);
}
//________________________________________________________________________________ 
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<