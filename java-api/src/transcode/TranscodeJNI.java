package transcode;

public class TranscodeJNI {
	public final static native byte[] flv2mp3(byte[] jarg1, int jarg2);
	public final static native byte[] raw2flv(byte[] jarg1, int jarg2);
	public final static native byte[] multiflv2mp3(byte[] jarg1, int[] jarg2, int jarg3);
}
