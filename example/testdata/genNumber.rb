require 'bindata'
#This program generate random 32bit integer numbers for test.
#
#input:
#  DataSize: how many numbers you want to generate.
#output:
#  JAVA.data: plain text file
#  FPGA.data: binary file
#
#bindata gem should be installed by commond: 
#gem install bindata

if ARGV.length != 1
	puts "usage: ruby genNumber.rb DataSize"
	exit
end

#P = 16*62500*\5
P = ARGV[0].to_i

hexValue = %w(0 1 2 3 4 5 6 7 8 9 A B C D E F)

file0 = File.open("JAVA.data", "w")
file1 = File.open("FPGA.data", "wb")
(P).times{
        o0 = 2147483648
        while o0 < -2147483648 || o0 > 2147483647 do
	newstring = ""
	8.times{newstring += hexValue[Random.rand(0 .. 15)]}
	o0 = newstring.hex
	end              
 
	file0.puts (o0)
	BinData::Int32le.new(o0).write(file1)
}
file0.close
file1.close
#
