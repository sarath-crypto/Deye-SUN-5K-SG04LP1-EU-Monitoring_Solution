import sys
import socket
import binascii
import re
import libscrc
import json
import configparser
import datetime

def twosComplement_hex(hexval):
    bits = 16
    val = int(hexval, bits)
    if val & (1 << (bits-1)):
        val -= 1 << bits
    return val

# CONFIG
configParser = configparser.RawConfigParser()
configFilePath = r'./config.cfg'
configParser.read(configFilePath)

inverter_ip=configParser.get('DeyeInverter', 'inverter_ip')
inverter_port=int(configParser.get('DeyeInverter', 'inverter_port'))
inverter_sn=int(configParser.get('DeyeInverter', 'inverter_sn'))
installed_power=int(configParser.get('DeyeInverter', 'installed_power'))
# END CONFIG

# PREPARE & SEND DATA TO THE INVERTER
output="{" # initialise json output
pini=59
pfin=112
chunks=0
while chunks<2:
 if chunks==-1: # testing initialisation
  pini=235
  pfin=235
  print("Initialise Connection")
 start = binascii.unhexlify('A5') #start
 length=binascii.unhexlify('1700') # datalength
 controlcode= binascii.unhexlify('1045') #controlCode
 serial=binascii.unhexlify('0000') # serial
 datafield = binascii.unhexlify('020000000000000000000000000000') #com.igen.localmode.dy.instruction.send.SendDataField
 pos_ini=str(hex(pini)[2:4].zfill(4))
 pos_fin=str(hex(pfin-pini+1)[2:4].zfill(4))
 businessfield= binascii.unhexlify('0103' + pos_ini + pos_fin) # sin CRC16MODBUS
 crc=binascii.unhexlify(str(hex(libscrc.modbus(businessfield))[4:6])+str(hex(libscrc.modbus(businessfield))[2:4])) # CRC16modbus
 checksum=binascii.unhexlify('00') #checksum F2
 endCode = binascii.unhexlify('15')
 
 inverter_sn2 = bytearray.fromhex(hex(inverter_sn)[8:10] + hex(inverter_sn)[6:8] + hex(inverter_sn)[4:6] + hex(inverter_sn)[2:4])
 print("SN ",inverter_sn2[0],inverter_sn2[1],inverter_sn2[2],inverter_sn2[3])
 print("SN ",hex(inverter_sn2[0]),hex(inverter_sn2[1]),hex(inverter_sn2[2]),hex(inverter_sn2[3]))

 frame = bytearray(start + length + controlcode + serial + inverter_sn2 + datafield + businessfield + crc + checksum + endCode)
 
 checksum = 0
 frame_bytes = bytearray(frame)
 for i in range(1, len(frame_bytes) - 2, 1):
     checksum += frame_bytes[i] & 255
 frame_bytes[len(frame_bytes) - 2] = int((checksum & 255))

 # OPEN SOCKET
 for res in socket.getaddrinfo(inverter_ip, inverter_port, socket.AF_INET,socket.SOCK_STREAM):
                  family, socktype, proto, canonname, sockadress = res
                  try:
                   clientSocket= socket.socket(family,socktype,proto);
                   clientSocket.settimeout(10);
                   clientSocket.connect(sockadress);
                  except socket.error as msg:
                   print("Could not open socket");
                   break
 
 # SEND DATA
 frame_hex = binascii.hexlify(frame_bytes);

 #print(frame_hex);
 #print(len(frame_bytes));

 clientSocket.sendall(frame_bytes);
 
 ok=False;
 while (not ok):
  try:
   data = clientSocket.recv(1024);
   ok=True
   try:
    data
   except:
    print("No data - Die")
    sys.exit(1) #die, no data
  except socket.timeout as msg:
   print("Connection timeout");
   sys.exit(1) #die
 print("data len",len(data)) 
 # PARSE RESPONSE (start position 56, end position 60)
 totalpower=0 
 i=pfin-pini
 a=0
 while a<=i:
  p1=56+(a*4)
  p2=60+(a*4)
  response=twosComplement_hex(str(''.join(hex(ord(chr(x)))[2:].zfill(2) for x in bytearray(data))+'  '+re.sub('[^\x20-\x7f]', '', ''))[p1:p2])
  hexpos=str("0x") + str(hex(a+pini)[2:].zfill(4)).upper()

  with open("DYRealTime.txt") as txtfile:
   parameters=json.loads(txtfile.read())
  for parameter in parameters:
   for item in parameter["items"]:
     title=item["titleEN"]
     ratio=item["ratio"]
     unit=item["unit"]
     for register in item["registers"]:
      if register==hexpos and chunks!=-1:
       #print(int(hexpos,0)," ",hexpos+"-"+title+":"+str(response*ratio)+unit)
       if title.find("Temperature")!=-1: 
        response=round(response*ratio-100,2)
       else: 	
        response=round(response*ratio,2)
        #if title=="PV1 Power":
        print(chunks," ",int(hexpos,0)," ",hexpos+"-"+title+":"+str(response*ratio)+unit,p1/2,p2/2)
        #print(chunks," ",title,"response", response,"p1",p1,"p2",p2,"ratio",ratio)
        #print("chunks ",chunks," data len",len(data),"p1/2",p1/2,"p2/2",p2/2);
       output=output+"\""+ title + "(" + unit + ")" + "\":" + str(response)+","
       if hexpos=='0x00BA': totalpower+=response*ratio;
       if hexpos=='0x00BB': totalpower+=response*ratio; 
  a+=1
 pini=150
 pfin=195
 chunks+=1  
output=output[:-1]+"}"
if totalpower<installed_power+1000:
  #print(output)
  print(" ");
