import sys


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

print(n)
