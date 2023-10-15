import re

#sizes = [1024, 2048, 4096, 8192]
#assoc = [1, 2, 4, 8]
#sizes = [1024, 2048, 4096, 8192, 16384, 32768]
#assoc = 4
#block_sizes = [16, 32, 64, 128]

l1_size = [1024, 2048, 4096, 8192]
l2_size = [16384, 32768, 65536]

csv = open("re1.csv", "a")

for l1 in l1_size:
	for l2 in l2_size:
		f = open("gcc4.32_"+str(l1)+"_4_"+str(l2)+"_8.txt", "r")
		for line in f:
			m = re.search(r"L1 miss rate:\s+(\d+\.\d+)", line)
			if m:
				mr1 = m.group(1)
				continue
			m = re.search(r"L2 miss rate:\s+(\d+\.\d+)", line)
			if m:
				mr2 = m.group(1)
				break
		csv.write("32,"+str(l1)+",4,"+str(l2)+",8,"+str(mr1)+","+str(mr2)+"\n")
		#csv.write(str(bs)+","+str(s)+","+str(assoc)+","+str(mr1)+"\n")
		f.close()

	#fa = int(s/32)
	#f = open("gcc2.32_"+str(s)+"_"+str(fa)+"_16384_8.txt", "r")
	#for line in f:
	#	m = re.search(r"L1 miss rate:\s+(\d+\.\d+)", line)
	#	if m:
	#		mr1 = m.group(1)
	#	#	continue
	#	#m = re.search(r"L2 miss rate:\s+(\d+\.\d+)", line)
	#	#if m:
	#	#	mr2 = m.group(1)
	#		break
	#csv.write("32,"+str(s)+","+str(fa)+","+str(mr1)+","+str(mr1)+"\n")
	#f.close()

csv.close()

		
