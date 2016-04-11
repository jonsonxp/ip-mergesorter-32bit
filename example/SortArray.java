import java.util.Arrays;
import java.io.File;
import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

class SortArray {
	private static int N;

	public static void main(String[] args)
	{
		N = Integer.parseInt(args[0]);
        	int[] iArr = new int[N]; // Input array
		int[] oArr = new int[N]; //Output array

		long timeMainIn = System.currentTimeMillis();
 
		SortArray arr = new SortArray();
		int counter = 0;
	    try{
			BufferedReader br = new BufferedReader(new FileReader(args[2]));
			StringBuilder sb = new StringBuilder();
			String line = br.readLine();
			while (line != null) {
				iArr[counter] = Integer.parseInt(line);
			counter++;
				line = br.readLine();
			}
			br.close();
		}
		catch(IOException e){
			System.out.println(e);
		}

		System.out.println(counter);
		long timeReadEnd = System.currentTimeMillis();
		System.out.println("Time before jni: "+(timeReadEnd));

	        System.out.println("java Arrays.sort");
 		Arrays.sort(iArr);

		long timeSortEnd = System.currentTimeMillis();

		if(Integer.parseInt(args[1]) == 1){
			System.out.println("Sorted array: ");
			for(int i = 0; i < counter; i++){
				System.out.println(iArr[i]);
			}
		}

		System.out.println("Read time: "+(timeReadEnd - timeMainIn));
  		System.out.println("Sort time: "+(timeSortEnd - timeReadEnd));
 		System.out.println("All time: "+(timeSortEnd - timeMainIn));
 		System.out.println("Time END: "+System.currentTimeMillis());

	}

}
