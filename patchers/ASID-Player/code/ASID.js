//parse ASID data
inlets = 2;
outlets = 2;
 
// SID Register Reihenfolge für ASID
const asid_sid_registers = [
    0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x04, 0x0b, 0x12, 0x04, 0x0b, 0x12
];

// Funktion zum Dekodieren der ASID MIDI-Daten
function list() {
    const data = arguments;
    const dataSize = arguments.length;
        var reg = 0;

        for (var mask = 0; mask < 4; mask++) {  // maximal 4 Masken
            for (var bit = 0; bit < 7; bit++) {  // jedes Paket hat 7 Bits
                if (data[mask + 2] & (1 << bit)) {  // 3-Byte Nachricht
                    var register_value = data[reg + 10];  // Wert aus dem Puffer abrufen

                    if (data[mask + 6] & (1 << bit)) {  // wenn Wert höher als 0
                        register_value |= 0x80;  // 8. MSB Bit setzen
                    }

                    const address = asid_sid_registers[mask * 7 + bit];
                    
                    // Verarbeite Daten (hier kann die Ausgabe auf SIDBlaster erfolgen)
                    outlet(1, address);
					outlet(0, register_value);
                    
                    reg++;
                }
            }
        }
    
}
