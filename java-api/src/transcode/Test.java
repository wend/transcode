package transcode;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Date;

public class Test {
	public static byte[] loadFile(String filename)
	{
		byte[] src = null;
		try {
			FileInputStream inputStream = new FileInputStream(filename);
			src = new byte[(int) inputStream.getChannel().size()];
			inputStream.read(src);
			inputStream.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return src;
	}
	
	public static void writeFile(String filename, byte[] data) {
		try {
			FileOutputStream outputStream = new FileOutputStream(filename);
			outputStream.write(data);
			outputStream.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public static void main(String argv[]) {
		System.loadLibrary("transcode");
		//String dllPath = System.getProperty("user.dir") + "/transcode.dll";
		//System.out.println(dllPath);
		//System.load(dllPath);
		
		Date date = new Date();
		long startTime = date.getTime();
		System.out.println(date.toString());
		String [] files = new String[50];//{"d:\\test\\20.flv", "d:\\test\\test.flv"};
		for(int i = 0; i<50; i++)
		{
			files[i] = "d:\\test\\20.flv";
		}
		int[] src_len = new int[files.length];
		String tmpStr = new String();
		byte[] src = null;
		for (int i = 0; i < files.length; i++)
		{
			byte[] tmp = loadFile(files[i]);
			// for one flv file to one mp3 file
			byte[] out_buf = Transcode.flv2mp3(tmp, tmp.length);
			writeFile(files[i] + ".mp3", out_buf);
			src_len[i] = tmp.length;
			tmpStr += new String(tmp);
		}
		src = tmpStr.getBytes();
		// for several flv files to one mp3 file
		byte[] dstByte = Transcode.multiflv2mp3(src, src_len, files.length);
		System.out.println("mp3 file size: " + dstByte.length);
		
		writeFile("d:\\test\\out.mp3", dstByte);
		Date endDate = new Date();
		long endTime = endDate.getTime();
		System.out.println(endTime - startTime);

		src = loadFile("d:\\test\\test.raw");
		dstByte = Transcode.raw2flv(src, src.length);
		writeFile("d:\\test\\test.raw.flv", dstByte);
	}
}
