package org.tukorea.pattern;

/*
 * 집합 클래스의 자료 구조와 상관없이 집합에 소속된 요소들을 쉽게 접근하기 위한 Iterator 인터페이스 
 */
interface Iterator {  
    public boolean hasNext();  
    public Object next();  
}

/*
 * 다양한 자료 구조를 갖는 데이터를 저장하는 집합 클래스
 */
interface Container {  
    public Iterator getIterator();  
}

/*
 * 영화 정보를 저장하는 MovieCollection 집합 클래스
 */
class MovieCollection implements Container {
    public String movie[]={"Usual Suspect", "Ghost Busters","God Father","Back to the future",
    		              "American Beauty", "the silence of the lambs", "alien", "starwars"};   
   
    @Override  
    public Iterator getIterator() {  
        return new MovieCollectionIterator() ;  
    }
    // MovieCollection 집합 클래스의 요소에 순차적으로 접근하도록 Iterator 인터페이스를 구현
    private class MovieCollectionIterator implements Iterator{  
        int n;  
        
        @Override  
        public boolean hasNext() {  
            if ( n < movie.length){  
                return true;  
            }  
            return false;  
        }  
        
        @Override  
        public Object next() {  
            if(this.hasNext()){  
                return movie[ n++ ];  
            }  
            return null;      
        }  
    }  
}

public class IteratorPattern {
	 public static void main(String[] args) {  
		 	MovieCollection movieRepository = new MovieCollection();  
	            
	        for(Iterator iterator = movieRepository.getIterator(); iterator.hasNext();){  
	            String item = (String)iterator.next();  
	            System.out.println("Movie : " + item);  
	        }      
	    }  
}