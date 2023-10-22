### **Bei Neustart letzte Schaltzeit nachholen**

Nach einem Neustart des Moduls kann die letzte Schaltzeit erneut ausgeführt werden. Sobald das Datum und die Uhrzeit erstmals über den Bus gesetzt worden sind, wird nach der spätesten Schaltzeit gesucht, die noch vor dem aktuellen Datum/Uhrzeit liegt. Dieser Schaltzeitpunkt wird dann ausgeführt.

Da eine Nachberechnung aller Schaltzeiten für bis zu 99 Zeitschaltuhren inklusive Feiertagsbehandlung direkt nach dem ersten Setzen der Zeit über den Bus sehr lange dauern würde und in dieser Zeit (mehrere Sekunden) die Funktion des Moduls gestört wäre, wird die Nachberechnung der Schaltzeiten durch einen Nebenprozess während der normalen Funktion des Moduls durchgeführt. Der Nebenprozess funktioniert in kleinen Schritten, die wenig Rechenzeit kosten und die Normalfunktion nicht behindern. Als Konsequenz kann es etwas dauern, bis der entsprechende nachberechnete Zeitschaltpunkt nachgeholt wird.

Wie lange es dauert, bis ein nachberechneter Zeitschaltpunkt nachgeholt wird, hängt wiederum vom Zeitschaltpunkt selbst ab.

Der Nebenprozess wird pro Sekunde zweimal aufgerufen und geht dabei jeweils einen weiteren Tag zurück, berechnet für diesen Tag die Feiertage und prüft für jede Zeitschaltuhr, die bisher noch keinen definierten Ausgangswert hat (sie könnte ja schon von sich aus im Rahmen der Normalfunktion geschaltet haben), ob diese Zeitschaltuhr an diesem Tag schalten sollte. Wenn ja, dann schaltet diese Zeitschaltuhr mit dem für diesen Tag zeitlich spätesten Wert. Damit ist der zeitlich späteste Schaltpunkt vor dem Modulneustart gegeben.

Obiges bedeutet, dass der Nebenprozess für Tagesschaltuhren, die auch Wochentage enthalten können, bis zu 3 Sekunden benötigen kann, um eine (Tages-)Schaltzeit nachzuholen, da er 2 Tage pro Sekunde zurückgeht.

Bei Jahresschaltuhren wird der späteste Schaltzeitpunkt, der nachberechnet wurde, 366 / 2 = 183 Sekunden nach dem ersten setzen der Zeit über den Bus erreicht, also etwa 3 Minuten nach dem Neustart. Dies ist ein theoretischer Wert, da in diesem Fall der Schaltzeitpunkt vor einem Jahr liegen müsste und sich zwischendurch nicht geändert hat. Da man meistens aber einen Schaltzeitpunkt für EIN und einen für AUS definiert, wird bei Jahresschaltzeiten wahrscheinlich einer der Schaltzeitpunkte bereits früher erreicht.

Der Nebenprozess beendet sich selbst, sobald alle Zeitschaltuhren einen definierten Ausgangswert haben.

> **Achtung:** Zeitschaltuhren, die Urlaubstage berücksichtigen, können bei der Nachberechnung der Zeitschaltpunkte nicht mit einbezogen werden, da die Information "Urlaubstag" per KO von extern dem Modul über den Bus gemeldet wird und somit nicht für die (historische) Nachberechnung zur Verfügung steht. Somit werden bei der Nachberechnung alle Zeitschaltuhren mit einer anderen Angabe als "Urlaub nicht beachten" ignoriert.

