### Feiertage auf dem Bus verfügbar machen?

Ein "Ja" bei dieser Einstellung schaltet 2 Kommunikationsobjekte frei. Über diese Kommunikationsobjekte wird die Nummer eines Feiertags gesendet. Jede gesendete Nummer entspricht genau einem Feiertag, die Nummern entsprechen denen in der Liste von Feiertagseinstellungen (siehe vorheriges Bild).

* KO 5 (Welcher Feiertag ist heute?) sendet, wenn der aktuelle Tag ein Feiertag ist,
* KO 6 (Welcher Feiertag ist morgen?) sendet, wenn der nächste Tag ein Feiertag ist.

Beide Kommunikationsobjekte (5 und 6) werden immer kurz nach Mitternacht (aber nicht exakt um Mitternacht) neu berechnet. Sie senden eine 0, wenn kein Feiertag ist und sich der Wert geändert hat.

