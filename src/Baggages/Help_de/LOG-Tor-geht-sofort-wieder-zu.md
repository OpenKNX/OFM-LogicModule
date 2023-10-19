### **Tor geht sofort wieder zu**

Ein Tor bleibt normalerweise offen, nachdem ein EIN-Telegramm am Toreingang empfangen wurde. Erst bei einem AUS-Telegramm am Toreingang geht das Tor wieder zu.

Wird "Tor geht sofort wieder zu" gewählt, geht das Tor beim Empfang eines EIN-Telegramms auf und danach sofort wieder zu. Es wird somit nicht auf ein AUS-Telegramm gewartet.

Man nennt einen solchen Eingang auch Impulseingang. Durch einen solchen Impulseingang kann man eine Taktung/Synchronisation der Werte erreichen, die am Eingang anliegen.

So kann z.B. ein Wert am Eingang, egal wie oft er sich ändert, nur jede Minute zum Ausgang durchgeschaltet werden. Oder ein Wert wird nur durchgelassen, wenn der Benutzer eine Taste betätigt.

Wenn man den Toreingang invertiert, kann man ein Tor realisieren, dass beim Empfang einer 0 kurz auf und sofort wieder zu geht.

> Abstrakt betrachtet entspricht ein Tor mit einem Impulseingang einem Tor mit einem normalen Eingang, vor den ein Treppenlicht geschaltet wurde, das sofort (z.B. nach 0.1 Sekunden) abläuft. Das Tor sendet dann nur beim öffnen, nichts beim schließen.

