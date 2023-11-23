### Logik auswerten

Erscheint nur, wenn die Logik-Operation nicht auf ZEITSCHALTUHR gestellt wurde.

Wie bereits in "Logikkanäle -> [Startverhalten](#startverhalten)" beschrieben, ist es notwendig, einer Logikverknüpfung zu sagen, wie sie mit undefinierten Eingängen umgehen soll.

#### auch wenn noch nicht alle Werte gültig sind

Die logische Verknüpfung betrachtet alle undefinierten Eingänge als ob sie mit "inaktiv" parametriert wären. Ein UND mit 3 Eingängen, das von den einer undefiniert ist, wird bereits ein EIN senden, wenn die 2 restlichen Eingänge EIN sind.

Ein TOR mit einem undefinierten Dateneingang oder einem undefinierten Toreingang kann nicht sinnvoll funktionieren und sendet dann gar nichts.

#### erst wenn alle Werte gültig sind

Die logische Verknüpfung wird erst dann einen Wert ermitteln, wenn an allen Eingängen gültige Werte vorliegen.

