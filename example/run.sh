if [ -z "$HOME" ]; then
        echo "Please setup the HOME environment parameter. For example,"
        echo "export HOME=/home/yourname"
        exit
fi

#prepare the test data
if [ ! -d "testdata/2097152" ]; then
	echo "Generate test data for the initial run."
	cd testdata
	mkdir 2097152
	cd 2097152
	ruby ../genNumber.rb 2097152
	cd ..
	cd ..
fi

javac SortArray.java
time java -Xbootclasspath/p:$HOME/.hcode/ip-drivers/ip-mergesorter-32bit/rt_mergetree SortArray 2097152 1 ./testdata/2097152/JAVA.data
