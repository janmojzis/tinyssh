import binascii

f = open("precomp.txt")

def l(line):

        l = ""
        for i in range(32):
                if i == 31:
                        l += "0x%s%s" % (line[2*i], line[2*i+1])
                else:
                        l += "0x%s%s," % (line[2*i], line[2*i+1])
        return l
        

while True:
        line=f.readline()
        if len(line) == 0:
                f.close()
                break
        print "{", l(line), "},"
