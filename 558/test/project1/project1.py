import sys
from p1_head import *

s0=s1=s2=s3=s4=s5=s6=s7=s8=s9=s10=s11=s12=s13=s14=s15=0
R1=R2=R3=0

def initialization(K,IV):
	global s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15
	global R1,R2,R3
	k0 = K[0]
	k1 = K[1]
	k2 = K[2]
	k3 = K[3]
	IV0 = IV[0]
	IV1 = IV[1]
	IV2 = IV[2]
	IV3 = IV[3]
	s15 = k3^IV0
	s14 = k2
	s13 = k1
	s12 = k0^IV1
	s11 = k3^0xffffffff
	s10 = k2^0xffffffff^IV2
	s9 = k1^0xffffffff^IV3
	s8 = k0^0xffffffff
	s7 = k3
	s6 = k2
	s5 = k1
	s4 = k0
	s3 = k3^0xffffffff
	s2 = k2^0xffffffff
	s1 = k1^0xffffffff
	s0 = k0^0xffffffff
	R1 = R2 = R3 = 0x00000000
	i = 0
	
	while(i<32):
		#print(hex(R1),hex(R2),hex(R3),hex(s0),hex(s1),hex(s2),hex(s3),hex(s4),hex(s5),hex(s6),hex(s7),hex(s8),hex(s9),hex(s10),hex(s11),hex(s12),hex(s13),hex(s14),hex(s15))
		F = (addmod(s15,R1))^R2
		#print(hex(R2),hex(R3^s5),hex(R2 + (R3^s5)))
		r = addmod(R2,(R3^s5))
		#print(hex(r))
		
		R3 = S2(R2)
		
		R2 = S1(R1)
		
		R1 = r
		#print(hex(R1))
		s00,s01,s02,s03 = split4(s0)
		#print(hex(s00),hex(s01),hex(s02),hex(s03))
		s110,s111,s112,s113 = split4(s11)
		#print(hex(s110),hex(s111),hex(s112),hex(s113))
		v = (concat(concat(concat(s01,s02),s03),0x00)) ^ (MULalpha(s00)) ^s2^ (concat(concat(concat(0x00,s110),s111),s112))^(DIValpha(s113))^F
		s0=s1
		s1=s2
		s2=s3
		s3=s4
		s4=s5
		s5=s6
		s6=s7
		s7=s8
		s8=s9
		s9=s10
		s10=s11
		s11=s12
		s12=s13
		s13=s14
		s14=s15
		s15=v
		i+=1
		#print(hex(s0))
		


#print(s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15)

def generate_keystream(n,KS):
	global s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15
	global R1,R2,R3
	
	r = addmod(R2,(R3^s5))
	R3 = S2(R2)
	R2 = S1(R1)
	R1 = r
	s00,s01,s02,s03 = split4(s0)
	s110,s111,s112,s113 = split4(s11)
	v = concat(concat(concat(s01,s02),s03),0x00) ^ MULalpha(s00) ^s2^ concat(concat(concat(0x00,s110),s111),s112)^DIValpha(s113)
	s0=s1
	s1=s2
	s2=s3
	s3=s4
	s4=s5
	s5=s6
	s6=s7
	s7=s8
	s8=s9
	s9=s10
	s10=s11
	s11=s12
	s12=s13
	s13=s14
	s14=s15
	s15=v
	
	i=0
	while(i<n):
		F = addmod(s15,R1)^R2
		r = addmod(R2,(R3^s5))
		R3 = S2(R2)
		R2 = S1(R1)
		R1 = r
		zt = F^s0
		s00,s01,s02,s03 = split4(s0)
		s110,s111,s112,s113 = split4(s11)
		v = concat(concat(concat(s01,s02),s03),0x00) ^ MULalpha(s00) ^s2^ concat(concat(concat(0x00,s110),s111),s112)^DIValpha(s113)
		s0=s1
		s1=s2
		s2=s3
		s3=s4
		s4=s5
		s5=s6
		s6=s7
		s7=s8
		s8=s9
		s9=s10
		s10=s11
		s11=s12
		s12=s13
		s13=s14
		s14=s15
		s15=v
		KS.append(zt)
		i+=1

t = []
with open(sys.argv[1]) as myfile:
	for line in myfile:
		t.append(line)

key = []
var = []

i = 1
j = 1

while(i<len(t[0].split())):
	key.append(int(t[0].split()[i],16))
	i+=1
while(j<len(t[1].split())):
	var.append(int(t[1].split()[j],16))
	j+=1
n = int(t[2].split()[1])


#k1 = [0xaaaaaaaa,0x1234bbbb,0xbbbbbbbb,0xcccccccc]
#v1 = [0xabcdabcd,0x11111111,0xabcdabcd,0x22222222]

KS=[]
initialization(key,var)
generate_keystream(n,KS)

f = open("output.txt","w")
for l in KS:
	f.write('{0:0{1}x}'.format(l,8)+"\n")
f.close()
print("The output keystreams are:")

for l in KS:
	print('{0:0{1}x}'.format(l,8))

