inputL = 42
filterL = 32
inputOffset = 0;
stop = inputL-filterL; 
for j in range(stop):
		result = 0;
		smallstop = filterL;
		for i in range(smallstop):
		    print(i, (i+inputOffset+j+1)%inputL)

