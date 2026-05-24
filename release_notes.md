# Release Notes

## 4.3.0
### Breaking Changes
- none

### Feature
- NEU: Die Signalverarbeitung eines Logikkanals kann jetzt auch eine Sperre enthalten. Siehe [Sperre](#sperre).

### Bug
- FIX: Interne KO-Verknüpfungen, die sich auf ausgeblendete KO (nicht in der ETS sichtbare KO) bezogen, konnten bei Eingängen, die ihren Zustand bei Stromausfall gesichert haben, konnten fehlerhafte Werte bei KO liefern, die mehr als 1 Byte Datenlänge hatten.

## 4.2.0
### Breaking Changes
- none

### Feature
- NEU: Für das Status-LED Framework gibt es jetzt einen Logikmodul-Status. Dieser blinkt rot, wenn irgendein Kanal eine Rückkopplung hat und abgeschaltet wurde, um eine Telegrammflut auf dem Bus zu vermeiden. Siehe Kommando [logic lim](#kommando-logic-lim---endlosschleifen-erkennung).

### Bug
- none

## 4.1.0
### Breaking Changes
- none

### Feature
- NEU: Die direkte Kommunikation von der ETS zum Gerät (z.B. prüfen von Benutzerformeln) wird jetzt passend zur APDU des Kommunikationskanals vorgenommen. Linienkoppler oder Router zwischen ETS und dem Gerät können die Kommunikation jetzt nicht mehr stören.

### Bug
- FIX: Eine verbundene Zeitschaltuhr (also die 2., 3. etc.) hat die erste Schaltzeit nicht ausgeführt. Das ist jetzt korrigiert.

## 4.0.0
### Breaking Changes
- none

### Feature
- WICHTIG: Die frühere Steuerung von Buzzer und LED wurde aus dem Logikmodul entfernt - zugunsten der neuen Module LED-Status und Buzzer/Vibration. Dies kann zu manuellen Nacharbeiten nach einem Update des Gerätes notwendig machen (siehe [Besonderheiten ab der Version 4.0 bezüglich Update und Konfigurationstransfer](#besonderheiten-ab-der-version-40-bezüglich-update-und-konfigurationstransfer)).
- NEU: Der Status von LEDs kann in der Logik als interner Eingang dienen
- NEU: Das Logikmodul kann jetzt den Ausgang auf Statuskanäle legen und so LEDs steuern

### Bug
- none

## 3.10.0
### Breaking Changes
- none

### Feature
- interne Version, nicht für ein Release vorgesehen. Falls (aus organisatorischen Gründen - bedingt durch den größeren Umbau) doch diese Version in ein Release gelangen sollte, gelten die Einschränkungen, die für Release 4.0 beschrieben wurden.

### Bug
- none

## 3.7.0
### Breaking Changes
- none

### Feature
- NEU: Ausgangs-KO eines Logikkanals kann jetzt manuell ausgeblendet werden. Der automatische Ein-/Ausblendalgorithmus wurde entfernt, weil er nicht alle gewünschten Fälle abdecken konnte. Nach einem Update müssen unerwünschte KO, die früher automatisch ausgeblendet worden sind, manuell ausgeblendet werden. Dies ändert nichts an der Funktionalität der Logik, es geht nur um die Übersichtlichkeit in der ETS.
- NEU: Überarbeitung der Einstellungs-Seiten:
- Jeder interne Eingang hat jetzt eine eigene Einstellungs-Seite
- Die Ausgangsseite wurde in eine Signalverarbeitungs- und eine Ausgangs-Seite aufgeteilt
- Die Signalverarbeitungs-Seite beinhaltet jetzt die eher selten genutzten Funktionen zur Beeinflussung des Ausgangssignals
- Die Ausgangs-Seite ist jetzt kompakter und übersichtlicher
- NEU: Eingangskonverter für DPT3 (Dimmen)
- NEU: Ausgangskonverter für DPT3 (Dimmen)
- NEU: Alle Eingangskonverter können auch als Trigger fungieren (jedes Telegramm triggert die Logik)
- NEU: Die gleiche Applikation sowohl für TP- wie auch für IP-Geräte
- NEU: Stark verbesserte KNX-TP-Kommunikation. Viele Telegramme in kurzer Zeit (nicht zu verwechseln mit hoher Buslast) werden jetzt sowohl sendeseitig wie auch empfangsseitig nahezu verlustfrei bewältigt.
- NEU: Konfigurationstransfer wurde verbessert und vereinfacht
- NEU: Zeitbehandlung für alle Zeitzonen möglich
- NEU: Kommunikationsobjekte des Logikmoduls wurden neu geordnet, um eine bessere Trennung von Common zu erreichen. Folgende KO sind betroffen (Umnummerierung passiert beim Update automatisch):
- KO 4 "Urlaub" -> KO 15
- KO 5 "Welcher Feiertag ist heute" -> KO 16
- KO 6 "Welcher Feiertag ist morgen" -> KO 17
- KO 8 "LED sperren" -> KO 18
- KO 9 "Buzzer sperren" -> KO 19

### Bug
- FIX: DPT2 am Ausgang sendet wieder korrekte Werte, ist wohl in der Vergangenheit durch eine Änderung am KNX-Stack kaputt gegangen und wurde erst jetzt gemerkt.
- FIX: Durch die Umgehung eines Bugs in der ETS wird partielles programmieren jetzt immer so schnell wie möglich durchgeführt

## 3.6.0
### Breaking Changes
- none

### Feature
- NEU: Weltkindertag (20. September) als Feiertag zugefügt

### Bug
- FIX: Wenn der Default für initial sichbare Kanäle größer war als die max. verfügbare Kanäle, hat sich das Logikmodul aufgehängt (Fix ist nur für developer wichtig, nicht für Enduser)

## 3.5.0
### Breaking Changes
- none

### Feature
- NEU: Firmware-Update über den KNX-Bus lastet den Bus weniger aus (weniger Telegrammwiederholungen)
- NEU: Die Erzeugung der ETS-Produktdatenbank (knxprod) funktioniert jetzt auch mit der ETS 6.3

### Bug
- FIX: Hysterese/Differenzhysterese mit invertiertem Eingang führte beim Schreiben von Werten im Hysteresebereich zu Toggeln des Ausgangs.
- FIX: Das neue Sendeverhalten am Ausgang und der Wiederholungsfilter haben sich leider gegenseitig beeinflusst und zu unerwarteten Ergebnissen geführt, die Logiken kaputt machen konnten.

## 3.4.0
### Breaking Changes
- none

### Feature
- NEU: Standardfunktion A = ABS(E1) (Betrag von E1) und A = ABS(E2) (Betrag von E2)
- NEU: Standardfunktion A = -E1 (Negiere E1) und A = -E2 (Negiere E2)
- NEU: Sendeverhalten für den Ausgang: Wertwiederholungen können jetzt unterdrückt werden
- NEU: Alle internen KO-Verknüpfungen können auch relativ zum eigenen KO angegeben werden
- NEU: Alle internen Eingänge können auch relativ zum eigenen Kanal angegeben werden
- NEU: Die Feiertage Totensonntag und Schweizer Nationalfeiertag sind neu
- NEU: Zeitschaltuhren können jetzt neben einem binären Wert auch einen Zahlenwert (1 Byte) enthalten, der als "Wert vom Eingang 1" im Ausgangskonverter genutzt werden kann [Zeitschaltuhr: Wert (als E1)](#spalte-wert-als-e1)
- NEU: Zeitschaltuhren können jetzt über mehrere Logikkanäle verbunden werden (sowohl relativ wie auch absolut). Sie werden dann wie eine Zeitschaltuhr behandelt. Das ist vor allem wichtig, wenn man bei Neustart Schaltzeiten nachholen will, erlaubt aber auch mehr als 8 Schaltzeiten pro Zeitschaltuhr [Schaltzeiten fortsetzen von Kanal](#schaltzeiten-fortsetzen-von-kanal)
- NEU: Jahres- und Tageszeitschaltuhren können verbunden werden und somit wie eine Zeitschaltuhr fungieren. Ermöglicht eine Kombination von Tages- und Jahresschaltungen in einer Zeitschaltuhr.
- NEU: Interne Eingänge können jetzt auch als Trigger definiert werden [Internen Eingang als Trigger nutzen](#internen-eingang-als-trigger-nutzen-ist-immer-logisch-ein)
- NEU: Alle Firmware-Varianten haben ein neues Script zum KNX-Firmware-Update bekommen, das einen Upload über den KNX-Bus erlaubt.

### Bug
- FIX: Zeitschaltuhren mit "Feiertage wie Sonntage behandeln" haben (leider) nicht immer zu den Sonntags-Schaltzeiten geschaltet.
- FIX: Zeitschaltuhren (und verbundene Zeitschaltuhren) werden jetzt korrekt in der "Übersicht interner Verknüpfungen" angezeigt.
- FIX: Bei Zeitschaltuhren, die im Urlaub schalten sollen, kann man nicht mehr die Einstellung "Schaltzeiten nachholen" vornehmen. Das Feld was schon immer funktionslos, da noch nie Urlaubs-Schaltzeiten nachgeholt werden konnten.

## 3.3.0
### Breaking Changes
- none

### Feature
- NEU: [Benutzerformeln](#benutzerformeln) sind nun in der ETS-Applikation definierbar, dies löst mittelfristig die bisher verfügbaren [Benutzerfunktionen](#benutzerfunktionen) ab.
- NEU: Es kann nun in der ETS getestet werden, ob die Ergebnisse von Benutzerformeln korrekt sind.
- NEU: Die mathematische Funktion "Potenz" ist jetzt bei den Standardformeln auch verfügbar.
- NEU: Durch die Logik verursachte Endlosschleifen werden jetzt erkannt und die entsprechenden Logikkanäle deaktiviert (siehe [Endlosschleifen-Erkennung](#endlosschleifen-erkennung))
- NEU: Es sind neue mathematische Funktionen hinzugekommen, die Logikkanäle vereinfachen können (siehe [Standardformeln](#standardformeln))
- Inkrementieren und Dekrementieren, damit der Aufbau von Zählern einfacher wird.
- Erhöhen und erniedrigen des Ausgangs um Eingangswerte
- Bit-Shifts um 1-Bit
- Min/Max unter Einbeziehung des Ausgangs

### Bug
- FIX: Ausgangskonverter Zahl->String (irgendein Zahl-DPT nach DPT16) ist jetzt implementiert.

## 3.2.0
### Breaking Changes
- none

### Feature
- NEU: Übersichtsseite mit allen internen KO-Verknüpfungen hinzugefügt
- Logik verwendet jetzt den neusten KNX-Stack, der auch bei hoher momentaner Buslast keine Telegramme mehr verpassen kann. Somit ist die Robustheit der Logik nochmal verbessert worden

### Bug
- FIX: TOR wurde nicht korrekt getriggert, wenn nur Eingang 2 als Trigger ausgewählt worden ist
- FIX: Differenzhysterese funktionierte seit der Einführung von DPT12, DPT13 und DPT14 nicht mehr

## 3.1.0
### Breaking Changes
- none

### Feature
- NEU: Der Befehl "logic help" ist jetzt nicht nur in der Console, sondern auch im Diagnoseobjekt verfügbar.
- ACHTUNG: Falls ein Update einer früheren Firmware versagt, liegt das daran, dass bei einer internen KO-Verbindung vom Eingang 2 eines Logikkanals auch das "eigene" KO des Eingang 2 mit einer GA belegt wurde. Der Eingang 2 ist definiert als: Externe KO-Verknüpfung, DPT5.001, Eingangskonverter ist ungleich "Wertintervall".
- ACHTUNG: Nach einem Update müssen alle Zeitschaltuhren, deren Schaltzeiten durch "Sonnenauf-/-untergang plus/minus Zeitversatz" definiert sind, neu definiert werden! Der Stunden-Zeitversatz geht durch das Update verloren und steht auf 0 Stunden.
- NEU: Ein Logikausgang kann jetzt auch seinen Wert zusätzlich über ein internes KO versenden, siehe [Zusätzlich senden](#wert-für-ein-an-ein-zusätzliches-ko-senden)
- NEU: Jede Logik kann jetzt auch einen mehrzeiligen Langtext (bis 512 Zeichen) als Kommentar enthalten, siehe [Kommentar](#kommentar)
- NEU: Ein Eingangskonverter für DPT1 erlaubt jetzt auch die Auswertung eines DPT1-Telegramms als Trigger, siehe [Eingangskonverter DPT1](#dpt-1xxx-schalten)
- NEU: Die Anzahl der verfügbaren Kanäle kann jetzt in der Applikation reduziert werden. Dies dient der Übersicht, siehe [Verfügbare Kanäle](#verfügbare-kanäle)

### Bug
- FIX: Zeitschaltuhren haben bei hoher Auslastung des Gerätes sporadisch ihre Schaltzeiten "verpasst". Das ist korrigiert.
- FIX: Wenn man Befehle im Diagnosemodus falsch oder unvollständig geschrieben hat, konnte das zum Aufhängen des Gerätes führen.
- FIX: Man kann bei Logiken einstellen, dass sie erst ausgewertet werden, wenn alle Eingänge gültig sind. Die Prüfung, dass ein Eingang gültig ist, konnte schon erfolgreich sein, obwohl noch keine Antwort von einem ReadRequest eingegangen ist. Dies war ein seltener Fehler, der jetzt korrigiert ist.
- FIX: Die Prüfung, ob ein Eingang einen gültigen Wert hat, konnte bei komplexeren Logiken versagen. Auch dies ist jetzt korrigiert.

## 3.0.0
### Breaking Changes
- none

### Feature
- NEU: Die Firmware kann jetzt über den KNX-Bus aktualisiert werden (nur bei RP2040-Prozessor aka Raspberry Pi Pico)
- NEU: Die Firmware kann jetzt auch auf reinen IP-Geräten eingesetzt werden und als Logikmodul-IP genutzt werden
- NEU: Output Converter "Wert eines KO senden" eingeführt
- NEU: Zusätzliche Infoboxen, die erklären, wie "nur bei geändertem Ergebnis" zu verstehen ist.

### Bug
- FIX: Textanpassung von "Kanalausgang X/Y zu Interner Eingang 3/4"
- FIX: Zeitschaltuhren Sonnenauf-/-untergang mit Zeitversatz erlauben jetzt nur einen Zeitversatz von &pm;6 Stunden und 59 Minuten. Der früher mögliche Zeitbereich konnte nicht funktionieren.
- FIX: Bei internen KO-Verknüpfungen konnte man immer noch "Vom Bus zyklisch lesen" einstellen. Diese Option ist an der stelle falsch und kann nicht mehr ausgewählt werden.

## 1.5.0
### Breaking Changes
- none

### Feature
- NEU: TOR hat jetzt einen Tri-State-Eingang zum öffnen vom Tor: Das Tor ist beim Neustart weder offen noch geschlossen und agiert somit beim ersten Telegramm erwartungskonform
- NEU: TOR ist bei Neustart am Ausgang undefiniert und wird mit dem ersten öffnen/schließen erst initialisiert.
- NEU: Mathematische Funktion "Glättung" von Werten eingeführt

### Bug
- FIX: Differenzintervall-/Differenzhysterese-Eingänge haben keine ReadRequests nach einem Neustart verschickt, wenn bei einem der beiden Eingänge ein Telegramm empfangen worden ist.
- FIX: Zeitschaltuhren Sonnenauf-/untergang mit Zeitversatz konnten intern zu ungültigen Zeiten führen und schalteten dann unerwartet oder gar nicht.
- FIX: Beim nachholen von Schaltzeiten wurde die Sommerzeit nicht beachtet.
- FIX: Der KNX-Stack ist jetzt wesentlich robuster bei hoch ausgelastetem KNX-Bus. Das hat direkte Auswirkungen auf die Logik, die früher bei Hochlast Telegramme ausgelassen wurden, die dann als Trigger für Logiken fehlten.
- FIX: Einige wenige DPT9-Werte ungleich 0 wurden vom KNX-Stack als 0 gesendet. Das ist gelöst. Es waren Werte der Form &pm;(2<sup>n</sup>)/100 für n>10, also z.B. &pm;20.48, &pm;40.96, &pm;81.92, &pm;163.84, &pm;327.68, &pm;655.36 usw.

## 1.4.0
### Breaking Changes
- none

### Feature
- Überflüssige Libraries entfernt, keine funktionalen Änderungen.
- NEU: Zeitschaltuhren können jetzt auch anhand des Sonnenstands (Elevation, Winkel unter/über dem Horizont) schalten.

### Bug
- none

## 1.3.0
### Breaking Changes
- none

### Feature
- NEU: Sommerzeit kann nicht nur über ein KO gesetzt oder intern berechnet werden (gilt nur für Deutschland), sondern auch aus einem DPT 19-Telegramm ermittelt werden, sofern der KNX-Zeitgeber das unterstützt.

### Bug
- FIX: Ungültige Datum- und Zeit-Telegramme werden ignoriert und stellen die Zeit nicht mehr auf unsinnige Werte
- FIX: Ein Datum mit einem Jahr vor 2022 wird ignoriert, das hilft, wenn beim System-Neustart erst mal veraltete Datum-Telegramme verschickt werden

## 1.2.0
### Breaking Changes
- none

### Feature
- NEU: Datum und Zeit kann jetzt auch über DPT 19 (Datum/Zeit kombiniert) an das Logikmodul übertragen werden (danke an Cornelius Köpp).
- NEU: Es werden alle Zeitzonen für die Berechnung des Sonnenauf- und -untergangs unterstützt.
- NEU: Neues Kommunikationsobjekt, mit dem man dem Logikmodul mitteilen kann, dass jetzt Sommerzeit aktiv ist (für Länder außerhalb Deutschlands notwendig).

### Bug
- FIX: Die Berechnung vom Sonnenauf- und -untergang ist jetzt korrekt (danke an @dhb2002)
- FIX: Das Logikmodul (standalone) hat keine Leseanfragen für Zeit/Datum wiederholt, wenn die erste Leseanfrage nicht beantwortet wurde.
- FIX: Das senden von Feiertagen auf den Bus wurde nicht durchgeführt (auch wenn in der Applikation eingestellt). Sie konnten aber schon immer per Read-Request gelesen werden (danke an Cornelius Köpp für den Fix).
- FIX: Formelergebnisse, die als DPT 5.001 versendet werden sollten, waren falsch (neuer Bug in 1.1).

## 1.1.0
### Breaking Changes
- none

### Feature
- NEU: Es werden auch die DPT 12, 13 und 14 (4-Byte-Werte) sowohl am Ein- wie am Ausgang unterstützt.

### Bug
- FIX: Konstante Eingänge werden wieder als Gültig betrachtet (war ein neuer Bug in 1.0).

## 1.0.0
### Breaking Changes
- none

### Feature
- NEU: Kein EEPROM mehr nötig, KO-Werte werden im Flash gespeichert
- NEU: Das Logikmodul kann jetzt auch ein "Gerät zurücksetzen" an sich selbst senden (also mit der eigenen PA).
- NEU: Technisch ist es notwendig, dass die **minimale** Zeit, bis das Gerät nach einem Neustart aktiv wird, auf 1 Sekunde erhöht wird (früher 0). Die 0 wurde in der Vergangenheit sowieso nicht erreicht, lag aber bei < 500 ms.
- NEU: Im Flash gespeicherte KO (Eingänge) können ihre Werte nicht nur über einen Ausgang, sondern auch sendend bereitstellen

### Bug
- FIX: DPT9 in Formeln wird jetzt korrekt berechnet, bisher konnte es passieren, dass der Wert um Faktor 10 oder gar 100 zu groß wurde.
- FIX: Binärfunktionen (Bit-AND, Bit-OR, ...) rechnen jetzt korrekt. Bisher konnte es durch eine interne Wandlung zu einer Fliesskommazahl zu Rundungsproblemen und damit zu Bitverschiebungen kommen und dadurch zu falschen Ergebnissen.

## 0.13.0
### Breaking Changes
- none

### Feature
- NEU: Eine neue [Formel "B2I (Bool zu Int)"](#a--b2ie1-e2-bool-zu-int) erlaubt die Umrechnung von 2 Einzelbits in einen Wert 0-3 bzw. Szene 1-4.
- NEU: Weitere Hardware verfügbar ([Siehe Unterstützte Hardware](#unterstützte-hardware))

### Bug
- FIX: Die KO-Nummer für interne Verbindungen war nur auf 3 Stellen beschränkt. Es gibt inzwischen aber Applikationen, die über 1000 KO haben.

## 0.12.0
### Breaking Changes
- none

### Feature
- Es gibt keinen funktionalen Unterschied zwischen 0.12.0 und 0.12.1 und die Applikationen sind identisch. Wer 0.12.0 verwendet, muss kein Upgrade machen. Wer 0.11 verwendet, sollte ein Update auf 0.12.1 machen.
- NEU: Interne Ausgänge (als Quelle für die X- und Y-Eingänge eines Logikkanals) können jetzt neben EIN- und AUS-Werten auch nur EIN- oder nur AUS-Werte weiterleiten. Siehe [Interne Eingänge](#interne-eingänge).
- NEU: Zu den mathematischen Funktionen, die Ausgangswerte berechnen können, ist jetzt die Funktion **% (Modulo)**, also Rest-Division, hinzugekommen.
- NEU: Neben den mathematischen Funktionen, die Ausgangswerte berechnen können, sind jetzt auch Bitoperationen hinzugekommen. Siehe [Standardformeln](#standardformeln). Es gibt jetzt
- **& (Bit-Und)**,
- **| (Bit-Oder)**,
- **^ (Bit-Exklusiv-Oder)**,
- **<< (Bit-Links-Verschiebung)**,
- **>> (Bit-Rechts-Verschiebung)**
- Die Zeitbasis für Zeitschaltuhren ist jetzt genauer, die Zeitschaltuhren driften jetzt weniger.

### Bug
- FIX: Treppenlicht konnte erst über KNX wieder abgeschaltet werden, sobald das Logikmodul länger lief als die eingestellte Treppenlichtzeit.
- FIX: "Einschaltverzögerung -> beim 2. EIN sofort schalten" wurde auch erst geschaltet, wenn das Logikmodul länger lief als die eingestellte Verzögerungszeit.
- FIX: "Ausschaltverzögerung -> beim 2. AUS sofort schalten" wurde auch erst geschaltet, wenn das Logikmodul länger lief als die eingestellte Verzögerungszeit.
- FIX: Konvertierung von DPT 9 nach DPT != 9 war um Faktor 10 zu groß. 9.0 wurde auf 90 Konvertiert statt auf 9.
- Formales Update, damit die Links auf die Dokumentation stimmen. Hier gab es einen Fehler im Release-Prozess.
- FIX: Bei der Einstellung ["Nur bei geändertem Ergebnis, aber erstes Telegramm immer senden"](#nur-bei-geändertem-ergebnis-aber-erstes-telegramm-nicht-senden) wurde das zweite Telegramm auch gesendet, wenn es gleich zum unterdrückten war. Das ist jetzt korrigiert.

## 0.11.0
### Breaking Changes
- none

### Feature
- NEU: Logikfunktion "Schalter" hinzugefügt. Siehe neues Kapitel ["Schalter (RS-Flip-Flop)"](#schalter-rs-flipflop)
- Neues Beispiel ["Einfacher Szenen-Controller"](#einfacher-szenen-controller) zugefügt.
- NEU: Feiertag Nationalfeiertag (AT) zugefügt (danke an mgeramb für den Code)
- NEU: Feiertag Maria Empfängnis (AT) zugefügt (danke an mgeramb für den Code)

### Bug
- FIX: Typo "Fronleichnam" korrigiert
- FIX: Eingangskonverter für Einzelwerte funktioniert jetzt korrekt
- FIX: Wenn man einen (oder mehrere) Logikkanäle ausgelassen hat, konnte es passieren, dass die Logiken hinter der Lücke nicht mehr ausgeführt wurden (z.B. Kanal 1, 2, 3, 5 => 5 wird nicht mehr berechnet)

## 0.8.0
### Breaking Changes
- none

### Feature
- Es gibt keine Änderungen an der Firmware oder ETS-Applikation, kein Update der Hardware oder der ETS nötig.
- Die Applikationsbeschreibung wurde um ein Beispiel ["Zeitschaltuhr schaltet alle 3 Tage"](#zeitschaltuhr-soll-jeden-n-ten-tag-schalten) ergänzt.
- Die Änderungshistorie wird jetzt in der umgekehrten Reihenfolge aufgeführt (vom neusten Eintrag als erstes bis hin zum ältesten als letztes). So werden die neusten Änderungen gleich zu Anfang präsentiert.
- Alle Bilder, Grafiken und Bildschirmfotos in der Anleitung haben jetzt einen Rahmen, der diese vom Text etwas absetzt. Dies erhöht die Lesbarkeit.
- Frauentag wurde in die Liste der Feiertage aufgenommen.

### Bug
- none

## 0.7.0
### Breaking Changes
- **Wichtig:** Diese Version ist eine komplett neue Applikation und somit nicht kompatibel zu der früheren Version 3.8. Die ETS-Applikation muss komplett neu parametrisiert werden.

### Feature
- initiales Release als OpenKNX LogicModule
- Basiert auf dem [Vorgänger-Logikmodul](https://github.com/mumpf/knx-logic) version 3.8 (im folgenden stehen die Neuerungen gegenüber 3.8)
- Erzeugung von Firmware und knxprod wurde stark vereinfacht
- (intern) verbesserte Kommunikation mit dem KNX-Bus
- ETS-Applikation wird auch mit der ETS 6 getestet
- Eingänge können jetzt auch beliebige Kommunikationsobjekte der gesamten Applikation (nicht nur des Logikmoduls) sein, keine Verbindung über GA nötig
- Die ETS-Applikation wurde optisch überarbeitet und übersichtlicher gestaltet
- ETS-Kanäle werden neu unterstützt
- Es wird technisch überprüft, ob die ETS-Applikation mit der installierten Firmware übereinstimmt
- FEATURE: Ein Tor kann jetzt auch einen Impulseingang haben. Damit wird das Tor geöffnet und danach sofort wieder geschlossen. So kann man auf eine einfache Weise Wertänderungen getaktet weiterleiten.

### Bug
- BUGFIX: Logikausgang war per Default "false" statt "initial". Damit wurde bei der Einstellung "nur bei Änderungen senden" so lange nichts gesendet, bis das erste Mal ein "true" festgestellt wird.

