package main;
import java.io.*;

public class CopiaMigliorato {
	
	public static void convertiInTxt(String path) {
		String[] dst = path.split("\\.");
		try {
			System.out.println(dst[0] + dst[1]);
			BufferedWriter scrivi = new BufferedWriter(new FileWriter("output/" + dst[0] + "_" + dst[1] + ".txt"));
			FileInputStream source = new FileInputStream("input/" + path);
			while(true) {
				
				int dato = source.read();
				scrivi.append(dato + System.lineSeparator());
				
				if(dato == -1) 
					break; //fine file
			}//for
			source.close();
			scrivi.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static void main(String[]args){
		File folder = new File("input/");
		File[] listOfFiles = folder.listFiles();

		for (int i = 0; i < listOfFiles.length; i++) {
			if (listOfFiles[i].isFile() && listOfFiles[i].getName().startsWith("test")) {
				System.out.println(listOfFiles[i].getName());
				convertiInTxt(listOfFiles[i].getName());
			}
		}
	}
}
