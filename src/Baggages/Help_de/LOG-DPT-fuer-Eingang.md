### DPT für Eingang

Dieses Auswahlfeld legt den DPT für den Eingang fest. Unterstützt werden:

* DPT 1: binärer Wert
* DPT 2: Zwangsführung
* DPT 5: vorzeichenlose Zahl (0 bis 255)
* DPT 5.001: Prozentzahl (0 bis 100)
* DPT 6: vorzeichenbehaftete Zahl (-128 bis 127)
* DPT 7: vorzeichenlose Zahl (0 bis 65.535)
* DPT 8: vorzeichenbehaftete Zahl (-32.768 bis 32.767)
* DPT 9: Gleitkommawert (-670.760,96 bis 670.760,96)
* DPT 12: vorzeichenlose Zahl (0 bis 4294967296)
* DPT 13: vorzeichenbehaftete Zahl (-2.147.483.648 bis 2.147.483.647)
* DPT 14: Gleitkommawert (-1.000.000.000.000 bis 1.000.000.000.000)
* DPT 17: Szenen Nummer (1-64)
* DPT 232: RGB-Wert (0-16.777.216)

Ist der DPT anders als DPT 1, erscheint je nach DPT ein Konverter, mit dem man den gewünschten Eingangs-DPT nach DPT 1 wandeln kann. Die gesamte weitere Verarbeitung des Eingangssignals erfolgt binär, also auf Basis von DPT 1.

> **Wichtig:** Wenn ein bestehendes Kommunikationsobjekt genutzt wird, muss der hier eingestellte DPT dem DPT des bestehenden KO entsprechen! Das kann nicht automatisch von der Applikation ermittelt werden. Wenn hier etwas falsch eingestellt wird und der falsche Eingangskonverter benutzt wird, kann der Logikkanal nicht korrekt funktionieren. 
