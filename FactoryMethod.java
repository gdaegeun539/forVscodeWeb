package org.tukorea.pattern;


/*
 * Player 인터페이스
 */
interface Player {
}
/*
 * CDPlayer 구현 클래스
 */
class CDPlayer implements Player {
    public CDPlayer(){
        System.out.println("CDPlayer 생성");
    }
}
/*
 * RecordPlayer 구현 클래스
 */
class RecordPlayer implements Player {
    public RecordPlayer(){
        System.out.println("RecodPlayer 생성");
    }
}
/*
 * Player 객체를 생성할 책임을 가진 PlayerFactory 클래스
 */
class PlayerFactory {
    public Player createPlayer(String type){
    	Player player = null;
        switch (type){
            case "CD":
            	player = new CDPlayer();
                break;

            case "Record":
            	player = new RecordPlayer();
                break;
        }

        return player;
    }
}

public class FactoryMethod {

	public static void main(String[] args) {
		PlayerFactory musicFactory = new PlayerFactory();
		musicFactory.createPlayer("CD");
		musicFactory.createPlayer("Record");
	}

}
