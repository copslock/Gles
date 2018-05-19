package com.app.jagles.audio;

import android.media.AudioManager;
import android.media.AudioTrack;

public class VideoAudioTrack {
	int mFrequency; 	//采样率
	
	int mChannel; 		//声道
	
	int mSampBit; 		//采样精度
	AudioTrack mAudioTrack;
	
	public VideoAudioTrack(int frequency, int channel, int sambit){
		mFrequency = frequency;
		mChannel = channel;
		mSampBit = sambit;
	}
	
	public void init(){
		if(mAudioTrack != null){
			mAudioTrack.release();
		}
		int minBufSize = AudioTrack.getMinBufferSize(mFrequency, mChannel, mSampBit);
		
		mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,mFrequency,
				mChannel, mSampBit, minBufSize*2, AudioTrack.MODE_STREAM);
	}
	
	public void release(){
		if(mAudioTrack != null){
			mAudioTrack.release();
		}
	}
	
	public synchronized void playAudioTrack(byte[] data, int offset, int length){
		if(data == null || data.length == 0){
			return;
		}
		try{
			mAudioTrack.write(data, offset, length);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	public void stop(){
		if(mAudioTrack != null)
			mAudioTrack.stop();
	}
	
	public void play(){
		if(mAudioTrack != null)
			mAudioTrack.play();
	}
	
	public int getPrimePlaySize(){
		int minBufSize = AudioTrack.getMinBufferSize(mFrequency, mChannel, mSampBit);
		return minBufSize*2;
	}
}
