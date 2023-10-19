### **Beim öffnen vom Tor wird**

Das Auswahlfeld erscheint nur, wenn als logische Operation TOR gewählt wurde.

Mit dem Auswahlfeld kann man einstellen, ob das Tor zusätzliche Telegramme verschicken soll, wenn es gerade geöffnet wird (Toreingang geht auf EIN).

Normalerweise ist das Tor entweder geöffnet oder geschlossen. Ein geöffnetes Tor kann nicht nochmal öffnen. Wird also wiederholt ein EIN-Signal an den Toreingang gesendet, werden die folgenden Einstellungen nur beim ersten EIN (beim wirklichen öffnen) berücksichtigt, nicht bei den Folgetelegrammen.

Nach einem Neustart ist das Tor weder geöffnet noch geschlossen, das erste EIN-Telegramm am Toreingang führt somit auf jeden Fall zum öffnen und zur Berücksichtigung der folgenden Einstellungen.

#### **nichts gesendet**

Beim öffnen vom Tor wird nichts gesendet, erst das nächste Telegramm am Dateneingang wird gesendet.

#### **AUS gesendet**

Beim öffnen vom Tor wird immer ein AUS-Signal gesendet.

#### **EIN gesendet**

Beim öffnen vom Tor wird immer ein EIN-Signal gesendet.

#### **Eingangswert gesendet**

Beim öffnen vom Tor wird der Eingangswert gesendet. Damit kann man erreichen, dass das letzte Signal, das vom Tor blockiert worden ist, nach dem öffnen doch noch durchkommt.

