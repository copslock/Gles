package com.app.jagles.audio;

public class EncoderAudio {

	static {
		System.loadLibrary("encoderaudio");
	}

	public EncoderAudio() {

	}

	public static native byte[] Pcm2G711(byte[] pcm, byte[] g711, int size);
}
