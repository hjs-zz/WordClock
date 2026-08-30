# WordClock

Arduino-Wortuhr mit 110 NeoPixel-LEDs, DS3231-Echtzeituhr und Taster zur
Farbauswahl.

Der Sketch ist in unabhängige Module für Zeit, Wortlogik und Anzeige
aufgeteilt. Alle Animationen laufen ohne blockierende `delay()`-Aufrufe, damit
Taster und RTC auch während eines Übergangs verarbeitet werden.

## Zeitbasis

Die DS3231 wird in **UTC** geführt. Der Sketch rechnet die Zeit automatisch in
deutsche Ortszeit (MEZ/UTC+1 bzw. MESZ/UTC+2) um. Die Sommerzeit beginnt am
letzten Sonntag im März und endet am letzten Sonntag im Oktober.

Bei einer neuen oder falsch gestellten RTC kann in `setup()` einmalig
`clockService.setFromLocalCompileTime(__DATE__, __TIME__)` aktiviert werden.
Danach die Zeile wieder
auskommentieren und erneut übertragen, damit die RTC bei späteren Neustarts
nicht auf den Kompilierungszeitpunkt zurückgesetzt wird. Die Systemzeit des
Rechners muss dabei auf deutsche Ortszeit eingestellt sein.

Der Sketch prüft beim Start und danach regelmäßig das Oscillator-Stop-Flag der
RTC. Bei einer ungültigen Uhrzeit blinken die vier Eck-LEDs rot. Nach einer
erfolgreichen Initialisierung nimmt die normale Anzeige automatisch ihren
Betrieb wieder auf.

## Bedienung

Der Taster an Pin 2 wechselt zwischen Weiß, Rot, Grün und Blau. Die Auswahl
wird im EEPROM gespeichert.

Die globale LED-Helligkeit ist in `Config.h` standardmäßig auf 64 von 255
begrenzt. Sie sollte nur erhöht werden, wenn Netzteil, Leitungsquerschnitt und
Stromeinspeisung dafür ausgelegt sind.

## Projektstruktur

- `WordClock_copy_20250322140103.ino`: Initialisierung und Hauptschleife
- `Config.h`: Pins, Helligkeit und Zeitintervalle
- `ClockLogic.*`: Übersetzung einer Uhrzeit in LED-Wörter
- `RtcClock.*`: UTC, deutsche Ortszeit und RTC-Zustand
- `LedDisplay.*`: Farben, Fehleranzeige und nicht blockierende Animationen

## Hardware

- Arduino Uno
- DS3231 RTC (SDA: A4, SCL: A5)
- 110 NeoPixel-LEDs (Datenleitung: Pin 8)
- Taster gegen GND (Pin 2 mit internem Pull-up)

Analogpin A0 bleibt unbeschaltet und dient als Zufallsquelle für die
Animationsreihenfolge.

## Offen

- CAD für LED-Platte fertigstellen
- Halterung für RTC in die Deckplatte integrieren
- Anschlussplan um RTC erweitern
- Frontplatte individualisierbar machen
