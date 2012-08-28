package transcode;

public class Transcode {
	  public static byte[] flv2mp3(byte[] src, int src_len) {
		    return TranscodeJNI.flv2mp3(src, src_len);
		  }
	  public static byte[] raw2flv(byte[] src, int src_len) {
		    return TranscodeJNI.raw2flv(src, src_len);
		  }
	  public static byte[] multiflv2mp3(byte[] src, int[] src_len, int len) {
		    return TranscodeJNI.multiflv2mp3(src, src_len, len);
		  }
}
