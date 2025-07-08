### Sendeverhalten für Ausgang

In dieser Auswahlbox kann für den Ausgangskonverter festgelegt werden, ob das wiederholte Senden des gleichen Wertes erlaubt ist oder nicht. 

Dies darf nicht mit dem Wiederholungsfilter verwechselt werden.

Der Wiederholungsfilter arbeitet mit den logischen Werten EIN und AUS, die in der Logikverarbeitung verwendet werden.
Der Ausgangskonverter übersetzt den logischen Wert z.B. in das Ergebnis einer Formel, die aus den beiden Eingangswerten berechnet wird, also einer Zahl eines anderen DPT. Das Sendeverhalten arbeitet mit dem zuletzt durch das KO gesendeten Wert und vergleicht diesen mit dem neu zu sendenden Wert. Je nachdem, wie das Sendeverhalten eingestellt ist, wird dann gesendet oder nicht.

#### Alle Werte senden

Jeder Wert wird auf den Bus gesendet, auch wenn er gleich dem zuletzt gesendetem Wert ist. Das ist das Standardverhalten.

#### Nur geänderte Werte senden

Der neu zu sendende Wert wird passend zum DPT des KO gewandelt und mit dem bisherigen KO-Wert vergleichen. Nur wenn sich die Werte unterscheiden, wird der neue Wert gesendet.


