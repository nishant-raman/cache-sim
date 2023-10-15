import re

#exp_num = [1, 2, 3, 4, 5]
exp_num = [5]
#block_sizes = [16, 32, 64, 128]
block_sizes = [16, 32]
l1_sizes = [1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576]
l1_assoc = [1, 2, 4, 8, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768]
#l2_sizes = [0, 16384, 32768, 65536]
#l2_assoc = [0, 8]
l2_sizes = [0]
l2_assoc = [0]
pfn = [0, 1, 2, 3, 4]
pfm = [0, 4]

#sizes = [1024, 2048, 4096, 8192, 16384, 32768]
#assoc = 4
#block_sizes = [16, 32, 64, 128]

#l1_size = [1024, 2048, 4096, 8192]
#l2_size = [16384, 32768, 65536]

#pfn = [0, 1, 2, 3, 4]
#pfm = 4


print("Opening CSV...")
csv = open("re.csv", "a")

for e in exp_num:
	for bs in block_sizes:
		for s1 in l1_sizes:
			for a1 in l1_assoc:
				for s2 in l2_sizes: 
					for a2 in l2_assoc:
						for n in pfn:
							for m in pfm:
								filename = "gcc"+str(e)+"."+str(bs)+"_"+str(s1)+"_"+str(a1)+"_"+str(s2)+"_"+str(a2)+"_"+str(n)+"_"+str(m)+".txt"
								#print("Searching for file... : ", filename)
								try:
									with open(filename, "r") as f:
										print("Found file :", filename)
										for line in f:
											srch = re.search(r"L1 miss rate:\s+(\d+\.\d+)", line)
											if srch:
												mr1 = srch.group(1)
												continue
											srch = re.search(r"L2 miss rate:\s+(\d+\.\d+)", line)
											if srch:
												mr2 = srch.group(1)
												break

										data = str(bs)+","+str(s1)+","+str(a1)+","+str(s2)+","+str(a2)+","+str(n)+","+str(m)+","+str(mr1)+","+str(mr2)+"\n"
										print("Writing to CSV: ", data)
										csv.write(data)
								
								except:
									continue
								
								#for line in f:
								#	m = re.search(r"L1 miss rate:\s+(\d\.\d+)", line)
								#	if m:
								#		mr1 = m.group(1)
								#		continue
								#	m = re.search(r"L2 miss rate:\s+(\d\.\d+)", line)
								#	if m:
								#		mr2 = m.group(1)
								#		break
	
								#f.close()
		
csv.close()

#for l1 in l1_size:
#	for l2 in l2_size:
#		f = open("gcc4.32_"+str(l1)+"_4_"+str(l2)+"_8.txt", "r")
#		for line in f:
#			m = re.search(r"L1 miss rate:\s+(\d+\.\d+)", line)
#			if m:
#				mr1 = m.group(1)
#				continue
#			m = re.search(r"L2 miss rate:\s+(\d+\.\d+)", line)
#			if m:
#				mr2 = m.group(1)
#				break
#		csv.write("32,"+str(l1)+",4,"+str(l2)+",8,"+str(mr1)+","+str(mr2)+"\n")
		#csv.write(str(bs)+","+str(s)+","+str(assoc)+","+str(mr1)+"\n")
#		f.close()

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
	#csv.write("32,"+str(s)+","+str(fa)+","+str(mr1)+","+str(mr2)+"\n")
	#f.close()

#for n in pfn:
#	f = open("gcc5.16_1024_1_"+str(n)+"_4.txt", "r")
#	for line in f:
#		m = re.search(r"L1 miss rate:\s+(\d+\.\d+)", line)
#		if m:
#			mr = m.group(1)
#		#	continue
#		#m = re.search(r"L2 miss rate:\s+(\d+\.\d+)", line)
#		#if m:
#		#	mr2 = m.group(1)
#		#	break
#	#csv.write("32,"+str(l1)+",4,"+str(l2)+",8,"+str(mr1)+","+str(mr2)+"\n")
#	#csv.write(str(bs)+","+str(s)+","+str(assoc)+","+str(mr1)+"\n")
#	csv.write("16,1024,1,"+str(n)+",4,"+str(mr)+"\n")
#	f.close()

#csv.close()

		
