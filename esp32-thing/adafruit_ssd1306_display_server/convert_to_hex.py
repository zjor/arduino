print("P(static_index) = {")

i = 0
with open("index.html", "r") as f:
	for c in f.read():
		v = f"0x{format(ord(c),'02X')}"
		print(v, end=', ')
		i += 1
		if i % 25 == 0:
			print()

print("};")
print(f"// len = {i}")

