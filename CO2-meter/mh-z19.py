import serial

p = [255, 134, 2, 244, 64, 4, 62, 122, 136]

def check_crc(packet):
	crc = 0
	for i in range(1, 8):
		crc = (crc + packet[i]) & 0xff
	crc = (255 - crc) + 1
	return packet[0] == 0xff and packet[1] == 0x86 and packet[8] == crc


if __name__ == "__main__":
	port = serial.Serial('/dev/cu.usbserial-A50285BI', timeout=5)

	command = [0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79]

	try:
		port.write(command)
		recv = port.read(9)
		ints = map(lambda x: ord(x), recv)
		print 'Received: %s' % ints
		if check_crc(ints):
			print 'CRC check: OK'
			co2Level = ints[2] << 8 | ints[3]
			print 'CO2 reading: %s' % co2Level 
		else:
			print 'CRC check: error'
	finally:
		port.close()

