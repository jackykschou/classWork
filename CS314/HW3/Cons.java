/**
 * this class Cons implements a Lisp-like Cons cell
 * 
 * @author  Gordon S. Novak Jr.
 * @version 29 Nov 01; 25 Aug 08; 05 Sep 08; 08 Sep 08; 12 Sep 08; 16 Feb 09
 *          01 Feb 12
 */

interface Functor { Object fn(Object x); }

interface Predicate { boolean pred(Object x); }

public class Cons
{
    // instance variables
    private Object car;
    private Cons cdr;
    private Cons(Object first, Cons rest)
       { car = first;
         cdr = rest; }
    public static Cons cons(Object first, Cons rest)
      { return new Cons(first, rest); }
    public static boolean consp (Object x)
       { return ( (x != null) && (x instanceof Cons) ); }
// safe car, returns null if lst is null
    public static Object first(Cons lst) {
        return ( (lst == null) ? null : lst.car  ); }
// safe cdr, returns null if lst is null
    public static Cons rest(Cons lst) {
      return ( (lst == null) ? null : lst.cdr  ); }
    public static Object second (Cons x) { return first(rest(x)); }
    public static Object third (Cons x) { return first(rest(rest(x))); }
    public static void setfirst (Cons x, Object i) { x.car = i; }
    public static void setrest  (Cons x, Cons y) { x.cdr = y; }
   public static Cons list(Object ... elements) {
       Cons list = null;
       for (int i = elements.length-1; i >= 0; i--) {
           list = cons(elements[i], list);
       }
       return list;
   }

    // convert a list to a string for printing
    public String toString() {
       return ( "(" + toStringb(this) ); }
    public static String toString(Cons lst) {
       return ( "(" + toStringb(lst) ); }
    private static String toStringb(Cons lst) {
       return ( (lst == null) ?  ")"
                : ( first(lst) == null ? "()" : first(lst).toString() )
                  + ((rest(lst) == null) ? ")" 
                     : " " + toStringb(rest(lst)) ) ); }

    public static int square(int x) { return x*x; }

    // ****** your code starts here ******

    //auxiliary function (recursive function that counts the number of element (Integer) in the list)
    private static int length(Cons lst){
    	return length(lst, 0);
    }
    
  //auxiliary function
    private static int length(Cons lst, int count){
    	if(first(lst) == null)
    		return count;
    	else
    		return length(rest(lst), ++count);
    }

    // iterative version
    /**
     * Question No.1
     * @param lst a list of Integer
     * @return the sum of the Integers in the list
     */
public static int sum (Cons lst) {
	//base case:
	if(lst == null)
		return 0;
	//recursive case:
	else
		return (Integer)first(lst) + sum(rest(lst));
}

    // mean = (sum x[i]) / n  
/**
 * Question No.1
 * @param lst a list of Integer
 * @return the mean of the Integers in the list
 */
public static double mean (Cons lst) {
	return sum(lst)/(double)length(lst);
}

    // square of the mean = mean(lst)^2  

    // mean square = (sum x[i]^2) / n  
/**
 * Question No.1
 * @param lst a list of Integer
 * @return the mean square of the Integers in the list
 */
public static double meansq (Cons lst) {
	return meansqHelper(lst)/length(lst);
}

//auxiliary function
private static double meansqHelper(Cons lst){
	//base case:
	if(lst == null)
		return 0;
	//recursive case:
	else
		return Math.pow((Integer)first(lst), 2) + meansqHelper(rest(lst));
}

/**
 * Question No.1
 * @param lst a list of Integers
 * @return the variance of the Integers in the list
 */
public static double variance (Cons lst) {
	return meansq(lst) - Math.pow(mean(lst), 2);
}

/**
 * Question No.1
 * @param lst a list of Integers
 * @return the standard deviation of the Integers in the list
 */
public static double stddev (Cons lst) {
	return Math.sqrt(variance(lst));
	
}

/**
 * Question No.2
 * @param x a double value
 * @return sin(x)
 */
public static double sine (double x) {
	return sineb(x, 1, x, x);
}

    // public static double sineb ( ... )
//auxiliary function
public static double sineb (double x, int order, double sum, double pre){
	//base case:
	if(order == 23)
		return sum;
	//recursive case:
	else 
		return sineb(x, order + 2, sum + (-pre * ((x*x)/((order+1)*(order+2)))), -pre * ((x*x)/((order+1)*(order+2))));
}

/**
 * Question No.3
 * @param n number of elements want to return
 * @param lst a list
 * @return a list from the first element to the n(th) element
 */
public static Cons nthcdr (int n, Cons lst) {
	//special case:
	if(length(lst) < n)
		return null;
	//base case:
	if(n == 0)
		return lst;
	//recursive case:
	else
		return nthcdr(--n, rest(lst));
		
}

/**
 * Question No.4
 * @param lst a list
 * @param n position of item in the list
 * @return n(th) item of the list
 */
public static Object elt (Cons lst, int n) {
	return(first(nthcdr(n, lst)));
}

/**
 * Question No.5
 * @param lst a list of Integers
 * @param x a double value
 * @return value of f(x)
 */
public static double interpolate (Cons lst, double x) {
	return (Integer)elt(lst, (int)x) + (x-(int)x) * ((Integer)elt(lst, 1+(int)x) - (Integer)elt(lst, (int)x));
}

/**
 * Question No.6
 * @param lst a list
 * @return the sum of all numbers in the list
 */
public static int sumtr (Cons lst) {
	return sumtrb(lst, 0);
}

//auxiliary function
private static int sumtrb (Cons lst, int sum) {
	//base case:
	if(lst == null)
		return sum;
	//recursive case:
	else if(consp(first(lst)))
		return sumtrb((Cons)first(lst), 0) + sumtrb(rest(lst), sum);
	else
		return sumtrb(rest(lst), sum + (Integer)first(lst));
}

    // use auxiliary functions as you wish.
/**
 * Question No.7
 * @param lst a list
 * @param start starting position
 * @param end ending position
 * @return the sub-sequence of the list from start to end-1
 */
public static Cons subseq (Cons lst, int start, int end) {
	
	Cons newList = copyList(lst);
	return removeBack(nthcdr(start, newList), nthcdr(start, newList), end - start);
}

//auxiliary function (return a new list that is a copy of lst)
private static Cons copyList(Cons lst){
	if(lst != null){
		Cons newList = cons(first(lst), null);
		return copyListb(newList, rest(lst), newList);
		}
	else
		return null;
}

//auxiliary function
private static Cons copyListb(Cons newList, Cons lst, Cons head){
	//base case:
	if(lst == null)
		return head;
	//recursive case:
	else{
		setrest(newList, cons(first(lst), null));
		return copyListb(rest(newList), rest(lst), head);
	}
}

//auxiliary function
private static Cons removeBack (Cons lst, Cons current, int n) {
	//base case:
	if(n == 1){
		setrest(current, null);
		return lst;
	}
	//recursive case:
	else
		return removeBack(lst, rest(current), --n);
}

/**
 * Question No.8
 * @param lst list of Integers
 * @return new list containing non-negative Integers
 */
public static Cons posfilter (Cons lst) {
	Cons newList = copyList(lst);
	while((Integer)first(newList) < 0)
		newList = rest(newList);
	return posfilterb(newList,rest(newList),newList);
}

//auxiliary function
private static Cons posfilterb(Cons lst, Cons current, Cons pre){
	//base case:
	if(current == null)
		return lst;
	//recursive case:
	if((Integer)first(current) < 0){
		setrest(pre, rest(current));
	    return posfilterb(lst, rest(current), pre);
	}
	else
		return posfilterb(lst, rest(current), rest(pre));
}

/**
 * Question No.9
 * @param p a predicate
 * @param lst a list
 * @return a list containing only values that satisfy the predicate in the list
 */
public static Cons subset (Predicate p, Cons lst) {
	Cons newList = copyList(lst);
	while(!p.pred(first(newList)))
		newList = rest(newList);
	return subsetb(p, newList, rest(newList), newList);
}

//auxiliary function
private static Cons subsetb(Predicate p, Cons lst, Cons current, Cons pre){
	//base case:
		if(current == null)
			return lst;
		//recursive case:
		if(p.pred((Integer)first(current))){
			return subsetb(p, lst, rest(current),rest(pre));
		}
		else{
			setrest(pre, rest(current));
			return subsetb(p, lst, rest(current), pre);
		}
}

 /**
  * Question No.10
  * @param f a functor
  * @param lst a list
  * @return a list containing the results of applying f.fn(item) to each item in the list.
  */
public static Cons mapcar (Functor f, Cons lst) {
	Cons newList = copyList(lst);
	return mapcarb(f, newList, newList);
}

//auxiliary function
private static Cons mapcarb(Functor f, Cons lst, Cons current){
	//base case:
	if(current == null)
		return lst;
	//recursive case:
	else{
		setfirst(current, f.fn(first(current)));
		return mapcarb(f, lst, rest(current));
	}
}

/**
 * Question No.11
 * @param p a predicate
 * @param lst a list
 * @return the first item in the list that satisfies the predicate
 */
public static Object some (Predicate p, Cons lst) {
	//special case: (no item satisfies the predicate)
	if(lst == null)
		return null;
	//base case:
	if(p.pred(first(lst)))
		return first(lst);
	//recursive case:
	else
		return some(p, rest(lst));
}
 /**
  * Question No.12
  * @param p a predicate
  * @param lst a list
  * @return a boolean that tests whether every item in the list satisfies the predicate
  */
public static boolean every (Predicate p, Cons lst) {
	//base case:
	if(lst == null)
		return true;
	//recursive case (or just return true when an item satisfies p):
	else
		return p.pred(first(lst)) ? every(p, rest(lst)) : false;
		
}

// Make a list of Binomial coefficients
// binomial(2) = (1 2 1)
/**
* The function in Assignment#2 that is now used in main to test the function interpolate(Cons lst, double x)
* @param n an Integer
* @return list of binomial coefficients
*/

public static Cons binomial(int n) {
	Cons lst = cons(null, null);
	return binomialb(n, 0, lst, null, lst);
}

//auxiliary function
private static Cons binomialb(int n, int currentLevel, Cons current, Cons pre, Cons currentHead){
	//base case (reach the end of required level and return)
	if(currentLevel == n && length(currentHead) == n){
		setfirst(current, 1);
		return currentHead;
	}
	//recursive case 1: (reach the end of current level, save the list and start a new level)
	else if(length(currentHead) == currentLevel){
		setfirst(current, 1);
		Cons newCons = cons(1, cons(null, null));
		return binomialb(n, ++currentLevel, rest(newCons), currentHead, newCons);
	}
	//recursive case 2: (add new element to the list in the current level) 
	else{
		setfirst(current, (Integer)first(pre) + (Integer)second(pre));
		setrest(current, cons(null, null));
		return binomialb(n, currentLevel, rest(current), rest(pre), currentHead);
	}
}

    // ****** your code ends here ******

    public static void main( String[] args )
      { 
        Cons mylist =
            list(new Integer(95), new Integer(72), new Integer(86),
                 new Integer(70), new Integer(97), new Integer(72),
                 new Integer(52), new Integer(88), new Integer(77),
                 new Integer(94), new Integer(91), new Integer(79),
                 new Integer(61), new Integer(77), new Integer(99),
                 new Integer(70), new Integer(91) );
        System.out.println("mylist = " + mylist.toString());
        System.out.println("sum = " + sum(mylist));
        System.out.println("mean = " + mean(mylist));
        System.out.println("meansq = " + meansq(mylist));
        System.out.println("variance = " + variance(mylist));
        System.out.println("stddev = " + stddev(mylist));
        System.out.println("sine(0.5) = " + sine(0.5));  // 0.47942553860420301
        System.out.print("nthcdr 5 = ");
        System.out.println(nthcdr(5, mylist));
        System.out.print("nthcdr 18 = ");
        System.out.println(nthcdr(18, mylist));
        System.out.println("elt 5 = " + elt(mylist,5));

        Cons mylistb = list(new Integer(0), new Integer(30), new Integer(56),
                           new Integer(78), new Integer(96));
        System.out.println("mylistb = " + mylistb.toString());
        System.out.println("interpolate(3.4) = " + interpolate(mylistb, 3.4));
        Cons binom = binomial(12);
        System.out.println("binom = " + binom.toString());
        System.out.println("interpolate(3.4) = " + interpolate(binom, 3.4));

        Cons mylistc = list(new Integer(1),
                            list(new Integer(2), new Integer(3)),
                            list(list(new Integer(4), new Integer(5)),
                                 new Integer(6)));
        System.out.println("mylistc = " + mylistc.toString());
        System.out.println("sumtr = " + sumtr(mylistc));

        Cons mylistd = list(new Integer(0), new Integer(1), new Integer(2),
                            new Integer(3), new Integer(4), new Integer(5),
                            new Integer(6) );
        System.out.println("mylistd = " + mylistd.toString());
        System.out.println("subseq(2 5) = " + subseq(mylistd, 2, 5));

        Cons myliste = list(new Integer(3), new Integer(17), new Integer(-2),
                            new Integer(0), new Integer(-3), new Integer(4),
                            new Integer(-5), new Integer(12) );
        System.out.println("myliste = " + myliste.toString());
        System.out.println("posfilter = " + posfilter(myliste));

        final Predicate myp = new Predicate()
            { public boolean pred (Object x)
                { return ( (Integer) x > 3); }};

        System.out.println("subset = " + subset(myp, myliste).toString());

        final Functor myf = new Functor()
            { public Integer fn (Object x)
                { return new Integer( (Integer) x + 2); }};

        System.out.println("mapcar = " + mapcar(myf, mylistd).toString());

        System.out.println("some = " + some(myp, myliste).toString());

        System.out.println("every = " + every(myp, myliste));

      }

}