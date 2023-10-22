### **Kommunikationsobjekt für Eingang**

Ein Eingang des Logikobjekts wird durch ein Kommunikationsobjekt repräsentiert. Dabei kann jedes Kommunikationsobjekt des Gerätes verwendet werden, auch KO, die nicht vom Logikmodul verwaltet werden. Dies ermöglicht, KO intern zu verbinden und so die Kommunikation auf dem Bus zu reduzieren. Eine solche interne Verbindung verhält sich genau so, als wäre der Eingang mit einer GA verbunden und Telegramme erhalten würde.

#### **Neues KO erzeugen**

Für den Eingang wird ein neues Kommunikationsobjekt erzeugt. Dieses KO ist somit komplett vom Logikmodul kontrolliert und verwaltet.

#### **Bestehendes KO nutzen**

Der Eingang wird durch ein existierendes Kommunikationsobjekt erzeugt. Das Kommunikationsobjekt wird somit anderweitig verwaltet, z.B. durch einen anderen Kanal oder eine andere Teilapplikation. Der Eingang "lauscht" somit nur auf die ankommenden Signale und kann keinerlei Aktionen an dem KO vornehmen, wie z.B. den DPT bestimmen oder Lese-Anforderungen verschicken.

