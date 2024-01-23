### Wert für EIN senden?


In dieser Auswahlbox wird festgelegt, ob und was für ein Wert bei einem EIN-Signal gesendet werden soll.

#### Nein

Für ein EIN-Signal wird kein Wert auf den Bus gesendet. Das entspricht einem Ausgangsfilter, der alle EIN-Signale unterdrückt.

#### Ja - Wert vorgeben

Hier kann der Wert, der für ein EIN-Signal gesendet wird, konstant vorgegeben werden. In einem weiteren Feld kann der konstante Wert DPT gerecht eingegeben werden.

#### Ja - Wert von Eingang 1

Bei einem EIN-Signal wird der Wert gesendet, der am Eingang 1 anliegt. Sollte der Wert nicht den passenden DPT haben, wird er generisch gewandelt.

#### Ja - Wert von Eingang 2

Bei einem EIN-Signal wird der Wert gesendet, der am Eingang 2 anliegt. Sollte der Wert nicht den passenden DPT haben, wird er generisch gewandelt.

#### Ja - Wert eines KO
Bei einem EIN-Signal wird der Wert gesendet, der am angegebenen KO anliegt. Sollte der Wert nicht den passenden DPT haben, wird er generisch gewandelt.

Man kann somit einen Wert senden, der an irgendeinem KO des Gerätes anliegt. Es sind explizit alle KO-Nummern erlaubt, nicht nur die KO-Nummern des Logikmoduls. Einzige Einschränkung: Es sind nur DPT erlaubt, die von einem Eingang des Logikmoduls unterstützt werden.

#### Ja - Wert einer Funktion

Bei einem EIN-Signal wird ein berechneter Wert gesendet. In einem weiteren Feld kann die Funktion ausgewählt werden, die angewendet werden soll. Sollte der Wert nicht den passenden DPT haben, wird er generisch gewandelt.

Im Kapitel Funktionen kann nachgelesen werden, wie Funktionen und Benutzerfunktionen verwendet werden können.

#### Ja - ReadRequest senden

Bei einem EIN-Signal wird kein Wert auf die GA am Ausgang gesendet sondern ein Lesetelegramm. Damit kann man für Geräte, die kein zyklisches Senden unterstützen, bei Bedarf eine Abfrage eines Ausgangs erreichen.

#### Ja - 'Gerät zurücksetzen' senden

Bei einem EIN-Signal wird kein Wert gesendet, sondern die ETS-Funktion "Gerät zurücksetzen" an eine bestimmte PA geschickt. So kann man bestimmte Geräte überwachen und bei Bedarf zurücksetzen, ohne die ETS starten zu müssen.

In einem weiteren Feld kann die PA ausgegeben werden.

#### Ja - Tonwiedergabe (Buzzer)

Wird nur angeboten, wenn ein Buzzer vorhanden ist.

Bei einem EIN-Signal wird kein Wert gesendet, sondern der interne Buzzer zur Tonwiedergabe angesprochen. In einem weiteren Feld wird angegeben, in welcher Lautstärke die Tonwiedergabe gestartet oder ob sie gestoppt wird.

Falls der Buzzer gerade über das Kommunikationsobjekt 9 gesperrt ist, wird kein Ton ausgegeben und ein eventueller laufender Ton abgeschaltet.

Falls dieser Kanal als Alarmkanal gekennzeichnet ist, wird ein Ton unabhängig von der Sperre ausgegeben.


#### Ja - RGB-LED schalten

Wird nur angeboten, wenn eine RGB-LED vorhanden ist.

Bei einem EIN-Signal wird kein Wert gesendet, sondern die interne RBG-LED angesprochen. So kann man eine optische Rückmeldung erreichen.

In einem weiteren Feld wird die Farbe eingestellt. Ist die Farbe Schwarz eingestellt, wir die LED ausgeschaltet.

Falls die LED gerade über das Kommunikationsobjekt 8 gesperrt ist, wird die LED nicht eingeschaltet und falls sie an ist, wird sie abgeschaltet.

Falls dieser Kanal als Alarmkanal gekennzeichnet ist, wird die LED unabhängig von der Sperre eingeschaltet.

