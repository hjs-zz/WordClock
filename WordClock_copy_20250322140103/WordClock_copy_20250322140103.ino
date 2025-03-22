#include <Adafruit_NeoPixel.h>
#include <DS3231.h>
#include <avr/power.h>
#include <Wire.h>
#include <Timezone.h>
#include <EEPROM.h>

//Pin Definitionen
const int Taster = 2; //Interrupt Pin -> Taster
#define PIN 8 // LED Driver

//Interrupt Definitionen
volatile bool ledState = LOW;
int color_state = EEPROM.read(0);
unsigned long lastInterruptTime = 0;

RTClib myRTC;
DS3231 Clock;

//Farben
byte Colour_R, Colour_G, Colour_B = 255;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(110, PIN, NEO_GRB + NEO_KHZ800);

//Statemachine
int state = 0;
int actual_state = 99;

//LED Array
bool light[110];
const int horiz_null[10] =  {10,11,32,33,54,55,76,77,98,99};


TimeChangeRule GSZ = { "GSZ", Last, Sun, Mar, 2, -120 };  //UTC hours
TimeChangeRule GWZ = { "GWZ", Last, Sun, Oct, 3, -60 };   //UTC + 1 hours
Timezone GER(GSZ, GWZ);
int t_minute = 20;
int t_hour = 5;

void setup() {
  pixels.begin();
  Change_Colour(color_state);
  Serial.begin(57600);
  Wire.begin();
  delay(500);
  Serial.println("Arduino Ready!");
  pinMode(Taster, INPUT_PULLUP);  // Taster als Eingang
  attachInterrupt(digitalPinToInterrupt(Taster), Interrupt_Handler, FALLING);
  //setRTCFromCompileTime();
  //Sommer & Winterzeit
}

//DEBUG FUNCTIONS
void printRTCtime() {
  Serial.print("Datum: ");
  Serial.print(Clock.getYear() + 2000);
  Serial.print("/");
  bool century = false;
  Serial.print(Clock.getMonth(century));
  Serial.print("/");
  Serial.print(Clock.getDate());
  Serial.print(" ");

  Serial.print("Zeit: ");
  Serial.print(Clock.getHour(century, century));
  Serial.print(":");
  Serial.print(Clock.getMinute());
  Serial.print(":");
  Serial.print(Clock.getSecond());
  Serial.println();
}
void print_array(bool array[]){
    Serial.println("Print Array");

  for (int i=10;i>-1;i--){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=11;i<22;i++){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=32;i>21;i--){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=33;i<44;i++){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=54;i>43;i--){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=55;i<66;i++){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=76;i>65;i--){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=77;i<88;i++){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=98;i>87;i--){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  for (int i=99;i<110;i++){
    Serial.print(String(array[i])+ " ");
  }
  Serial.println("");
  Serial.println("Print Done");
}
void setRTCFromCompileTime() {
  int year, month, day, hour, minute, second;

  // Datum aus Makro __DATE__ (Format: "Mar 18 2025")
  char monthStr[4];
  sscanf(__DATE__, "%s %d %d", monthStr, &day, &year);

  // Zeit aus Makro __TIME__ (Format: "12:34:56")
  sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);

  // Konvertiere Monats-String in Zahl
  const char* months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
  for (month = 0; month < 12; month++) {
    if (strncmp(monthStr, months[month], 3) == 0) {
      month++;  // Monate starten bei 1
      break;
    }
  }

  // RTC setzen
  Clock.setClockMode(false);   // 24-Stunden-Format
  Clock.setYear(year - 2000);  // RTC erwartet Jahr ab 2000
  Clock.setMonth(month);
  Clock.setDate(day);
  Clock.setHour(hour);
  Clock.setMinute(minute);
  Clock.setSecond(second);
  Serial.println("RTC mit Kompilierungszeit gesetzt!");
}

//Übergänge
void Laufschrift(bool light[]){
  //Alles Schwarz stellen
  int i_delay = 150;
  LED_OFF();

  //Jeden Charakter durchgehen, falls erforderlich auf Hell stellen
  for (int i=10;i>-1;i--){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=11;i<22;i++){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=32;i>21;i--){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=33;i<44;i++){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=54;i>43;i--){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=55;i<66;i++){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=76;i>65;i--){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=77;i<88;i++){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=98;i>87;i--){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
  for (int i=99;i<110;i++){
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
    }
  }
}
void Horizontal(bool light[]){
  int i_delay = 60;
  int a[10] =  {10,11,32,33,54,55,76,77,98,99};
  int b[10] =  {10,11,32,33,54,55,76,77,98,99};
  LED_OFF();
  for (int j = 0; j<10 ;j++){
        pixels.setPixelColor(a[j], pixels.Color(Colour_R, Colour_G, Colour_B));
      }
  pixels.show();
  delay(i_delay);
  for (int j = 0; j<10 ;j++){
        pixels.setPixelColor(a[j], pixels.Color(0,0,0));
      }
      
  for (int i= 1; i<11;i++){
    //Horizontale Rot färben  
      for (int j = 0; j < 10; j++) {
        if (j % 2 == 0) {
          a[j] = horiz_null[j]-i;  // Beispielberechnung für gerade Indizes
          b[j] = horiz_null[j]-(i-1); 
        } else {
          a[j] = horiz_null[j]+i;  // Beispielberechnung für ungerade Indizes
          b[j] = horiz_null[j]+(i-1);
        }
      }

      for (int j = 0; j<10 ;j++){
        pixels.setPixelColor(a[j], pixels.Color(Colour_R, Colour_G, Colour_B));
      }
      for (int j = 0; j<10 ;j++){
        if(light[b[j]]==true){
            pixels.setPixelColor(b[j], pixels.Color(Colour_R, Colour_G, Colour_B));
        }
        else{
          pixels.setPixelColor(b[j], pixels.Color(0, 0, 0));
        }
      }
      pixels.show();
      delay(i_delay);
      for (int j = 0; j<10 ;j++){
        pixels.setPixelColor(a[j], pixels.Color(0,0,0));
      }
      pixels.show();
    //
  }
  int c[10] =  {0,21,22,43,44,65,66,87,88,109};
  for (int j = 0; j<10 ;j++){
        if(light[c[j]]==true){
            pixels.setPixelColor(c[j], pixels.Color(Colour_R, Colour_G, Colour_B));
        }
        else{
          pixels.setPixelColor(c[j], pixels.Color(0, 0, 0));
        }
      }
    pixels.show();
}
void Simple(bool light[]){
  LED_OFF();
  for (int i=0;i<110;i++){
    if(light[i]==true){
            pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
        }
  }
  pixels.show();
}
void Snake(bool light[]){
  //Alles Schwarz stellen
  int i_delay = 30;
  LED_OFF();

  //Jeden Charakter durchgehen, falls erforderlich auf Hell stellen
  for (int i=10;i>-1;i--){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
  for (int i=11;i<22;i++){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=32;i>21;i--){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=33;i<44;i++){
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      pixels.show();
      delay(i_delay);
      if (light[i] == true) {
        pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
      }
      else{
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
      pixels.show();
  }

  for (int i=54;i>43;i--){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=55;i<66;i++){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=76;i>65;i--){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=77;i<88;i++){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=98;i>87;i--){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

  for (int i=99;i<110;i++){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }

}
void Schreiber(bool light[]){
  int i_delay = 30;
  LED_OFF();
  //Jeden Charakter durchgehen, falls erforderlich auf Hell stellen
  for (int i=0;i<110;i++){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
    delay(i_delay);
    if (light[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();
  }
}
void Shuffle(bool light[]){
  int i_delay = 100;
  LED_OFF();

  int array[110];
  int arraySize = 0;
  int sum = 0;  // Variable für die Summe
  // Schleife, um alle Elemente des Arrays zu durchlaufen und zu addieren
  for (int i = 0; i < 110; i++) {
    if(light[i]==true){
      array[arraySize] = i;
      arraySize++;  // Addiere das aktuelle Element zur Summe
    }
  }
  //Random
  for (int i = arraySize - 1; i > 0; i--) {
    // Wähle einen zufälligen Index
    int j = random(0, i + 1);
    
    // Tausche die Elemente an den Positionen i und j
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
  Serial.println("Array Größe " + String(arraySize));


  for (int i=0; i<arraySize;i++){
    delay(i_delay);
    pixels.setPixelColor(array[i], pixels.Color(Colour_R, Colour_G, Colour_B));
    pixels.show();
  }
}
void ReverseShuffle(bool light[]){
  int i_delay = 20;
  for (int i=0; i < 110;i++){
    pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
  }
  pixels.show();

  int array[110];
  int arraySize = 0;
  int sum = 0;  // Variable für die Summe
  // Schleife, um alle Elemente des Arrays zu durchlaufen und zu addieren
  for (int i = 0; i < 110; i++) {
    if(light[i]==false){
      array[arraySize] = i;
      arraySize++;  // Addiere das aktuelle Element zur Summe
    }
  }
  //Random
  for (int i = arraySize - 1; i > 0; i--) {
    // Wähle einen zufälligen Index
    int j = random(0, i + 1);
    
    // Tausche die Elemente an den Positionen i und j
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }

  for (int i=0; i<arraySize;i++){
    delay(i_delay);
    pixels.setPixelColor(array[i], pixels.Color(0, 0, 0));
    pixels.show();
  }
}
//PROD FUNCTIONS
void Interrupt_Handler() {
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 1000) {
    color_state = (color_state + 1) % 4;
    EEPROM.write(0, color_state);
    lastInterruptTime = interruptTime;
  }
  Change_Colour(color_state);
}
void Change_Colour(int color_state) {
  switch (color_state) {
    case 0: //WHITE
      Colour_R = Colour_G = Colour_B = 255;
      break;
    case 1: //RED
      Colour_R = 255;
      Colour_G = Colour_B = 0;
      break;
    case 2: // GREEN
      Colour_G = 255;
      Colour_R = Colour_B = 0;
      break;
    case 3: //BLUE
      Colour_R = Colour_G = 0;
      Colour_B = 255;
      break;
    default:
      Colour_R = Colour_G = Colour_B = 255;
      break;
  }
}
void update(bool array[]) {
  for (int i = 0; i < 110; i++) {
    if (array[i] == true) {
      pixels.setPixelColor(i, pixels.Color(Colour_R, Colour_G, Colour_B));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));;
    }
  }
  pixels.show();
}
void LED_OFF(){
  for (int i=0; i < 110;i++){
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
}
void all_black(bool light[]){
  for (int i = 0; i < 110; i++) {
    light[i] = false;
  }
}
bool isSummerTime(time_t unixTime) {
  tmElements_t timeinfo;
  breakTime(unixTime, timeinfo);  // Unixtime in Struktur umwandeln

  int year = timeinfo.Year + 1970;  // Jahr bestimmen
  int month = timeinfo.Month;
  int day = timeinfo.Day;
  int weekday = timeinfo.Wday;  // Wochentag (Sonntag = 1)

  // Start der Sommerzeit: Letzter Sonntag im März
  if (month == 3) {
    int lastSunday = getLastSunday(3, year);
    return (day > lastSunday || (day == lastSunday && timeinfo.Hour >= 2));
  }

  // Ende der Sommerzeit: Letzter Sonntag im Oktober
  if (month == 10) {
    int lastSunday = getLastSunday(10, year);
    return (day < lastSunday || (day == lastSunday && timeinfo.Hour < 2));
  }

  // Sommerzeit gilt zwischen April und September
  return (month > 3 && month < 10);
}
int getLastSunday(int month, int year) {
  tmElements_t t;
  t.Day = 31;  // Gehe vom letzten Tag des Monats aus
  t.Month = month;
  t.Year = year - 1970;  // tmElements_t verwendet Jahre ab 1970
  t.Hour = 0;
  t.Minute = 0;
  t.Second = 0;

  time_t time = makeTime(t);
  breakTime(time, t);  // Um sicherzugehen, dass der Monat stimmt

  while (t.Wday != 1) {  // Rückwärtsgehen, bis ein Sonntag gefunden wird
    time -= 86400;       // Ein Tag in Sekunden
    breakTime(time, t);
  }

  return t.Day;  // Tag des letzten Sonntags zurückgeben
}
void turnOnLights(bool light[], int start, int end) {
    for (int i = start; i < end; i++) {
        light[i] = true;
    }
}
int set_min(bool array[], int t_minute){
    int state;
      if (t_minute < 3) {
        state = 0;
        turnOnLights(light, 106, 109);  // Nach
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 3 && t_minute < 8) {
        state = 1;
        turnOnLights(light, 0, 4);      // Fünf
        turnOnLights(light, 35, 39);    // Nach
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 8 && t_minute < 13) {
        state = 2;
        turnOnLights(light, 35, 39);    // Nach
        turnOnLights(light, 11, 15);    // Zehn
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 13 && t_minute < 18) {
        state = 3;
        turnOnLights(light, 35, 39);    // Nach
        turnOnLights(light, 22, 29);    // Viertel
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 18 && t_minute < 23) {
        state = 4;
        turnOnLights(light, 35, 39);    // Nach
        turnOnLights(light, 15, 22);    // Zwanzig
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 23 && t_minute < 28) {
        state = 5;
        turnOnLights(light, 0, 4);      // Fünf
        turnOnLights(light, 39, 42);    // Vor
        turnOnLights(light, 51, 55);    // Halb
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 28 && t_minute < 33) {
        state = 6;
        turnOnLights(light, 51, 55);    // Halb
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 33 && t_minute < 38) {
        state = 7;
        turnOnLights(light, 0, 4);      // Fünf
        turnOnLights(light, 35, 39);    // Nach
        turnOnLights(light, 51, 55);    // Halb
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 38 && t_minute < 43) {
        state = 8;
        turnOnLights(light, 11, 15);    // Zehn
        turnOnLights(light, 35, 39);    // Nach
        turnOnLights(light, 51, 55);    // Halb
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 43 && t_minute < 48) {
        state = 9;
        turnOnLights(light, 22, 29);    // Viertel
        turnOnLights(light, 39, 42);    // Vor
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 48 && t_minute < 53) {
        state = 10;
        turnOnLights(light, 39, 42);    // Vor
        turnOnLights(light, 11, 15);    // Zehn
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else if (t_minute >= 53 && t_minute < 58) {
        state = 11;
        turnOnLights(light, 39, 42);    // Vor
        turnOnLights(light, 0, 4);      // Fünf
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    } else {
        state = 0;
        turnOnLights(light, 106, 109);  // Nach
        turnOnLights(light, 5, 8);      // Ist
        turnOnLights(light, 9, 11);     // Es
    }
  return state;
}

void loop() {
  all_black(light);
  DateTime now = myRTC.now();
  bool t_summer = isSummerTime(now.unixtime());

  t_minute = now.minute();
  t_hour = now.hour();
  //printRTCtime();
  
  //t_minute Manipulation
  state = set_min(light, t_minute);

  //Stunden Manipulation
    if (t_summer) {
      t_hour = (t_hour % 24) + 1;  // Ensures it wraps correctly
      if (t_hour == 25) t_hour = 1;
    }
    if (t_minute > 22) {
        t_hour = t_hour+1;
    }
    t_hour = (t_hour > 12) ? t_hour - 12 : t_hour;  


  switch (t_hour) {
    case 0: turnOnLights(light, 45, 50); break;
    case 1: turnOnLights(light, 57, 61); break;
    case 2: turnOnLights(light, 55, 59); break;
    case 3: turnOnLights(light, 72, 76); break;
    case 4: turnOnLights(light, 84, 88); break;
    case 5: turnOnLights(light, 66, 70); break;
    case 6: turnOnLights(light, 100, 105); break;
    case 7: turnOnLights(light, 60, 66); break;
    case 8: turnOnLights(light, 94, 98); break;
    case 9: turnOnLights(light, 80, 84); break;
    case 10: turnOnLights(light, 90, 94); break;
    case 11: turnOnLights(light, 77, 80); break;
    default: turnOnLights(light, 45, 50); break;
  }

  if(state != actual_state){
    int randomNum = random(1, 8);
      switch (randomNum){
        case 1: Laufschrift(light); break;
        case 2: Horizontal(light); break;
        case 3: Simple(light); break;
        case 4: Snake(light); break;
        case 5: Schreiber(light); break;
        case 6: Shuffle(light); break;
        case 7: ReverseShuffle(light); break;
      }
    actual_state = state;
  }
  delay(10000);
}