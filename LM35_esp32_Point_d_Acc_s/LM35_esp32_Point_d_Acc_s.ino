#include <WiFi.h>
#include <WebServer.h>

/* Paramètres WiFi créés par l’ESP32 */
const char* ssid = "ESP32_Temperature";     // Nom du réseau à afficher
const char* password = "user123";        // Mot de passe du réseau

/* Adresse IP locale de l’ESP32 */
IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

/* Serveur Web sur le port 80 */
WebServer server(80);

/* Broche du capteur LM35 */
const int LM35_PIN = 33;
float temperature = 0.0;

void setup() {
  Serial.begin(115200);

  // Création du réseau Wi-Fi local
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  Serial.println("Point d'accès Wi-Fi créé");
  Serial.println("Nom du réseau : " + String(ssid));
  Serial.println("Adresse IP : " + WiFi.softAPIP().toString());

  // Routes HTTP
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Serveur HTTP lancé !");
}

void loop() {
  temperature = lireTemperature();  // Met à jour la température
  server.handleClient();           // Gère les requêtes
}

float lireTemperature() {
  int analogValue = analogRead(LM35_PIN);     // Lecture analogique
  float voltage = analogValue * (3.3 / 4095.0); // Tension (3.3V pour ESP32)
  float tempC = voltage * 100.0;              // LM35 : 10 mV/°C
  return tempC;
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML(temperature));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Page non trouvée");
}

String SendHTML(float temp) {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Température LM35</title>";
  html += "<style>body{font-family:Arial;text-align:center;padding-top:50px;}</style>";
  html += "</head><body>";
  html += "<h1>Température Actuelle</h1>";
  html += "<h2>" + String(temp, 2) + " &deg;C</h2>";
  html += "<p>Actualiser la page pour mettre à jour</p>";
  html += "</body></html>";
  return html;
}
