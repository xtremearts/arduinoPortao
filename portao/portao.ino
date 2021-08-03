//Conversão da pinagem Wemos D1 para generic esp8266
//D0 = 16;
//D1 = 5;
//D2 = 4;
//D3 = 0;
//D4 = 2;
//D5 = 14;
//D6 = 12;
//D7 = 13;
//D8 = 15;

byte IN1 = 0;
byte IN2 = 2;
byte D8 = 16;
byte botaoIniciarMotor = 13;

byte sensorMagnetico1 = 12;
byte sensorMagnetico2 = 14;
String estado;
int velocidadePortao = 115;

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#ifndef STASSID
#define STASSID "goku"
#define STAPSK "gokugoku"


// CONFIGURAÇÕES DISPLAY
#define endereco 0x27
#define colunas 16
#define linhas 2
#define ip WiFi.localIP().toString()

#endif

const char *ssid = STASSID;
const char *password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

LiquidCrystal_I2C lcd(endereco, colunas, linhas);

void imprimirLinhas(String msg1, String msg2)
{
  lcd.clear();
  lcd.print(msg1);
  lcd.setCursor(0, 1);
  lcd.print(msg2);
  lcd.setCursor(1, 1);
}

void abrirPortao(){
  while (digitalRead(sensorMagnetico2) == LOW && estado == "fechado")
    {
      analogWrite(D8, velocidadePortao);
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      Serial.println("PORTÃO ABRINDO!");
    }
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    estado = "aberto";
    Serial.println("PORTÃO ABERTO!");
    imprimirLinhas("ABERTO", ip);  
}

void fecharPortao(){
  while (digitalRead(sensorMagnetico1) == HIGH && estado == "fechado")
    {
      analogWrite(D8, velocidadePortao);
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      Serial.println("PORTÃO FECHANDO!");
    }
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, HIGH);
    estado = "aberto";
    Serial.println("FECHADO!");
    imprimirLinhas("FECHADO", ip);
}

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.clear();

  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(botaoIniciarMotor, INPUT);
  pinMode(D8, INPUT);
  pinMode(sensorMagnetico1, INPUT);
  pinMode(sensorMagnetico2, INPUT);

  // Connectando Wifi
  Serial.println();
  Serial.println();
  Serial.print(F("Conectando em "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
    imprimirLinhas("Nome Wifi: " + (String)ssid, "senha: " + (String)password);
  }

  Serial.println();
  Serial.println(F("WiFi Conectado"));

  imprimirLinhas("Digite a url:", ip);

  // Iniciando Servidor
  server.begin();
  Serial.println(F("Servidor Iniciado"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  estado = "fechado";
  
  //Abrindo o portão com o botão
  if (digitalRead(botaoIniciarMotor) == LOW && digitalRead(sensorMagnetico2) == LOW)
  {
    abrirPortao();
  }

  if (digitalRead(botaoIniciarMotor) == LOW && digitalRead(sensorMagnetico2) == HIGH)
  {
    fecharPortao();
  }

  // Checa se algum cliente conectou no IP
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println(F("Novo Cliente web"));
  client.setTimeout(5000); // Para o cliente depois de um determinado tempo

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

 Serial.println(estado);
 if (req.indexOf(F("/gpio/0")) != -1 && digitalRead(sensorMagnetico2) == LOW)
  {
    abrirPortao();
  }
  

  if (req.indexOf(F("/gpio/0")) != -1 && digitalRead(sensorMagnetico1) == HIGH)
  {
    fecharPortao();
  }

  
  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available())
  {
    // byte by byte is not very efficient
    client.read();
  }
  
  client.print(F("<!DOCTYPE html>"));
  client.print(F("<html lang='pt'>"));
  client.print(F("<head>"));
  client.print(F("<meta charset='UTF-8'>"));
  client.print(F("<meta http-equiv='X-UA-Compatible' content='IE=edge'>"));
  client.print(F("<meta name='viewport' content='width=device-width, initial-scale=1.0'>"));
  client.print(F("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css' rel='stylesheet' integrity='sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC' crossorigin='anonymous'>"));
  client.print(F("<link rel='icon' type='image/x-icon' href='https://www.unidesc.edu.br/wp-content/uploads/2017/02/favicon1.png'>"));
  client.print(F("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap-icons@1.5.0/font/bootstrap-icons.css'>"));
  client.print(F("<title>Portão Unidesc</title>"));
  client.print(F("</head>"));
  client.print(F("<body>"));
  client.print(F("<div class='container'>"));
  client.print(F("<div class='container mt-2 mb-2'>"));
  client.print(F("<div class='row justify-content-center'>"));
  client.print(F("<img class='col-5' src='https://www.unidesc.edu.br/wp-content/uploads/2017/02/logo-site1.png' alt=''>"));
  client.print(F("</div>"));
  client.print(F("</div>"));
  client.print(F("<div class='container' style='margin-top: 50%'>"));
  client.print(F("<a href='/gpio/0' class='row mt-5 mb-2 ml-3 mr-3 border text-decoration-none'>"));
  client.print(F("<button type='button' class='col-12 p-3 btn btn-outline-primary' ><i class='bi bi-play'></i> Aperte</button>"));
  client.print(F("</a>"));
  client.print(F("<p class='text-center text-secondary'>*Aperte o botão para abrir ou fechar o portão</p>"));
  client.print(F("</div>"));
  client.print(F("</div>"));
  client.print(F("<footer class='fixed-bottom container'>"));
  client.print(F("<div class='container'>"));
  client.print(F("<p class='text-center text-secondary'><small>Projeto Integralizador - TADS 2019 - Professor: <a href='https://www.linkedin.com/in/efrain-rodriguez-b235b9b4/'>Efain</a></small></p>"));
  client.print(F("</div>"));
  client.print(F("<div class='row justify-content-center border-top pt-2'>"));
  client.print(F("<p class='text-center col'>"));
  client.print(F("<a class='text-decoration-none' href='https://www.linkedin.com/in/tiago-silva-pereira-84375750/'><i class='bi bi-linkedin'></i> Tiago </a>"));
  client.print(F("</p>"));
  client.print(F("<p class='text-center col'>"));
  client.print(F("<a class='text-decoration-none' href='https://github.com/xtremearts/arduinoPortao/'><i class='bi bi-github'></i> Código Fonte </a>"));
  client.print(F("</p>"));
  client.print(F("</div>"));
  client.print(F("</footer>"));
  client.print(F("</body>"));
  client.print(F("</html>"));
  Serial.println(F("Cliente Desconectado"));
}
