/**
 * this class Cons implements a Lisp-like Cons cell
 * 
 * @author  Gordon S. Novak Jr.
 * @version 29 Nov 01; 25 Aug 08; 05 Sep 08; 08 Sep 08; 12 Sep 08; 24 Sep 08
 *          02 Oct 09; 12 Feb 10
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

    // iterative destructive merge using compareTo
public static Cons dmerj (Cons x, Cons y) {
  if ( x == null ) return y;
   else if ( y == null ) return x;
   else { Cons front = x;
          if ( ((Comparable) first(x)).compareTo(first(y)) < 0)
             x = rest(x);
            else { front = y;
                   y = rest(y); };
          Cons end = front;
          while ( x != null )
            { if ( y == null ||
                   ((Comparable) first(x)).compareTo(first(y)) < 0)
                 { setrest(end, x);
                   x = rest(x); }
               else { setrest(end, y);
                      y = rest(y); };
              end = rest(end); }
          setrest(end, y);
          return front; } }

public static Cons midpoint (Cons lst) {
  Cons current = lst;
  Cons prev = current;
  while ( lst != null && rest(lst) != null) {
    lst = rest(rest(lst));
    prev = current;
    current = rest(current); };
  return prev; }

    // Destructive merge sort of a linked list, Ascending order.
    // Assumes that each list element implements the Comparable interface.
    // This function will rearrange the order (but not location)
    // of list elements.  Therefore, you must save the result of
    // this function as the pointer to the new head of the list, e.g.
    //    mylist = llmergesort(mylist);
public static Cons llmergesort (Cons lst) {
  if ( lst == null || rest(lst) == null)
     return lst;
   else { Cons mid = midpoint(lst);
          Cons half = rest(mid);
          setrest(mid, null);
          return dmerj( llmergesort(lst),
                        llmergesort(half)); } }


    // ****** your code starts here ******
    // add other functions as you wish.

/**
 * Question no. 1
 * @param x a list represents a set
 * @param y a list represents a set
 * @return a list (set) that is the union of the 2 sets
 */
public static Cons union (Cons x, Cons y) {
	Cons sortedx = llmergesort(x);
	Cons sortedy = llmergesort(y);
	return mergeunion(sortedx, sortedy);
}

    // following is a helper function for union
public static Cons mergeunion (Cons x, Cons y) {
	if(x == null)
		return y;
	if(y == null)
		return x;
	if(first(x).equals(first(y)))
		return cons(first(x), mergeunion(rest(x), rest(y)));
	else if(((Comparable)first(x)).compareTo(first(y)) < 0)
		return cons(first(x), mergeunion(rest(x), y));
	else
		return cons(first(y), mergeunion(x, rest(y)));
}

/**
 * Question No. 1
 * @param x a list represents a set
 * @param y a list represents a set
 * @return a list (set) that is the setDifference of the 2 sets
 */
public static Cons setDifference (Cons x, Cons y) {
	Cons sortedx = llmergesort(x);
	Cons sortedy = llmergesort(y);
	return mergediff(sortedx, sortedy);
}

    // following is a helper function for setDifference
private static Cons mergediff (Cons x, Cons y) {
	if(y == null || x == null)
		return x;
	if(((Comparable)first(x)).compareTo(first(y)) < 0)
		return cons(first(x), mergediff(rest(x), y));
	else if(first(x).equals(first(y))){
		return mergediff(rest(x), rest(y));
	}
	else
		return mergediff(rest(x), mergediff(x, rest(y)));
}

/**
 * Question No. 2
 * @param accounts a list of bank account
 * @param updates a list of bank account used for update
 * @return a list of updated account
 */
public static Cons bank(Cons accounts, Cons updates) {
	return bankb(accounts, llmergesort(updates));
}

//helper function
private static Cons bankb(Cons accounts, Cons updates){
	if(updates == null)
		return accounts;
	//special case: new name of account encounter
	else if(accounts == null || ((Account)first(accounts)).name().compareTo(((Account)first(updates)).name()) > 0){
		Integer sum = ((Account)first(updates)).amount();
		while(rest(updates) != null && (((Account)first(updates)).name().equals(((Account)second(updates)).name()))){
			sum += ((Account)second(updates)).amount();
			updates = rest(updates);
		}
		//print message and possibly add new account
		if(sum > 0){
			System.out.println( "There is a new account. Name: " + ((Account)first(updates)).name() 
					+ " Balance: " + sum);
			return cons((Account.account(((Account)first(updates)).name(), sum)), bankb(accounts, rest(updates)));
		}
		else{
			System.out.println("The account with name: " + ((Account)first(updates)).name() 
					+ " Balance: " + ((Account)first(updates)).amount() + " does not exist");
			return bankb(accounts, rest(updates));
		}
	}
	//find a matched account for update
	else if(((Account)first(accounts)).name().compareTo(((Account)first(updates)).name()) != 0)
		return bankb(rest(accounts), updates);
	else{
		boolean overcharge = false;
		Integer sum = ((Account)first(accounts)).amount();
		while(updates != null && ((Account)first(accounts)).name().compareTo(((Account)first(updates)).name()) == 0){
				sum += ((Account)first(updates)).amount();
				if(sum < 0){
					overcharge = true;
					sum -= 30;
				}
				updates = rest(updates);
		}
		if(overcharge)
			System.out.println("Overdraft(s) (each overdraft fee is $30): Account Name: " 
					+ ((Account)first(accounts)).name() + " Balance: " + sum);
		return cons(Account.account(((Account)first(accounts)).name(), sum), bankb(rest(accounts), updates));
	}
}

/**
 * Question No. 3
 * @param x an array of String
 * @param y an array of String
 * @return an array of the sorted sum of the two arrays
 */
public static String [] mergearr(String [] x, String [] y) {
	String []sum = new String[x.length + y.length];
	for(int i = 0, j = 0, k = 0; i < sum.length; i++){
		if(j < x.length && x[j].compareTo(y[k]) < 0){
			sum[i] = x[j];
			j++;
		}
		else{
			sum[i] = y[k];
			k++;
		}
	}
	return sum;
}

/**
 * Question No. 4
 * @param text a list of text
 * @return boolean to check if the markup in the text is well-formed.
 */
public static boolean markup(Cons text) {
	return markupb(text, null, 0);
}

//helper function
private static boolean markupb(Cons text, Cons top, int position){
	//base case:
	if(text == null && top == null)
		return true;
	//error case: (opening tag(s) is not closed)
	else if(text == null && top != null){
		System.out.println("Error: missing closing tag for the tag " 
	+ (String)first(top) + ". position: " + position);
		return false;
	}
	//case for opening tag:
	 if(!((String)first(text)).isEmpty() && isTag((String)first(text)) && !isCloseTag((String)first(text)))
		return markupb(rest(text), cons((String)first(text), top), ++position);
	//case for closing tag:
	else if(!((String)first(text)).isEmpty() && isCloseTag((String)first(text))){
		if(top != null && ((String)first(text)).substring(2).equals(((String)first(top)).substring(1)))
				return markupb(rest(text), rest(top), ++position);
		//error case: (closing tag not match opening tag)
		else{
			System.out.println("Error: closing tag " + (String)first(text) 
					+ (top != null?( " not matching the opening tag " + (String)first(top)): (" is an excessive tag"))
					+ ". position: " + position);
			return false;
		}	
	}
	//normal case: (not a tag)
	else
		return markupb(rest(text), top, ++position);
}

//helper function
private static boolean isTag(String s){
	return (s.charAt(0) == '<')? true: false;
}

//helper function
private static boolean isCloseTag(String s){
	return (isTag(s) && s.charAt(1) == '/')? true: false;
}
    // ****** your code ends here ******

    public static void main( String[] args )
      { 
        Cons set1 = list("d", "b", "c", "a");
        Cons set2 = list("f", "d", "b", "g", "h");
        System.out.println("set1 = " + set1.toString());
        System.out.println("set2 = " + set2.toString());
        System.out.println("union = " + union(set1, set2).toString());

        Cons set3 = list("d", "b", "c", "a");
        Cons set4 = list("f", "d", "b", "g", "h");
        System.out.println("set3 = " + set3.toString());
        System.out.println("set4 = " + set4.toString());
        System.out.println("difference = " +
                           setDifference(set3, set4).toString());

        Cons accounts = list(
               new Account("Arbiter", new Integer(498)),
               new Account("Flintstone", new Integer(102)),
               new Account("Foonly", new Integer(123)),
               new Account("Kenobi", new Integer(373)),
               new Account("Rubble", new Integer(514)),
               new Account("Tirebiter", new Integer(752)),
               new Account("Vader", new Integer(1024)) );

        Cons updates = list(
               new Account("Foonly", new Integer(100)),
               new Account("Flintstone", new Integer(-10)),
               new Account("Arbiter", new Integer(-600)),
               new Account("Garble", new Integer(-100)),
               new Account("Rabble", new Integer(100)),
               new Account("Flintstone", new Integer(-20)),
               new Account("Foonly", new Integer(10)),
               new Account("Tirebiter", new Integer(-200)),
               new Account("Flintstone", new Integer(10)),
               new Account("Flintstone", new Integer(-120))  );
        System.out.println("accounts = " + accounts.toString());
        System.out.println("updates = " + updates.toString());
        Cons newaccounts = bank(accounts, updates);
        System.out.println("result = " + newaccounts.toString());

        String[] arra = {"a", "big", "dog", "hippo"};
        String[] arrb = {"canary", "cat", "fox", "turtle"};
        String[] resarr = mergearr(arra, arrb);
        for ( int i = 0; i < resarr.length; i++ )
            System.out.println(resarr[i]);

        Cons xmla = list( "<TT>", "foo", "</TT>");
        Cons xmlb = list( "<TABLE>", "<TR>", "<TD>", "foo", "</TD>",
                          "<TD>", "bar", "</TD>", "</TR>",
                          "<TR>", "<TD>", "fum", "</TD>", "<TD>",
                          "baz", "</TD>", "</TR>", "</TABLE>" );
        Cons xmlc = list( "<TABLE>", "<TR>", "<TD>", "foo", "</TD>",
                          "<TD>", "bar", "</TD>", "</TR>",
                          "<TR>", "<TD>", "fum", "</TD>", "<TD>",
                          "baz", "</TD>", "</WHAT>", "</TABLE>" );
        Cons xmld = list( "<TABLE>", "<TR>", "<TD>", "foo", "</TD>",
                          "<TD>", "bar", "</TD>", "", "</TR>",
                          "</TABLE>", "</NOW>" );
        Cons xmle = list( "<THIS>", "<CANT>", "<BE>", "foo", "<RIGHT>" );
        Cons xmlf = list( "<CATALOG>",
                          "<CD>",
                          "<TITLE>", "Empire", "Burlesque", "</TITLE>",
                          "<ARTIST>", "Bob", "Dylan", "</ARTIST>",
                          "<COUNTRY>", "USA", "</COUNTRY>",
                          "<COMPANY>", "Columbia", "</COMPANY>",
                          "<PRICE>", "10.90", "</PRICE>",
                          "<YEAR>", "1985", "</YEAR>",
                          "</CD>",
                          "<CD>",
                          "<TITLE>", "Hide", "your", "heart", "</TITLE>",
                          "<ARTIST>", "Bonnie", "Tyler", "</ARTIST>",
                          "<COUNTRY>", "UK", "</COUNTRY>",
                          "<COMPANY>", "CBS", "Records", "</COMPANY>",
                          "<PRICE>", "9.90", "</PRICE>",
                          "<YEAR>", "1988", "</YEAR>",
                          "</CD>", "</CATALOG>");
        System.out.println("xmla = " + xmla.toString());
        System.out.println("result = " + markup(xmla));
        System.out.println("xmlb = " + xmlb.toString());
        System.out.println("result = " + markup(xmlb));
        System.out.println("xmlc = " + xmlc.toString());
        System.out.println("result = " + markup(xmlc));
        System.out.println("xmld = " + xmld.toString());
        System.out.println("result = " + markup(xmld));
        System.out.println("xmle = " + xmle.toString());
        System.out.println("result = " + markup(xmle));
        System.out.println("xmlf = " + xmlf.toString());
        System.out.println("result = " + markup(xmlf));

      }

}