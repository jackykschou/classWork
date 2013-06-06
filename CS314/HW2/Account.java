// Account.java

    public class Account implements Comparable<Account> {
        private String name;
        private Integer amount;
        public Account(String nm, Integer amt) {
            name = nm;
            amount = amt; }
        public static Account account(String nm, Integer amt) {
            return new Account(nm, amt); }
        public String name() { return name; }
        public Integer amount() { return amount; }
        public boolean equals(Object x) {
            if ( x == null ) return false;
            else if ( getClass() != x.getClass() ) return false;
            else return name.equals( ((Account)x).name); }

        // return -1 to sort this account before x, else 1
        public int compareTo(Account x) {

            // ***** your code here *****
        	if(this.name().compareTo(x.name()) != 0)
        		return (this.name().compareTo(x.name()) < 0)? -1: 1;
        	else if(this.amount() >= 0 || x.amount() >= 0)
        		return (this.amount() > x.amount())? -1: 1;
        	else
        		return (this.amount() < x.amount())? -1: 1;
        	
        }

        public String toString() {
            return ( "(" + this.name + " " + this.amount + ")"); }
    }