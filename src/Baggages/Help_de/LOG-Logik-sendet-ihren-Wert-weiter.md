### **Logik sendet ihren Wert weiter**

Diese Auswahlbox erlaubt eine Detaillierte Einstellung des Verhaltens.

#### **Nur bei geändertem Ergebnis**

Das Ergebnis der Logikauswertung wird nur dann weitergeschickt, wenn sich das Ergebnis geändert hat. Dazu wird das zuvor ermittelte Ergebnis der Logik (wichtig: nicht das Ergebnis am Ausgang des Logikkanals) herangezogen und mit dem aktuellen Ergebnis verglichen. Weicht es ab, wird das gerade ermittelte Ergebnis weitergeleitet.

#### **Nur bei geändertem Ergebnis, aber erstes Telegramm immer senden**

Diese Einstellung hat ein spezifisches Verhalten beim Neustart der Logik. Bei einem Neustart ist nicht klar, was "geändertes Ergebnis" heißt. Mit dieser Einstellung sagt man klar, dass das erste Ergebnis der Logik immer als "geändert" behandelt wird und so weitergeschickt wird. Gleichzeitig stellt das Ergebnis den Vergleichswert für die nächste Logikoperation dar, anhand dessen ein "geändertes Ergebnis" festgestellt werden kann.

#### **Nur bei geändertem Ergebnis, aber erstes Telegramm nicht senden**

Diese Einstellung hat ein spezifisches Verhalten beim Neustart der Logik. Bei einem Neustart ist nicht klar, was "geändertes Ergebnis" heißt. Mit dieser Einstellung sagt man klar, dass das erste Ergebnis der Logik immer als "nicht geändert" behandelt wird und somit nicht weitergeschickt wird. Gleichzeitig stellt das Ergebnis den Vergleichswert für die nächste Logikoperation dar, anhand dessen ein "geändertes Ergebnis" festgestellt werden kann.

#### **bei allen Eingangstelegrammen**

Sobald ein neues Eingangstelegramm eintrifft, wird das Ergebnis der logischen Verknüpfung ermittelt und an den nächsten Funktionsblock weitergeleitet.

#### **bei allen Eingangstelegrammen, aber erstes Telegramm nicht senden**

Sobald ein neues Eingangstelegramm eintrifft, wird das Ergebnis der logischen Verknüpfung ermittelt und an den nächsten Funktionsblock weitergeleitet. Allerdings wird das erste Telegramm nach einem Neustart unterdrückt. Damit kann man vermeiden, dass mögliche Statusmeldungen bei einem Neustart ungewollt Logiken bzw. Folgelogiken auslösen.

#### **bei folgenden Eingangstelegrammen**

<kbd>![Logik sendet](pics/LogikSendet.PNG)</kbd>

Es erscheint eine Liste mit allen aktiven Eingängen. 
