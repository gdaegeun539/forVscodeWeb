package org.tukorea.pattern;

/*
 * 클라이언트가 기본적으로 사용하는  MediaPlayer 인터페이스 
 * Target
 */
interface MediaPlayer {
   public void play(String audioType, String fileName);
}

/*
 * 새로 추가된 음원을 지원하는  AdvancedMediaPlayer 인터페이스 
 * Target(기본 호환 불가)
 */
interface AdvancedMediaPlayer {	
   public void playMp4(String fileName);
}

/*
 * AdvancedMediaPlayer 인터페이스를 구현한  Mp4Player 구현 클래스
 * Adaptee(기본 호환 불가)
 */
class Mp4Player implements AdvancedMediaPlayer{

   @Override
   public void playMp4(String fileName) {
      System.out.println("Playing mp4 file. Name: "+ fileName);		
   }
}

/*
 * MediaPlayer 인터페이스를 구현한  MediaAdapter 구현 클래스
 * MediaAdapter 클래스는 내부적으로 클라이언트가 원하는 인터페이스를 조정할 수 있다.
 * Adapter
 */
class MediaAdapter implements MediaPlayer {

   AdvancedMediaPlayer advancedMusicPlayer;

   public MediaAdapter(String audioType){
   
	   if (audioType.equalsIgnoreCase("mp4")){
         advancedMusicPlayer = new Mp4Player();
      }	
   }

   @Override
   public void play(String audioType, String fileName) {
   
	  if(audioType.equalsIgnoreCase("mp4")){
         advancedMusicPlayer.playMp4(fileName);
      }
   }
}

/*
 * MediaPlayer 인터페이스를 구현한  AudioPlayer 구현 클래스
 * 호환되지 않는 인터페이스로 기존 클래스를 사용해야하는 경우 MediaAdapter 클래스를 통해 문제 해결할 수 있다.
 * Adapter
 */
class AudioPlayer implements MediaPlayer {
   MediaAdapter mediaAdapter; // Adapter

   @Override
   public void play(String audioType, String fileName) {		
      if(audioType.equalsIgnoreCase("mp3")){
         System.out.println("Playing mp3 file. Name: " + fileName);			
      } 
      else if( audioType.equalsIgnoreCase("mp4")){
         mediaAdapter = new MediaAdapter(audioType);
         mediaAdapter.play(audioType, fileName); // Adaptee - specificRequest
      }
      else{
         System.out.println("Invalid media. " + audioType + " format not supported");
      }
   }   
}


/**
 * Client
 * PSVM
 */
public class AdapterPattern {
	 public static void main(String[] args) {  
 		  MediaPlayer audioPlayer = new AudioPlayer();
	      audioPlayer.play("mp3", "over the horizon.mp3");
	      audioPlayer.play("mp4", "alone.mp4");
	      audioPlayer.play("avi", "mind me.avi");
	 }
}
