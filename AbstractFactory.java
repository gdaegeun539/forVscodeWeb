package org.tukorea.pattern;

/*
 * Player 인터페이스, CDPlayer, RecordPlayer 구현 클래스  -> FactoryMethod.java 
 */

/*
 * Media 인터페이스
 */
interface Media {
}
/*
 * CD 구현 클래스
 */
class CD implements Media {
    public CD(){
        System.out.println("CD 생성");
    }
}
/*
 * Album 구현 클래스
 */
class Album implements Media {
    public Album(){
        System.out.println("Album 생성");
    }
}
/*
 * 음악 재생 책임을 가진 MusicFactory 추상 클래스
 */
abstract class MusicFactory {
	public abstract Player createPlayer();
	public abstract Media createMedia();
}
/*
 * 디지털 음악 재생 객체 생성 책임을 가진 DigitalMusicFactory 클래스
 */
class DigitalMusicFactory extends MusicFactory {
    @Override
    public Player createPlayer() {
    	return new CDPlayer();
    }

	@Override
	public Media createMedia() {
		// TODO Auto-generated method stub
		return new CD();
	}
}
/*
 * 아날로그 음악 재생 객체  생성 책임을 가진 AnalogMusicFactory 클래스
 */
class AnalogMusicFactory extends MusicFactory {
    @Override
    public Player createPlayer() {
    	return new RecordPlayer();
    }

	@Override
	public Media createMedia() {
		// TODO Auto-generated method stub
		return new Album();
	}
}
/*
 * 음악 재생 객체  생성 책임을 가진 FactoryOfMusicFactory 클래스
 */
class FactoryOfMusicFactory {
	public MusicFactory createMusic(String type){
		MusicFactory musicFactory = null;
        switch (type){
            case "analog":
            	musicFactory = new AnalogMusicFactory();
                break;

            case "digital":
            	musicFactory = new DigitalMusicFactory();
                break;
        }
        if(musicFactory != null) {
        	musicFactory.createMedia();
        	musicFactory.createPlayer();
        }
        return musicFactory;
    }
}

public class AbstractFactory {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		FactoryOfMusicFactory factoryOfMusicFactory = new FactoryOfMusicFactory();
		factoryOfMusicFactory.createMusic("analog");
		factoryOfMusicFactory.createMusic("digital");
	
	}

}
