### Wert für EIN an ein zusätzliches KO senden


Ein Ausgang kann seinen EIN-Wert nicht nur über das ihm zugewiesene KO senden, sondern auch zusätzlich an ein internes KO. Das bedeutet, der Wert wird in ein beliebiges KO des Gerätes (nicht nur des Logikmoduls) geschrieben, ohne dass dafür eine GA-Verknüpfung notwendig ist.

> Wichtig: Der DPT des Ziel-KO muss der gleiche sein wie der DPT des Ausgangs. Falls nicht, sind die Ergebnisse nicht abschätzbar.


#### Eigenes KO

Das ist der Standardwert und bedeutet, dass der Wert des Ausgangs an das KO, dass für diesen 
Ausgang vorgesehen ist, gesendet wird.

Zu Informationszwecken wird die Nummer des KO angegeben, das diesem Ausgang zugeordnet ist.

#### Absolutes KO

Wird absolutes KO ausgewählt, kann man die Nummer des KO angeben, an das der Wert des Ausgangs gesendet wird. Das kann ein beliebiges KO des Gerätes sein, auf dem das Logikmodul läuft. Es können auch KO anderer Module angegeben werden.

Ist eine GA mit dem eigenen KO des Ausgangs verknüpft, wird der Wert des Ausgangs auch an diese GA gesendet.

#### Relatives KO

Wie bei "Absolutes KO" wird der Wert des Ausgangs an ein anderes KO gesendet. Man gibt aber einen relativen Wert (Offset) ein und die Nummer des Ziel-KO wird ausgerechnet. Den berechneten Wert sieht man zur Information direkt hinter dem Eingabefeld.

Relative KO sind von Vorteil, wenn man häufiger mit Logiken arbeitet, die aus mehreren Logikkanälen bestehen (Logikblock). Verbindet man die Logiken eines Logikblocks über relative KO-Verknüpfungen, können diese Logikblöcke innerhalb des Logikmoduls verschoben werden oder auf ein anderes Logikmodul kopiert werden (mittels Konfigurationstransfer), ohne dass die KO-Verknüpfungen invalidiert werden.

Ist eine GA mit dem eigenen KO des Ausgangs verknüpft, wird der Wert des Ausgangs auch an diese GA gesendet.

