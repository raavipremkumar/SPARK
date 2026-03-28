#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>
#include <PZEM004Tv30.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------- WIFI ----------------
#define WIFI_SSID "Chaitu"
#define WIFI_PASSWORD "03032006"

// ---------------- FIREBASE (UPDATED FOR SPARK-4446C) ----------------
#define FIREBASE_HOST "spark-4446c-default-rtdb.firebaseio.com"
// NOTE: For legacy token authentication, you need the "Database Secret" from Firebase Console.
// If you are using Google/Email Auth, consider using the modern Firebase-ESP-Client library for better security.
#define FIREBASE_AUTH "REPLACE_WITH_YOUR_DATABASE_SECRET" 

// ---------------- DHT ----------------
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- PZEM ----------------
PZEM004Tv30 pzem(D6, D5);

// ---------------- RELAY ----------------
#define RELAY_PIN D7 // Define the pin connected to your Relay

// ---------------- FIREBASE OBJECTS ----------------
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// ---------------- VARIABLES ----------------
float voltage, current, temperature, humidity;
float power, pf;

double units = 0.0;
double bill = 0.0;
bool loadState = false;

// ---------------- SETUP ----------------
void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Default to OFF

  lcd.init();
  lcd.backlight();

  dht.begin();

  // WiFi Connect
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  // Firebase setup
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// ---------------- MAIN LOOP ----------------
void loop()
{
  // ----------- DHT READ -----------
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("DHT Error!");
    temperature = 0;
    humidity = 0;
  }

  // ----------- PZEM READ -----------
  voltage = pzem.voltage();
  current = pzem.current();
  power   = pzem.power();
  pf      = pzem.pf();

  if (isnan(voltage) || isnan(current))
  {
    Serial.println("PZEM Error!");
    voltage = 0;
    current = 0;
    power = 0;
  }

  // ----------- ENERGY CALCULATION -----------
  double kW = power / 1000.0;
  // Approximating energy for the delay period 
  units += kW * (1.5 / 3600.0); 

  // ----------- BILL CALCULATION -----------
  if (units <= 100)
    bill = units * 3.85;
  else if (units <= 200)
    bill = (100 * 3.85) + ((units - 100) * 5);
  else if (units <= 300)
    bill = (100 * 3.85) + (100 * 5) + ((units - 200) * 7);
  else
    bill = (100 * 3.85) + (100 * 5) + (100 * 7) + ((units - 300) * 10);

  // ----------- SERIAL OUTPUT -----------
  Serial.println("------ DATA ------");
  Serial.print("Temp: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Voltage: "); Serial.print(voltage); Serial.println(" V");
  Serial.print("Current: "); Serial.print(current); Serial.println(" A");
  Serial.print("Power: "); Serial.print(power); Serial.println(" W");
  Serial.print("Units: "); Serial.println(units);
  Serial.print("Bill: Rs "); Serial.println(bill);

  // ----------- LCD DISPLAY -----------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("U:");
  lcd.print(units, 2);
  lcd.print(" B:");
  lcd.print(bill, 0);

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print(" H:");
  lcd.print(humidity, 0);

  // ----------- FIREBASE UPDATE (MATCHING WEBSITE PATHS) -----------
  FirebaseJson energyJson;
  energyJson.set("voltage", voltage);
  energyJson.set("current", current);
  energyJson.set("power", power);
  energyJson.set("energy", units);
  energyJson.set("bill", bill);
  Firebase.updateNode(firebaseData, "/energy", energyJson);

  FirebaseJson envJson;
  envJson.set("temperature", temperature);
  envJson.set("humidity", humidity);
  Firebase.updateNode(firebaseData, "/environment", envJson);

  // ----------- REMOTE CONTROL (LOAD) -----------
  if (Firebase.getBool(firebaseData, "/control/load")) {
    loadState = firebaseData.boolData();
    digitalWrite(RELAY_PIN, loadState ? HIGH : LOW);
    Serial.print("Load state: "); Serial.println(loadState ? "ON" : "OFF");
  }

  delay(1000); 
}
