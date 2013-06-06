// libtest.java      GSN    03 Oct 08; 21 Feb 12
// 

import java.util.LinkedList;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.ListIterator;

interface Functor { Object fn(Object x); }

interface Predicate { boolean pred(Object x); }

public class libtest {

    // ****** your code starts here ******

/**
 * Question No. 1
 * @param lst a LinkedList of integers
 * @return the sum of those integers
 */
public static Integer sumlist(LinkedList<Integer> lst) {
	Integer sum = 0;
	for (Iterator<Integer> i = lst.iterator(); i.hasNext();)
		sum += i.next();
	return sum;
		
}

/**
 * Question No. 1
 * @param lst a ArrayList of integers
 * @return the sum of those integers
 */
public static Integer sumarrlist(ArrayList<Integer> lst) {
	Integer sum = 0;
	for (Iterator<Integer> i = lst.iterator(); i.hasNext();)
		sum += i.next();
	return sum;
}

/**
 * Question No. 2
 * @param p a predicate
 * @param lst a  LinkedList of Objects
 * @return a new LinkedList that contains the Objects that pass the predicate
 */
public static LinkedList<Object> subset (Predicate p,
                                          LinkedList<Object> lst) {
	LinkedList<Object> newlist = new LinkedList<Object>();
	for(Iterator<Object> i = lst.iterator(); i.hasNext();){
		Object next = i.next();
		if(p.pred(next))
				newlist.addLast(next);
	}
	return newlist;
}

/**
 * Question No. 3
 * @param p a predicate
 * @param lst a LinkedList of objects
 * @return a possible reduced LinkedList that only contains the Objects that pass the predicate
 */
public static LinkedList<Object> dsubset (Predicate p,
                                           LinkedList<Object> lst) {
	for(Iterator<Object> i = lst.iterator(); i.hasNext();)
		if(!p.pred(i.next()))
			i.remove();
	return lst;
}

/**
 * Question No. 4
 * @param p a predicate
 * @param lst a LinkedList of objects
 * @return the first item in the LinkedList that satisfies the predicate
 */
public static Object some (Predicate p, LinkedList<Object> lst) {
	for(Iterator<Object> i = lst.iterator(); i.hasNext();){
		Object next = i.next();
		if(p.pred(next))
			return next;
	}
	return null;
}

/**
 * Question No. 5
 * @param f a functor
 * @param lst a LinkedList of objects
 * @return a new LinkedList containing the results of applying the functor to each item in the LinkedList
 */
public static LinkedList<Object> mapcar (Functor f, LinkedList<Object> lst) {
	LinkedList<Object> newlist = new LinkedList<Object>();
	for(Iterator<Object> i = lst.iterator(); i.hasNext();)
		newlist.addLast(f.fn(i.next()));
	return newlist;
}

/**
 * Question No. 6
 * @param lsta a LinkedList of objects
 * @param lstb a LinkedList of objects
 * @return a new LinkedList which is a combination of the two list in ascending order
 */
public static LinkedList<Object> merge (LinkedList<Object> lsta,
                                        LinkedList<Object> lstb) {
	LinkedList<Object> newlist = new LinkedList<Object>();
	return mergeb(lsta, lstb, newlist, 0, 0);
}

//helper function
private static LinkedList<Object> mergeb (LinkedList<Object> lsta,
        LinkedList<Object> lstb, LinkedList<Object> newlist, int aindex, int bindex) {
	//base cases:
	if(aindex >= lsta.size()){
		while(bindex < lstb.size())
			newlist.addLast(lstb.get(bindex++));
		return newlist;
	}
	else if(bindex >= lstb.size()){
		while(aindex < lsta.size())
			newlist.addLast(lsta.get(aindex++));
		return newlist;
	}
	//recursive cases:
	else if(((Comparable)lsta.get(aindex)).compareTo(lstb.get(bindex)) < 0){
		newlist.addLast(lsta.get(aindex));
		return mergeb(lsta, lstb, newlist, ++aindex, bindex);	
	}
	else{
		newlist.addLast(lstb.get(bindex));
		return mergeb(lsta, lstb, newlist, aindex, ++bindex);
	}
	
}

/**
 * Question No. 7
 * @param lst a LinkedList of objects
 * @return a sorted list
 */
public static LinkedList<Object> sort (LinkedList<Object> lst) {
	if(lst.size() == 1){
		return lst;
	}
	else{
		LinkedList<Object> firsthalf = sublist(lst, 0, lst.size()/2);
		LinkedList<Object> secondhalf = sublist(lst, lst.size()/2, lst.size());
		return merge(sort(firsthalf), sort(secondhalf));
	}
}

private static LinkedList<Object> sublist(LinkedList<Object> lst, int start, int end){
	LinkedList<Object> newlist = new LinkedList<Object>();
	for(; !lst.isEmpty() && start < end; start++)
		newlist.addLast(lst.get(start));
	return newlist;
		
}

/**
 * Question No. 8
 * @param lsta a LinkedList represents a set of objects
 * @param lstb a LinkedList represents a set of objects
 * @return a LinkedList represents the intersection of those two sets
 */
public static LinkedList<Object> intersection (LinkedList<Object> lsta,
                                               LinkedList<Object> lstb) {
	LinkedList<Object> sorta = sort(lsta);
	LinkedList<Object> sortb = sort(lstb);
	LinkedList<Object> newlist = new LinkedList<Object>();
	return intersectionb(sorta, sortb, newlist, 0, 0);
}

//helper function
private static LinkedList<Object> intersectionb (LinkedList<Object> lsta,
        LinkedList<Object> lstb, LinkedList<Object> newlist, int aindex, int bindex) {
	//base cases:
	if(aindex >= lsta.size() || bindex >= lstb.size())
		return newlist;
	//recursive case:
	else if(lsta.get(aindex).equals(lstb.get(bindex))){
		newlist.addLast(lsta.get(aindex));
		return intersectionb(lsta, lstb, newlist, ++aindex, ++bindex);
	}
	else{
		return intersectionb(lsta, lstb, newlist, ++aindex, bindex);
	}
}

/**
 * Question No. 9
 * @param lst a LinkedList of objects
 * @return a new list in the reversed order of the list
 */
public static LinkedList<Object> reverse (LinkedList<Object> lst) { 
	LinkedList<Object> newlist = new LinkedList<Object>();
	for(Iterator<Object> i = lst.iterator(); i.hasNext();)
		newlist.addFirst(i.next());
	return newlist;
}

    // ****** your code ends here ******

    public static void main(String args[]) {
        LinkedList<Integer> lst = new LinkedList<Integer>();
        lst.add(new Integer(3));
        lst.add(new Integer(17));
        lst.add(new Integer(2));
        lst.add(new Integer(5));
        System.out.println("lst = " + lst);
        System.out.println("sum = " + sumlist(lst));

        ArrayList<Integer> lstb = new ArrayList<Integer>();
        lstb.add(new Integer(3));
        lstb.add(new Integer(17));
        lstb.add(new Integer(2));
        lstb.add(new Integer(5));
        System.out.println("lstb = " + lstb);
        System.out.println("sum = " + sumarrlist(lstb));

        final Predicate myp = new Predicate()
            { public boolean pred (Object x)
                { return ( (Integer) x > 3); }};

        LinkedList<Object> lstc = new LinkedList<Object>();
        lstc.add(new Integer(3));
        lstc.add(new Integer(17));
        lstc.add(new Integer(2));
        lstc.add(new Integer(5));
        System.out.println("lstc = " + lstc);
        System.out.println("subset = " + subset(myp, lstc));

        System.out.println("lstc     = " + lstc);
        System.out.println("dsubset  = " + dsubset(myp, lstc));
        System.out.println("now lstc = " + lstc);

        LinkedList<Object> lstd = new LinkedList<Object>();
        lstd.add(new Integer(3));
        lstd.add(new Integer(17));
        lstd.add(new Integer(2));
        lstd.add(new Integer(5));
        System.out.println("lstd = " + lstd);
        System.out.println("some = " + some(myp, lstd));

        final Functor myf = new Functor()
            { public Integer fn (Object x)
                { return new Integer( (Integer) x + 2); }};

        System.out.println("mapcar = " + mapcar(myf, lstd));

        LinkedList<Object> lste = new LinkedList<Object>();
        lste.add(new Integer(1));
        lste.add(new Integer(3));
        lste.add(new Integer(5));
        lste.add(new Integer(6));
        lste.add(new Integer(9));
        System.out.println("lste = " + lste);
        LinkedList<Object> lstf = new LinkedList<Object>();
        lstf.add(new Integer(2));
        lstf.add(new Integer(3));
        lstf.add(new Integer(6));
        lstf.add(new Integer(7));
        System.out.println("lstf = " + lstf);
        System.out.println("merge = " + merge(lste, lstf));

        LinkedList<Object> lstg = new LinkedList<Object>();
        lstg.add(new Integer(39));
        lstg.add(new Integer(84));
        lstg.add(new Integer(5));
        lstg.add(new Integer(59));
        lstg.add(new Integer(86));
        lstg.add(new Integer(17));
        System.out.println("lstg = " + lstg);

        System.out.println("intersection(lstd, lstg) = "
                           + intersection(lstd, lstg));
        System.out.println("reverse lste = " + reverse(lste));

        System.out.println("sort(lstg) = " + sort(lstg));

   }
}