### Anschliessend die Signalverarbeitung

Die Sperre funktioniert wie ein Filter, auch im gesperrten Zustand ist die Logik weiterhin aktiv und reagiert entsprechend der Logikdefinition, inklusive der kompletten Signalverarbeitung. Es wird nur kein Signal an die Ausgangskonverter weitergegeben. Würde z.B. während einer Einschaltverzögerung von 5 Minuten die Sperre nach 4 Minuten zurückgesetzt, wurde nach einer weiteren Minute die Einschaltverzögerung ablaufen und ein EIN senden. Ein solches Verhalten kann gewünscht sein, aber auch nicht gewünscht sein. Aus diesem Grunde kann man das Verhalten hier einstellen:

* **Nicht zurücksetzen** - Die Signalverarbeitung bleibt so wie sie ist und schaltet entsprechend der Parametrisierung
* **Nach dem Sperren zurücksetzen** - Die Signalverarbeitung wird nach dem Sperren zurückgesetzt, läuft aber während der Sperre normal weiter.
* **Nach dem Entsperren zurücksetzen** - Die Signalverarbeitung wird nach dem Entsperren zurückgesetzt und läuft dann normal von der Logik getriggert weiter. Das ist default, da es das wahrscheinlichste erwartete Verhalten ist.

Wird ein "Zurücksetzen" gewählt, werden immer alle Operationen der Signalverarbeitung zurückgesetzt, dazu gehören:

* Treppenlichtzeit
* Die interne Zeit fürs Blinken 
* Die Zeitmesser für die Ein- und Ausschaltverzögerung
* Die Zeiten zum zyklischen Senden

Der Eingang der Signalverarbeitung (der gleichzeitig der Ausgang der eigentlichen Logikfunktion ist) wird auf den zuletzt an den Ausgangskonverter gesendeten Wert gesetzt (aber ohne dass die Signalverarbeitung erneut gestartet wird). Nach dem Zurücksetzen läuft die Signalverarbeitung mit dem nächsten von der Logik berechneten Ausgangssignal wieder los, egal ob die Sperre noch aktiv ist oder nicht.

Berücksichtigt man die möglichen Signalverzögerungen während der Signalverarbeitung durch Treppenlicht und Ein- bzw. Ausschaltverzögerung und die mögliche Erzeugung von neuen Signalen durch Blinken und zyklisch Senden, dann kann ein Entsperren, vor allem mit "Beim Entsperren den aktuellen Wert senden" zu unerwarteten Ergebnissen führen. 

Wahrscheinlich ist in den meisten Fällen ein zurücksetzen der Signalverarbeitung beim Entsperren sinnvoll, deswegen ist dieser Wert auch der Standardwert. Die gewünschte Einstellung sollte auf jeden Fall getestet werden, um sicherzustellen, dass sie zu gewünschten Ergebnis führt.

