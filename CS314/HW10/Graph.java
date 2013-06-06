// Graph.java    GSN   ; 09 Aug 10
// 27 Aug 08; 04 Dec 08; 25 Mar 09; 27 Mar 09; 30 Mar 09; 17 Apr 09

import java.util.*;

interface Heuristic { Integer fn(Graph.Vertex x, Graph.Vertex y); }

public class Graph {
    private TreeMap<String,Vertex> vertices;
    public void add (Vertex v) {
        if ( vertices == null )
            vertices = new TreeMap<String,Vertex>();
        vertices.put(v.contents, v); }
    public Vertex vert(String s) {
        Vertex v = new Vertex(s);
        add(v);
        return v; }
    public Vertex vert(String s, Double lat, Double longx) {
        Vertex v = new Vertex(s, lat, longx);
        add(v);
        return v; }
    public TreeMap<String,Vertex> vertices() { return vertices; }
    public Vertex vertex(String s) { return vertices.get(s); }
    public void print() {
        Collection c = vertices.values();
        Iterator itr = c.iterator();
        while(itr.hasNext()) {
            Vertex v = (Vertex) itr.next();
            v.print(); } }
    public Graph (Cons lst) {
        //        System.out.println("Entering Graph()");
        //   for (Cons ptr = lst; ptr != null; ptr = Cons.rest(ptr) ) {
        //       if ( Cons.first(ptr) != null )
               //    System.out.println(Cons.first(ptr).toString()); }
        vertices = new TreeMap<String,Vertex>();
        // put all vertices in first
        for (Cons ptr = lst; ptr != null; ptr = Cons.rest(ptr) ) {
            Cons item = (Cons) Cons.first(ptr);
            //         System.out.println("item = " + item.toString());
            Vertex node = vert((String) Cons.first(item),
                               (Double) Cons.second(item),
                               (Double) Cons.third(item) ); }
        for (Cons ptr = lst; ptr != null; ptr = Cons.rest(ptr) ) {
            Cons item = (Cons) Cons.first(ptr);
            //         System.out.println("item = " + item.toString());
            Cons conns = (Cons) Cons.fourth(item);
            Vertex node = vertices.get((String) Cons.first(item));
            for ( ; conns != null; conns = Cons.rest(conns) ) {
                Cons conn = (Cons) Cons.first(conns);
                //      System.out.println("conn = " + conn.toString());
                Vertex target = vertices.get((String) Cons.first(conn));
                node.add(target, (Integer) Cons.second(conn));
                target.add(node, (Integer) Cons.second(conn)); } } }

public class Vertex {
    private String contents;
    private ArrayList<Edge> edges;
    private Integer cost;
    private Integer estimate;
    private Vertex parent;
    private boolean visited;
    private Double latitude;
    private Double longitude;
    public Vertex (String s)
      { contents = s;
        edges = new ArrayList<Edge>(); }
    public Vertex (String s, Double lat, Double longx) {
        contents = s;
        latitude = lat;
        longitude = longx;
        edges = new ArrayList<Edge>(); }
    public void add (Edge e) { edges.add(e); }
    public void add (Vertex v, int c) {
        edges.add( new Edge(v, c) ); }
    public void add (Vertex v, int c, boolean undirected) {
        edges.add( new Edge(v, c) );
        if ( undirected && ( this.contents.compareTo(v.contents) < 0 ) )
            v.edges.add( new Edge(this, c) ); }
    public String str() { return contents; }
    public int cost() { return cost; }
    public Vertex parent() { return parent; }
    public Double latitude() { return latitude; }
    public Double longitude() { return longitude; }
    public int compareTo (Vertex other) {
        if ( this.cost == other.cost )
            return 0;
        else if ( this.cost < other.cost )
            return -1;
        else return 1; }
    public void print() {
        System.out.print("node " + contents);
        if ( latitude != 0.0 ) {
            System.out.print("  lat = " + latitude);
            System.out.print("  long = " + longitude); }
        System.out.print("  cost = " + cost);
        System.out.print("  parent = " +
                               ((parent == null) ? "null"
                                : parent.contents) + "   ");
        for ( Edge e : edges ) e.print();
        System.out.println(); }
    public Integer distanceTo(Vertex other) {     // in miles
        return ((Double) (gcdist(this.latitude, this.longitude,
                                 other.latitude, other.longitude)
                          * 0.62137119)).intValue(); }
}

public class Edge {
    private Vertex target;
    private int cost;
    public Edge ( Vertex v, int c ) {
        target = v;
        cost = c; }
    public void print() {
        System.out.print("(" + target.str() + ", " + cost + ") "); }
}

// Approx Great-circle distance.  Args in degrees, result in kilometers
public double gcdist (double lata, double longa, double latb, double longb)
  {
    double midlat, psi, dist;
    midlat = 0.5 * (lata + latb);
    psi = 0.0174532925
          * Math.sqrt(Math.pow(lata - latb, 2)
                  + Math.pow((longa - longb)
                             * Math.cos(0.0174532925 * midlat), 2));
    dist = 6372.640112 * psi;
    return dist;
  }   /* gcdist */

    // ****** your code starts here ******

/**
 * Question No.1
 * @param s a starting Vertex in Dijkstra algorithms
 * @return the number of nodes removed from the priority queue
 */
public int dijkstra( Vertex s ) {
	int counter = 0;
	for ( Vertex v : vertices.values()) {
	    v.visited = false;
	    v.cost = 999999; }
	  s.cost = 0;
	  s.parent = null;
	  PriorityQueue<Vertex>
	      fringe = new PriorityQueue<Vertex>(20,
	        new Comparator<Vertex>() {
	          public int compare(Vertex i, Vertex j) {
	              return (i.cost - j.cost); }});
	  fringe.add(s);
	  while ( ! fringe.isEmpty() ) {
	    Vertex v = fringe.remove();  // lowest-cost
	    counter++;
	    if ( ! v.visited )
	      { v.visited = true;
	        for ( Edge e : v.edges )
	          { int newcost = v.cost + e.cost;
	            if ( newcost < e.target.cost )
	             { e.target.cost = newcost;
	               e.target.parent = v;
	               fringe.add(e.target);  
	               } 
	            } 
	        } 
	    } 
	  return counter;
 }

/**
 * Question No.2
 * @param s a starting Vertex in Prim algorithms
 * @return the total cost of the MST as the result of prim
 */
public int prim( Vertex s ) {
	int counter = 0;
	for ( Vertex v : vertices.values() ) {
	    v.visited = false;
	    v.parent = null;
	    v.cost = 999999; }
	  s.cost = 0;
	  PriorityQueue<Vertex>
	      fringe = new PriorityQueue<Vertex>(20,
	        new Comparator<Vertex>() {
	          public int compare(Vertex i, Vertex j) {
	              return (i.cost - j.cost); }});
	  fringe.add(s);
	  while ( ! fringe.isEmpty() ) {
	    Vertex v = fringe.remove();  // lowest-cost
	    if ( ! v.visited )
	      { v.visited = true;
	      counter += v.cost();
	        for ( Edge e : v.edges )
	          {  if ( (! e.target.visited) &&
	                  ( e.cost < e.target.cost ) )
	             { e.target.cost = e.cost;
	               e.target.parent = v;
	               fringe.add(e.target); 
	               } 
	          } 
	        } 
	    } 
	  return counter;
 }

/**
 * Question  No.3
 * @param start the starting Vertex
 * @param goal the goal Vertex
 * @param h 
 * @return
 */
public int astar( Vertex start, Vertex goal, Heuristic h ) {
	int counter = 0;
	for ( Vertex v : vertices.values()) {
	    v.visited = false;
	    v.cost = 999999; 
	    v.estimate = 999999;}
	  start.cost = 0;
	  start.estimate = h.fn(start, goal);
	  start.parent = null;
	  PriorityQueue<Vertex>
	      fringe = new PriorityQueue<Vertex>(20,
	        new Comparator<Vertex>() {
	          public int compare(Vertex i, Vertex j) {
	              return (i.estimate - j.estimate); }});
	  fringe.add(start);
	  while ( ! fringe.isEmpty() ) {
	    Vertex v = fringe.remove();  // lowest-cost
	    counter++;
	    if(v.contents.equals(goal.contents))
	    	return counter;
	    if ( ! v.visited )
	      { v.visited = true;
	        for ( Edge e : v.edges )
	          { int newcost = v.cost + e.cost;
	            if ( newcost < e.target.cost )
	             { e.target.cost = newcost;
	               e.target.estimate = newcost + h.fn(e.target, goal);
	               e.target.parent = v;
	               fringe.add(e.target);  
	               } 
	            } 
	        } 
	    } 
	  return counter;
 }

public static final Heuristic dist = new Heuristic() {
        public Integer fn(Vertex from, Vertex to) {
            return from.distanceTo(to); } };

public static final Heuristic halfass = new Heuristic() {
	public Integer fn(Vertex from, Vertex to) {
        return from.distanceTo(to) / 2; }
};

public static final Heuristic zip = new Heuristic() {
	public Integer fn(Vertex from, Vertex to) {
        return 0; }
};

    static Random rng = new Random();

public static final Heuristic randombs = new Heuristic() {
	public Integer fn(Vertex from, Vertex to) {
        return (Integer)(int)((rng.nextDouble() * 1.0) * from.distanceTo(to)); }
};

public static final Heuristic randomlies = new Heuristic() {
        public Integer fn(Vertex from, Vertex to) {
            return (Integer)((int) (rng.nextDouble() * 5000.0)); } };

/**
 * Question No.2
 * @param start the Vertex to begin
 * @param goal the Vertex to end
 * @return the cost from start Vertex to goal Vertex
 */
public int edgecost( Vertex start, Vertex goal) {
	Edge target = null;
	for(Edge e: start.edges){
		if(e.target.contents.equals(goal.contents))
			target = e;		
	}
	return target.cost;
 }

/**
 * Question No.2
 * @param v the goal Vertex
 * @return the total cost of the edge from v to the root of the tree
 */
public int pathcost (Vertex v) {
	//base case:
	if(v.parent() == null)
		return 0;
	//recursive case:
	else
		return v.cost() + pathcost(v.parent());
 }

/**
 * Question No.2
 * @return the total cost of all edges in the graph
 */
public int totalcost( ) {
	int totalCost = 0;
	for(Vertex v: vertices.values())
		for(Edge e: v.edges)
			totalCost += e.cost;
	return totalCost/2;
	
 }

    // list of city names to get to specified city
/**
 * Question No.2
 * @param city the name of a city
 * @return a path represeted by a list of city names from the starting city to the specified city
 */
public Cons pathto(String city) {
	Vertex cityV = vertices.get(city);
	return pathtob(cityV, null);
 }

//private helper method of pathto
private Cons pathtob(Vertex city, Cons path) {
	//base case:
	if(city.parent() == null)
		return Cons.cons(city.contents, path);
	//recursive case:
	else
		return pathtob(city.parent(), Cons.cons(city.contents, path));
}


    // ****** your code ends here ******

}