### Beim schließen vom Tor wird

Das Auswahlfeld erscheint nur, wenn als Logik-Operation TOR gewählt wurde und das Tor nicht sofort nach dem Öffnen geschlossen wird.

Mit dem Auswahlfeld kann man einstellen, ob das Tor zusätzliche Telegramme verschicken soll, wenn es gerade geschlossen wird (Toreingang geht auf AUS).

Normalerweise ist das Tor entweder geöffnet oder geschlossen. Ein geschlossenes Tor kann nicht nochmal schließen. Wird also wiederholt ein AUS-Signal an den Toreingang gesendet, werden die folgenden Einstellungen nur beim ersten AUS (beim wirklichen schließen) berücksichtigt, nicht bei den Folgetelegrammen.

Nach einem Neustart ist das Tor weder geöffnet noch geschlossen, das erste AUS-Telegramm am Toreingang führt somit auf jeden Fall zum schließen und zur Berücksichtigung der folgenden Einstellungen.

#### nichts gesendet

Beim schließen vom Tor wird nichts gesendet.

#### AUS gesendet

Beim schließen vom Tor wird immer ein AUS-Signal gesendet.

#### EIN gesendet

Beim schließen vom Tor wird immer ein EIN-Signal gesendet.

#### Eingangswert gesendet

Beim schließen vom Tor wird der Eingangswert gesendet. Da dieser Wert ja faktisch schon mal gesendet worden ist (als das Tor noch offen war), ist das effektiv eine einmalige Wiederholung des letzten Wertes.

