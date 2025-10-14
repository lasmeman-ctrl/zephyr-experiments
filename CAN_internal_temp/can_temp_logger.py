import can

# open socketCAN on can0 (your VulCAN interface)
bus = can.interface.Bus(channel='can0', bustype='socketcan')

print("Listening on can0 for ID 0x321 ...")

for msg in bus:
    if msg.arbitration_id == 0x321 and len(msg.data) >= 2:
        centi_c = int.from_bytes(msg.data[0:2], 'little', signed=True)
        print(f"Temperature: {centi_c/100.0:.2f} °C")
