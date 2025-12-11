// OttoNinja Web Control 

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Servo.h>
#include <FS.h> // Required for LittleFS
#include <LittleFS.h> // LittleFS support
#include <ESP8266mDNS.h> // mDNS support

#define CONNECT_MODE 2 // 1 for station ... 2 for access point

const char* ssid = "ottoninja"; // write your WiFi name
const char* password = "ottoninja"; // write your WiFi password
const char* esp_hostname = "ottoninja"; // Desired hostname

IPAddress ap_ip(192,168,1,1);     
IPAddress ap_gateway(192,168,1,1);   
IPAddress ap_subnet(255,255,255,0); 

// leg & foot Pins
const uint8_t LeftLegPin = D8; // D8 or gpio 15
const uint8_t RightLegPin = D4; // D4 or gpio 2
const uint8_t LeftFootPin = D7; // D7 or gpio 13
const uint8_t RightFootPin = D3; // D3 or gpio 0

// Ultrasound & Buzzer options
#define ECHO D6 // gpio 12
#define TRIG D5 // gpio 14
#define DIST 15 // distance in centimeters (cm)
#define BUZZER D1  // gpio 5

// Left Leg standing Position
#define LA0 60    // 0 = Full Tilt Right   180 = Full Tilt Left   Default = 60 // was 60

// Right Leg standing position
#define RA0 120   // 0 = Full Tilt Right   180 = Full Tilt Left   Default = 120 // was 120

// Left Leg tilt left walking position
#define LATL 100   // 0 = Full Tilt Right   180 = Full Tilt Left   Default BASIC = 85   Default HUMANOID = 80 // was 120

// Right Leg tilt left walking position
#define RATL 175  // 0 = Full Tilt Right   180 = Full Tilt Left   Default BASIC = 175   Default HUMANOID = 150 // was 180

// Left Leg tilt right walking position
#define LATR 5   // 0 = Full Tilt Right   180 = Full Tilt Left   Default BASIC = 5   Default HUMANOID = 30 // was 0

// Right Leg tilt right walking position
#define RATR 80  // 0 = Full Tilt Right   180 = Full Tilt Left   Default BASIC = 95  Default HUMANOID = 100 // was 65

// Left Leg roll Position
#define LA1 180  // 0 = Full Tilt Right   180 = Full Tilt Left   Default = 170 // was 170

// Right Leg roll position
#define RA1 0    // 0 = Full Tilt Right   180 = Full Tilt Left   Default = 10 // was 10

// Creamos una instancia del servidor
// y especificamos el puerto a escuchar como un argumento.
WiFiServer server(80);

Servo LeftLeg;
Servo LeftFoot;
Servo RightFoot;
Servo RightLeg;

bool walkMode = false;
bool rollMode = false;
int roll_right_forward_speed = 40;
int roll_left_forward_speed = 40;
int roll_right_backward_speed = 40;
int roll_left_backward_speed = 40;
String command = "";

// ultrasound_distance centimeters (cm)
long ultrasound_distance() {
   long duration, distance;
   digitalWrite(TRIG, LOW);
   delayMicroseconds(2);
   digitalWrite(TRIG, HIGH);
   delayMicroseconds(10);
   digitalWrite(TRIG, LOW);
   duration = pulseIn(ECHO, HIGH);
   distance = duration/58;
   Serial.print("Distance (cm): ");
   Serial.println(distance);
   return distance;
}

void AvoidanceWalk() {
  if (ultrasound_distance() <= DIST) {
    Home();
    delay(50);
    WalkRight();
  }
  WalkForward();
}

void AvoidanceRoll() {
  if (ultrasound_distance() <= DIST) {
    NinjaRollBackward(roll_right_backward_speed, roll_left_backward_speed);
    delay(500);
    NinjaRollStop();
    delay(100);
    NinjaRollRight(roll_right_forward_speed, roll_left_forward_speed);
    delay(500);
    NinjaRollStop();
    delay(100);
  }
  NinjaRollForward(roll_right_forward_speed, roll_left_forward_speed);
  //delay(100);
}

void Home() {

  LeftFoot.attach(LeftFootPin, 544, 2400);
  RightFoot.attach(RightFootPin, 544, 2400);
  LeftLeg.attach(LeftLegPin, 544, 2400);
  RightLeg.attach(RightLegPin, 544, 2400);
  delay(100);

  LeftFoot.write(90);
  RightFoot.write(90);
  LeftLeg.write(LA0); 
  RightLeg.write(RA0);
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
}

void SetWalk() {
  walkMode = true;
  rollMode = false;
  LeftLeg.attach(LeftLegPin, 544, 2400);
  RightLeg.attach(RightLegPin, 544, 2400);
  LeftLeg.write(LA0);
  RightLeg.write(RA0);
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
  delay(100);
}

void SetRoll() {
  walkMode = false;
  rollMode = true;
  LeftLeg.attach(LeftLegPin, 544, 2400);
  RightLeg.attach(RightLegPin, 544, 2400);
  LeftLeg.write(LA1);  // was 170
  RightLeg.write(RA1); // was 10
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
  delay(100);
}

void TiltToRight() {
  LeftLeg.attach(LeftLegPin, 544, 2400);
  RightLeg.attach(RightLegPin, 544, 2400);
  LeftLeg.write(LATR);  // was 0
  RightLeg.write(RATR); // was 65
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
  delay(300);
}

void TiltToLeft() {
  LeftLeg.attach(LeftLegPin, 544, 2400);
  RightLeg.attach(RightLegPin, 544, 2400);
  LeftLeg.write(LATL);  // was 120
  RightLeg.write(RATL); // was 180
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
  delay(300);
}

void MoveRightFoot(int s, int t) {
  RightFoot.attach(RightFootPin, 544, 2400);
  RightFoot.write(s);
  delay(t);
  RightFoot.write(90);
  delay(100);
  RightFoot.detach();
  delay(300);
}

void MoveLeftFoot(int s, int t) {
  LeftFoot.attach(LeftFootPin, 544, 2400);
  LeftFoot.write(s);
  delay(t);
  LeftFoot.write(90);
  delay(100);
  LeftFoot.detach();
  delay(300);
}

void ReturnFromRight() {
  LeftLeg.attach(LeftLegPin, 544, 2400);
  LeftLeg.write(60);
  RightLeg.attach(RightLegPin, 544, 2400);
  for(int count=65;count<=120;count+=2) {
    RightLeg.write(count);
    delay(25);
  }
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
}

void ReturnFromLeft() {
  RightLeg.attach(RightLegPin, 544, 2400);
  RightLeg.write(120);
  LeftLeg.attach(LeftLegPin, 544, 2400);
  for(int count=120;count>=60;count-=2) {
    LeftLeg.write(count);
    delay(25);
  }
  delay(300);
  LeftLeg.detach();
  RightLeg.detach();
}

void WalkForward() {
  TiltToRight();
  delay(100);
  MoveRightFoot(70, 250);
  delay(100);
  ReturnFromRight();
  
  TiltToLeft();
  delay(100);
  MoveLeftFoot(140, 350);
  delay(100);
  ReturnFromLeft();
}

void WalkRight() {
  TiltToRight();
  delay(100);
  MoveRightFoot(70, 350);
  delay(100);
  ReturnFromRight();
}

void WalkBackward() {
  TiltToRight();
  delay(100);
  MoveRightFoot(120, 250);
  delay(100);
  ReturnFromRight();
  
  TiltToLeft();
  delay(100);
  MoveLeftFoot(60, 350);
  delay(100);
  ReturnFromLeft();
}

void WalkLeft() {
  TiltToLeft();
  delay(100);
  MoveLeftFoot(140, 350);
  delay(100);
  ReturnFromLeft();
}

void NinjaRollForward(int speedR, int speedL) {
  LeftFoot.attach(LeftFootPin, 544, 2400);
  RightFoot.attach(RightFootPin, 544, 2400);
  LeftFoot.write(90 + speedL);
  RightFoot.write(90 - speedR);
}

void NinjaRollRight(int speedR, int speedL) {
  LeftFoot.attach(LeftFootPin, 544, 2400);
  RightFoot.attach(RightFootPin, 544, 2400);
  LeftFoot.write(90 + speedL);
  RightFoot.write(90 + speedR);
}

void NinjaRollLeft(int speedR, int speedL) {
  LeftFoot.attach(LeftFootPin, 544, 2400);
  RightFoot.attach(RightFootPin, 544, 2400);
  LeftFoot.write(90 - speedL);
  RightFoot.write(90 - speedR);
}

void NinjaRollBackward(int speedR, int speedL) {
  LeftFoot.attach(LeftFootPin, 544, 2400);
  RightFoot.attach(RightFootPin, 544, 2400);
  LeftFoot.write(90 - speedL);
  RightFoot.write(90 + speedR);
}

void NinjaRollStop() {
  LeftFoot.write(90);
  RightFoot.write(90);
  LeftFoot.detach();
  RightFoot.detach();
}

void joystickWalk(int x, int y) {
  if ((x >= -5)&&(x <= 5)&&(y >= -5)&&(y <= 5)){ command = "home"; }
  else if (y < -25 && x < -25 || y > 25 && x < -25) { command = "left"; }
  else if (y < -25 && x > 25 || y > 25 && x > 25) { command = "right"; }
  else if (y < -25) { command = "forward"; }
  else if (y > 25) { command = "backward"; }
}
  
void joystickRoll(int x, int y) {
  if ((x >= -5)&&(x <= 5)&&(y >= -5)&&(y <= 5)){NinjaRollStop();}
  else{
    LeftFoot.attach(LeftFootPin, 544, 2400);
    RightFoot.attach(RightFootPin, 544, 2400);
    int LWS= map(y, -50, 50, 135, 45);
    int RWS= map(y, -50, 50, 45, 135);
    int LWD= map(x, 50, -50, 45, 0);
    int RWD= map(x, 50, -50, 0, -45);
    LeftFoot.write(LWS+LWD);
    RightFoot.write(RWS+RWD);
    }
}

void Settings(String speeds) {
  decodeSpeeds(speeds);
}

void decodeSpeeds(String c) {
  int counter = 0;
  String RF = "";
  String LF = "";
  String RB = "";
  String LB = "";
  
  for (int i=1; i<c.length(); i++) {
      if(isDigit(c[i])) {
          if(counter == 0) {
              RF += c[i];
          }
          else if (counter == 1) {
              LF += c[i];
          }
          else if (counter == 2) {
              RB += c[i];
          }
          else if (counter == 3) {
              LB += c[i];
          }
      }
      else if (c[i] == '&') {
          counter++;
      }
  }

  roll_right_forward_speed = RF.toInt();
  roll_left_forward_speed = LF.toInt();
  roll_right_backward_speed = RB.toInt();
  roll_left_backward_speed = LB.toInt();
  Serial.println("");
  Serial.println(c);
  Serial.println(RF.toInt());
  Serial.println(LF.toInt());
  Serial.println(RB.toInt());
  Serial.println(LB.toInt());
  
}

void setup() {
  Serial.begin(115200);
  delay(10);  

  pinMode(BUZZER, OUTPUT); // Sets the buzzer pin as an Output
  pinMode(TRIG, OUTPUT); // Sets the trig Pin as an Output
  pinMode(ECHO, INPUT); // Sets the echo Pin as an Input
  
  // Imprimimos la información sobre la red WiFi.
  Serial.println();
  Serial.println();

#if (CONNECT_MODE == 1)
  Serial.print("Conectando a ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  
  // Set the hostname *before* connecting
  WiFi.hostname(esp_hostname);   
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");

#elif (CONNECT_MODE == 2)
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_AP);
  WiFi.config(ap_ip, ap_gateway, ap_subnet);
  
  // Set the hostname *before* connecting
  WiFi.hostname(esp_hostname);   
  boolean result = WiFi.softAP(ssid, password);

  if (result == true) {
    Serial.println(" Ready");
  } else {
    Serial.println(" Failed!");
  }

  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

#endif
  
  // Iniciamos el servidor
  server.begin();
  Serial.println("Servidor iniciado");

  Serial.println(WiFi.localIP());

  // Initialize mDNS
  if(!MDNS.begin(esp_hostname)){
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
  
  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  tone(BUZZER, 440, 1000);
  Home();
  SetWalk(); // default to walk mode
}

void loop() {
  CheckClient();//if something is coming at us

  if (command == "walkmode") {
    SetWalk();
    command = "";
  }
  else if (command == "rollmode") {
    SetRoll();
    command = "";
  }
  else if (command == "home") {
    Home();
    command = "";
  }
  else if (command == "forward") {
    if(walkMode) {
      WalkForward();
      command = "";
    } else {
      NinjaRollForward(roll_right_forward_speed, roll_left_forward_speed);
    }
  }
  else if (command == "backward") {
    if(walkMode) {
      WalkBackward();
      command = "";
    } else {
      NinjaRollBackward(roll_right_backward_speed, roll_left_backward_speed);
    }
  }
  else if (command == "right") {
    if(walkMode) {
      WalkRight();
      command = "";
    } else {
      NinjaRollRight(roll_right_forward_speed, roll_left_forward_speed);
      command = "";
    }
  }
  else if (command == "left") {
    if(walkMode) {
      WalkLeft();
      command = "";
    } else {
      NinjaRollLeft(roll_right_backward_speed, roll_left_backward_speed);
      command = "";
    }
  }
  else if (command == "stop") {
    NinjaRollStop();
  }
  else if (command == "avoidancewalk") {
    if(rollMode) {
      SetWalk();
    }
    AvoidanceWalk();
  }
  else if (command == "avoidanceroll") {
    if(walkMode) {
      SetRoll();
    }
    AvoidanceRoll();
  }

}

String getContentType(String filename){
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".svg")) return "image/svg+xml";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  else if(filename.endsWith(".woff2")) return "font/woff2"; // Added for woff2 fonts
  return "text/plain";
}

bool handleFileRead(WiFiClient client, String path) {
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    if (file) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: " + contentType);
      client.println("Content-Length: " + String(file.size()));
      client.println();
      size_t bytesSent = client.write(file);
      Serial.println(String("Sent file: ") + path + " (" + bytesSent + " bytes)");
      file.close();
      return true;
    }
  }
  Serial.println(String("File Not Found: ") + path);
  return false;
}

void CheckClient() {
  // Verificamos si un cliente se ha conectado
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Esperamos hasta que un cliente envíe algún dato.
  Serial.println("nuevo cliente");
  while(!client.available()){
    delay(1);
  }
  
  // Leemos la primera línea de la petición
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Extract the path from the request
  String path = "";
  int space1 = req.indexOf(' ');
  if (space1 != -1) {
    int space2 = req.indexOf(' ', space1 + 1);
    if (space2 != -1) {
      path = req.substring(space1 + 1, space2);
    }
  }

  if (req.indexOf("/walkmode") != -1) {
    command = "walkmode";
  }
  else if (req.indexOf("/rollmode") != -1){
    command = "rollmode";
  }
  else if (req.indexOf("/home") != -1) {
    command = "home";
  }
  else if (req.indexOf("/forward") != -1){  
    command = "forward";
  }
  else if (req.indexOf("/right") != -1){
    command = "right";
  }
  else if (req.indexOf("/backward") != -1){
    command = "backward";
  }
  else if (req.indexOf("/left") != -1){
    command = "left";
  }
  else if (req.indexOf("/stop") != -1){
    command = "stop";
  }
  else if (req.indexOf("/avoidancewalk") != -1){
    command = "avoidancewalk";
  }
  else if (req.indexOf("/avoidanceroll") != -1){
    command = "avoidanceroll";
  }
  else if (req.indexOf("/R=") > 0){
    Settings(req);
    Home();
    command = "";
  }
  else if (req.indexOf("/J") > 0){
    command = "joystick";
    GetCoords(req);
  }
  else {
    // If not a command, try to serve a file from LittleFS
    if (!handleFileRead(client, path)) {
      Serial.println("petición inválida o archivo no encontrado");
      // Send a 404 Not Found response
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/plain");
      client.println();
      client.println("404 Not Found");
    }
    client.stop();
    return;
  }
  
  client.flush();

  // For commands, send a simple OK response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nCommand Executed</html>\n";

  // Enviamos la respuesta al cliente
  client.print(s);
  delay(1);
  Serial.println("Cliente desconectado");
}

void GetCoords(String str) {
  String x = str.substring(str.lastIndexOf('J')+1, str.lastIndexOf(','));
  String y = str.substring(str.lastIndexOf(',')+1,str.lastIndexOf('H')-1);
  //Serial.println("X:" + x + ", Y:" + y);
  walkMode == true ? joystickWalk(x.toInt(), y.toInt()) : joystickRoll(x.toInt(), y.toInt());
}
