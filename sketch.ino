#include <WiFi.h>
#include <HTTPClient.h>

#define LED_AZUL 9 // Pino utilizado para controle do led azul
#define LED_VERDE 40 // Pino utilizado para controle do led verde
#define LED_VERMELHO 41 // Pino utilizado para controle do led vermelho
#define LED_AMARELO 42 // Pino utilizado para controle do led amarelo

const int PINO_BOTAO = 18; //Pino utilizado para o botão

const int PINO_LDR = 4; //Pino utilizado para o LDR
int LIMIAR_LDR = 600;

// Definição de variáveis auxiliares
int botaoEstado = 0;
int botaoUltimoEstado = 0;
int botaoContador = 0;
unsigned long debounceUltimoTempo = 0;
unsigned long debounceAtraso = 50;

void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);

  // Inicialização das entradas
  pinMode(PINO_BOTAO, INPUT); 

  // Inicia os LEDs como desligados
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AMARELO, LOW);

  Serial.begin(9600); // Configuração do serial entre ESP e computador de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando o loop, mostrar que o esp está conectado ao wifi.

}

void loop() {
  int statusLDR = analogRead(PINO_LDR); // realiza a leitura do sensor LDR

  if(statusLDR <= LIMIAR_LDR){

    // Configuração para ativar o modo noturno e piscar o led amarleo
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_AMARELO, HIGH);
    delay(500);
    Serial.print("Modo noturno ativado!");
    Serial.println(statusLDR);

  }else{

    // Configuração do modo de funcionamdento tradicional do semaforo
    digitalWrite(LED_VERDE, HIGH);
    delay(3000);
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_AMARELO, HIGH);
    delay(2000);
    digitalWrite(LED_AMARELO, LOW);

    digitalWrite(LED_VERMELHO, HIGH);
    lerBotao(); // Verifica o estado do botão no estado "fechado"
    delay(5000);
    digitalWrite(LED_VERMELHO, LOW);
  }
}

void lerBotao (){ //função para realizar a leitura do estado do botão e resolver o debounce
  int leitura = digitalRead(PINO_BOTAO);
  
  if (leitura != botaoUltimoEstado) {
    debounceUltimoTempo = millis ();
  }

  if ((millis() - debounceUltimoTempo) > debounceAtraso) { //solução do debounce
    if (leitura != botaoEstado) {
      botaoEstado = leitura;
      if (botaoEstado == HIGH) {
        botaoContador++;
        Serial.println("O botão está pressionado!");

        if (botaoContador == 1) {
          delay(1000);
          digitalWrite(LED_VERMELHO, LOW);
          digitalWrite(LED_VERDE, HIGH);
          delay(3000);
          digitalWrite(LED_VERDE, LOW);
        }

        if (botaoContador == 3) {
          requisicaoHTTP();
          botaoContador = 0; // zera a contagem
        }
      }
    }
  
  }
}

void requisicaoHTTP (){ //função para enviar a requisição http para conexão
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String servidor = "http://www.google.com.br/";

    http.begin(servidor.c_str());
    int respostaHTTP = http.GET();

    if (respostaHTTP > 0) {
      Serial.print("Segue a resposta HTTP: ");
      Serial.println(respostaHTTP);
      String cargaUtil = http.getString();
      Serial.println(cargaUtil);
    } else {
      Serial.print("Código de erro: ");
      Serial.println(respostaHTTP);
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado");
  }
}