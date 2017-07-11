#include<String.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };//MAC address

IPAddress ip(192,168,137,20);//
EthernetServer server(80);

String number1="+919448228044";
String number2="+919482513153";
//String number3="+919071108984";
String number3="+918848337736";

String message="";
String num="";

String st="http://maps.google.com/?q=";

SoftwareSerial mySerial(2,3);//(Rx,Tx)

int relaypin1 = 4;
int relaypin2 = 5;
int voltagepin=A0;

float voltage=0.0;
int value1=0;
int value2=0;
int flag=0;
int gpsflag=0;

void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  server.begin();
  mySerial.begin(9600);
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(1000);
  Serial.println(mySerial.readString());
  
  pinMode(relaypin1, OUTPUT);
  pinMode(relaypin2, OUTPUT);
  pinMode(voltagepin, INPUT);
  
  digitalWrite(relaypin1,HIGH);
  digitalWrite(relaypin2,HIGH);
}

void loop()
{ 
  voltage=analogRead(voltagepin);
  voltage=voltage*24/1024.0;
  Serial.print("Voltage:");
  Serial.print(voltage);
  Serial.println(" V");
  ether();  
  gsm();
  if(value1==1 && voltage<0.5 && flag==0)
  {
    //Gpslocate();
    st="POWER FAILURE AT "+ st;
    flag=1;
    SendMessage(st,num);
  } 
  if(voltage>0.5) 
    flag=0; 
}   

void ether()
{
  EthernetClient client;
  String readString;
  client = server.available();
  if (client) 
  {
    while (client.connected()) 
    {   
      voltage=analogRead(voltagepin);
      voltage=voltage*24/1024.0;
      Serial.print("voltage:");
      Serial.print(voltage);
      Serial.println(" V");
      if (client.available()) 
      {
         char c = client.read(); 
         //read char by char HTTP request
         if (readString.length() < 100) 
         {
          //store characters to string
          readString += c;
         }   
         //if HTTP request has ended
         if (c == '\n') 
         {              
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();  
              
           client.println("<HTML>");
           
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Arduino Project</TITLE>");
           client.println("</HEAD>");
           
           client.println("<BODY>");  
                  
           client.println("<br/>");  
           client.println("<H2>IOT BASED POWER GRID MANAGEMENT</H2>"); 
           client.println("VOLTAGE:");
           client.println(voltage);
           client.println(" V");
                              
           client.println("<br />");
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\">Turn On Line 1</a>");
           client.println("<a href=\"/?button1off\"\">Turn Off Line 1</a><br />");   
           client.println("<br />");
           client.println("<br />");  
           client.println("<a href=\"/?button2on\"\">Turn On Line 2</a>");
           client.println("<a href=\"/?button2off\"\">Turn Off Line 2</a><br />");  
           client.println("<br />");    
           
           client.println("</BODY>");
           client.println("<head>");
           client.println("<meta http-equiv='refresh' content='1'>");
           client.println("</head>");
           
           client.println("</HTML>");
     
           //delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
           
           if (readString.indexOf("?button1on") >0)
              value1=1;
           if (readString.indexOf("?button1off") >0)
              value1=0;       
           if (readString.indexOf("?button2on") >0)
              value2=1;
           if (readString.indexOf("?button2off") >0)
              value2=0;           
           process();                  
         }  
      } 
    }     
    readString="";            
  }
}

void process()
{
    if(value1==1)
    {
      digitalWrite(relaypin1,LOW);
      Serial.println("LINE1 ON");
    }
    if(value1==0)
    {
      digitalWrite(relaypin1,HIGH);
      Serial.println("LINE1 OFF");
    }
    if(value2==1)
    {
      digitalWrite(relaypin2,LOW);
      Serial.println("LINE2 ON");
    }
    if(value2==0)
    {
      digitalWrite(relaypin2,HIGH);
      Serial.println("LINE2 OFF");
    }
}

 void gsm()
{
  if (mySerial.available())
  { 
    message="";
    num="";
    String msg=mySerial.readString();
    Serial.println(msg);
    num=msg.substring(9,22);
    Serial.print("Number:");   
    Serial.println(num);   
    if(num==number1 || num==number2 || num==number3) 
    {    
       message=msg.substring(51,msg.length()-2); 
       if(message=="ON1")
       {
          value1=1;
          //SendMessage("LINE1 ON",num);
          Serial.println("LINE1 ON");
       }   
       if(message=="OFF1")  
       { 
          value1=0;
          //SendMessage("LINE1 OFF",num);
          Serial.println("LINE1 OFF");
       }
       
       if(message=="ON2")
       {
          value2=1;
          //SendMessage("LINE2 ON",num);
          Serial.println("LINE2 ON");
       }   
       if(message=="OFF2")  
       { 
          value2=0;
          //SendMessage("LINE2 OFF",num);
          Serial.println("LINE2 OFF");
       }
       process();  
       delay(1000);
    }   
  }
}

void SendMessage(String ms,String nu)
{
  mySerial.println("AT+CMGF=1\r\n"); //Sets the GSM Module in Text Mode
  delay(1000); // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\""+nu+"\"\r\n"); // Replace x with mobile number
  delay(1000);
  mySerial.println(ms);
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

