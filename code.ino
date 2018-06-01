//=========================================================== server, dường dẫn, port 
//String my_server="http://gentle-castle-74854.herokuapp.com";
//String my_port="80";
String my_server="192.168.1.154";  //\\\\\
//String my_server="192.168.1.197";  //\\\\\
String my_port="5000";
String uri = "/data"; 
//=========================================================== tên và pass wifi 
String tenwf="Nha Thuoc Manh Ha"; 
String pass="dongocduc"; 
//String tenwf="DCE-KTMT"; 
//String pass="ktmt-dce";
//========================================================== thông số vị trí
String lat="21.03905";
String lng="105.847527";
//========================================================== thông số cho cảm biến bụi đo bụi
int ledPower = 8;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
//int capnhat=300000; //=====================================thời gian cập nhật lên server(5 phút)
int capnhat=10000;
//=========================================================== thư viện DHT11
#include <SimpleDHT.h> //thư viện dht11
int pinDHT11 = 7; //Thiet lap GPIO 7 để đọc DHT11
SimpleDHT11 dht11;
//============================================================ biến toàn cục
byte h;
byte t;
int cambienbui;
//============================================================ đọc cảm biến bụi
void doccambienbui()
{
//  cambienbui=analogRead(A0); // đọc cảm biến bụi chân A0
//    cambienbui=map(cambienbui,0,1023,0,100); //chuyển đổi về phần trăm
  digitalWrite(ledPower,LOW); // bật led phát hồng ngoại điện trở low
  delayMicroseconds(samplingTime); // delay lấy mẫu
  
  cambienbui=analogRead(A0); // đọc cảm biến bụi chân A0
  cambienbui=map(cambienbui,0,1023,0,5000); //chuyển đổi về phần trăm
  cambienbui = (cambienbui)*0.171-103;
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // tắt led hồng ngoại
  delayMicroseconds(sleepTime);
    
}
// khởi tạo kết nối wifi 
void ESP8266_init() // khởi tạo esp8266
{
  Serial.println("AT");delay(100); //gửi AT test SIM
  Serial.println("ATE0");delay(100); // tắt chế độ phản hồi
  Serial.println("AT+CWMODE=1");delay(100);  // chế độ STATION hoạt động như 1 thiết bị wifi
  Serial.println("AT+CIPMUX=0");delay(100);  //chế độ đơn kênh
  String conn=("AT+CWJAP=\"");
         conn+=tenwf;
         conn+="\",\"";
         conn+=pass;
         conn+="\"";
  Serial.println(conn); // kết nối đến wifi
  delay(5000);
}
// ============================================================== chương trình gửi 
int sendSensor()
{  
  int err = SimpleDHTErrSuccess; // err dht
  doccambienbui();
  if ((err = dht11.read(pinDHT11, &t, &h, NULL)) != SimpleDHTErrSuccess)return 1; //đọc cảm biến, nếu Fall thì thoát ra đọc lại
  //gửi dữ liệu lên server
  String data="AT+CIPSTART=\"TCP\",\"";
         data+=my_server;
         data+="\",";
         data+=my_port;
   Serial.println(data);delay(2000);// thuc hien mo cong ket noi tcp len my_server post my_port //A  T+CIPSTART="TCP","192.168.4.1",80
   data="pm_2_5="+String(cambienbui)+"&temperature="+String(t)+"&humidity="+String(h)+"&lat="+lat+"&lng="+lng; 
   String postRequest =
   "POST " + uri + " HTTP/1.1\r\n" +
   "Host: "+ my_server +" \r\n" +   
   "Content-Type: application/x-www-form-urlencoded \r\n" +
   "Content-Length: " + data.length() + "\r\n" +
   "\r\n" + data +"\r\n" ;
  String sendCmd = "AT+CIPSEND=";//determine the number of characters to be sent.
         sendCmd += String(postRequest.length()); //độ dài của data
   Serial.println(sendCmd);delay(500);  
   Serial.print(postRequest);delay(1000); // gửi xong !   /||\\\\//  
   Serial.println(" send data complete ");
   return 0;
}

void setup() 
{
  Serial.begin(115200);
  delay(2000);
  ESP8266_init();
}

void loop() 
{
  int check_fall;
  check_fall=sendSensor();
  if(!check_fall)delay(capnhat); //cập nhật capnhat lần /s (nếu không lỗi)
}
