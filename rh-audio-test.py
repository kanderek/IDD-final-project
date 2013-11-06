'''Remote Host
 -----------
 /usr/local/bin/python 
 1.  Sends 10 bytes of data representing the number 
     of samples.  Each byte is 1 digit of an unsigned long.
 
 
 2.  Each time the host recieves a byte it sends the next
     128 audio samples to fill the Arduino's receive buffer.
'''


import serial

def make10ByteStr(number):
	charTen = str(number)
	if(len(charTen) > 10):
		return None
	else: 
		for i in range(len(charTen), 10):
			charTen = '0' + charTen
		return charTen 


filename = 'testsamples'
f = open(filename, 'r')

listOfChars = []

for line in f:
	line = line.strip()
	line = line.replace(" ", "")
	listOfChars.extend(line.split(',', 19))
	#print("line " + str(count) + ": " + line)

#print listOfChars
#print len(listOfChars)
sounddata_length = make10ByteStr(len(listOfChars))
print sounddata_length

ser = serial.Serial('/dev/tty.usbmodem1411', 115200)  # open first serial port
print ser.name       # check which port was really used

#ser.write(sounddata_length)      # write a string

count = 0
numSamples = 128

while(count < len(listOfChars)):
	send = ser.read(size=1)
	if(send):
		print send
		if(len(listOfChars)-count >= 128):
			numSamples = 128
		else:
			numSamples = len(listOfChars)-count
			
		for i in range(count,count+numSamples):
			ser.write(listOfChars[i])
			count += 1
	else:
		print "nothing read"

ser.close()       # close port