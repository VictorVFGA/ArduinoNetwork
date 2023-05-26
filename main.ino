#include <DHT.h>
#include <DHT_U.h>
#include <EtherCard.h>

static byte myip[] = {192, 168, 1, 68};
static byte gwip[] = {192, 168, 1, 254};
static byte mymac[] = {0x74, 0x69, 0x69, 0x2D, 0x30, 0x31};

byte Ethernet::buffer[500];

int SENSOR = 3;
int TEMPERATURA;
int TEMPF;
int HUMEDAD;
DHT dht(SENSOR, DHT11);

void setup() {
  Serial.begin(19200);
  dht.begin();
  Serial.println("\n[Vuelva pronto]");

  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println("Error al acceder al controlador Ethernet");

  ether.staticSetup(myip, gwip);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
}

static word homePage() {
  BufferFiller bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "<!DOCTYPE html>\r\n"
                    "<html>\r\n"
                    "<head>\r\n"
                    "<title>Sensor Arduino</title>\r\n"
                    "<meta charset='utf-8'>\r\n"
                    "</head>\r\n"
                    "<body>\r\n"
                    "<h1>Datos de sensores del Arduino</h1>\r\n"
                    "<h2>La temperatura es de: %d °C</h2>\r\n"
                    "<h2>La temperatura es de: %d °F</h2>\r\n"
                    "<h2>La humedad es de: %d %%</h2>\r\n"
                    "</body>\r\n"
                    "</html>\r\n"),
               TEMPERATURA, TEMPF, HUMEDAD);

  return bfill.position();
}

void loop() {
  if (ether.packetLoop(ether.packetReceive())) {
    TEMPERATURA = dht.readTemperature();
    HUMEDAD = dht.readHumidity();
    TEMPF = dht.readTemperature(true);
    ether.httpServerReply(homePage());
    delay(2000);
  }
}
