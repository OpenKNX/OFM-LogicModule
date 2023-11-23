### Eingangswert speichern und beim nächsten Neustart als Vorbelegung nutzen

Mit "Ja" legt man fest, dass der zuletzt an diesem Eingang empfangene Wert im nichtflüchtigen Speicher abgelegt wird und nach einem Neustart wieder gelesen wird. Der dann gelesene Wert wird als Vorbelegung für den Eingang genommen, falls nötig über den Eingangskonverter in einen DPT 1 konvertiert und dann die logische Operation getriggert. Ist der Eingang als interner Eingang über sein KO mit weiteren Eingängen verbunden (egal ob das KO im Logikmodul oder einem anderen Modul liegt, das das Logikmodul verwendet), empfangen auch alle verknüpften Eingänge den aus dem nichtflüchtigen Speicher geladenen Wert.

Normalerweise triggert der neu geladene Wert die zugeordnete Logikfunktion und kann über den zugehörigen Ausgang auf den Bus gesendet werden. Man kann allerdings den Wert auch direkt vom Eingang auf den Bus senden lassen:

* Über eine aktive Leseanforderung: Hierzu muss das L-Flag am zugehörigen Eingang gesetzt werden
* Aktives Senden gleich nach einem Neustart: Hierzu muss das Ü-Flag am zugehörigen Eingang gesetzt werden

> **Achtung:** Da das Lesen vom nichtflüchtigen Speicher direkt nach einem Neustart geschieht und alle Initialisierungen sofort laufen, werden beim Setzen des Ü-Flag am Eingang jegliche gelesenen Werte sofort gesendet und nicht erst, nachdem die Zeit abgelaufen ist, bis der Kanal aktiv wird. Das ist der Grund, warum das Ü-Flag nicht standardmäßig gesetzt ist. Würden alle 99 möglichen Kanäle bei einem Neustart ihre Werte von nichtflüchtigen Speicher lesen und sofort senden, würden die resultierenden 99 Telegramme sofort den Bus überlasten und einige davon verloren gehen. Um solche Situationen zu vermeiden, sollte man einen aus dem nichtflüchtigen Speicher gelesenen Wert über den Ausgang auf den Bus senden, dieser Wert wird dann erst gesendet, nachdem der entsprechende Kanal aktiv ist. 

Da nichtflüchtige Speicher nur eine relativ geringe Anzahl an Schreibzyklen zulassen, wird der Eingangswert nicht direkt nach dem Empfang im Speicher geschrieben, sondern erst beim Stromausfall, bei einem "Gerät zurücksetzen" über die ETS oder bei einer Neuprogrammierung über die ETS. Wird die RESET-Taste direkt am Gerät gedrückt, wird der nichtflüchtige Speicher nicht mit dem Eingangswert beschrieben.

> **Wichtig:** Das speichern der Werte in den nichtflüchtigen Speicher bei Stromausfall ist hardwareabhängig und wird nicht von jeder Hardware unterstützt. Auch in einem solchen Fall kann die Funktion sinnvoll sein, z.B. bie einem Neustart nach einer ETS-Programmierung, deswegen wird die Funktion immer angeboten. Ob ein Speichern beim Stromausfall unterstützt wird, steht (hoffentlich) in der Anleitung zum Hardware-Gerät, dass das Logikmodul verwendet.

