
if [ -z "$JAVA_HOME" ]; then
	echo "Please setup the JAVA_HOME environment parameter. For example,"
	echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/"
	exit
fi

#compile new Arrays.java
javac Arrays.java
cd ../..
#generate header file for JNI
javah java.util.Arrays
cd java/util
cp ../../*.h .
#compile the JNI IP driver library
gcc -O3 -fPIC -shared sortlib.c -I $JAVA_HOME/include -I $JAVA_HOME/include/linux/ -o libjava_util_Arrays.so
#copy the driver library to jdk library folder
sudo cp libjava_util_Arrays.so $JAVA_HOME/jre/lib/amd64
cd ../../..
