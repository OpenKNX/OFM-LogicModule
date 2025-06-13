### Kommunikationsobjekt für Eingang

Ein Eingang des Logikobjekts wird durch ein Kommunikationsobjekt repräsentiert. Dabei kann jedes Kommunikationsobjekt des Gerätes verwendet werden, auch KO, die nicht vom Logikmodul verwaltet werden. Dies ermöglicht, KO intern zu verbinden und so die Kommunikation auf dem Bus zu reduzieren. Eine solche interne Verbindung verhält sich genau so, als wäre der Eingang mit einer GA verbunden und Telegramme erhalten würde.

Es gibt 2 Möglichkeiten, interne KO-Verknüpfungen zu realisieren: Absolut und relativ. 

* Absolute KO-Verknüpfungen werden über die Angabe einer KO-Nummer vorgenommen. Dieses KO wird dann für den Eingang benutzt.
* Relative KO-Verknüpfungen werden über die Angabe eines KO-Offsets vorgenommen. Das eigentliche KO wird dadurch ermittelt, dass der angegebene Offset zur Nummer des eigenen KO addiert wird und so das Ziel-KO ermittelt wird.

Relative und absolute KO-Verknüpfungen funktionieren absolut identisch. Relative Verknüpfungen sind von Vorteil, wenn man Logikblöcke baut, die aus mehreren Kanälen bestehen. Falls solche Logikblöcke mal verschoben oder auf ein anderes Gerät kopiert werden sollen, bekommen die Ein- und Ausgänge neue KO-Nummern. Wenn innerhalb der Logikblöcke mit relativen KO-Verknüpfungen gearbeitet wurde, funktionieren die Logikblöcke weiterhin, bei absoluten KO-Verknüpfungen müssen alle KO-Nummern angepasst werden.

#### Eigenes KO

Für den Eingang wird das Kommunikationsobjekt benutzt, dass für diesen Eingang vorgesehen ist. Dieses KO ist somit komplett vom Logikmodul kontrolliert und verwaltet.

#### **Absolutes KO"

Der Eingang wird durch ein existierendes Kommunikationsobjekt erzeugt, dessen KO-Nummer direkt angegeben wird. 

Das Kommunikationsobjekt wird anderweitig verwaltet, z.B. durch einen anderen Kanal oder eine andere Teilapplikation. Der Eingang "lauscht" somit nur auf die ankommenden Signale und kann keinerlei Aktionen an dem KO vornehmen, wie z.B. den DPT bestimmen oder Lese-Anforderungen verschicken.

#### **Relatives KO"

Der Eingang wird durch ein existierendes Kommunikationsobjekt erzeugt, dessen KO-Nummer berechnet wird, indem die eigene KO-Nummer genommen wird und der eingegebene Offset addiert wird. 

Das Kommunikationsobjekt wird anderweitig verwaltet, z.B. durch einen anderen Kanal oder eine andere Teilapplikation. Der Eingang "lauscht" somit nur auf die ankommenden Signale und kann keinerlei Aktionen an dem KO vornehmen, wie z.B. den DPT bestimmen oder Lese-Anforderungen verschicken.

