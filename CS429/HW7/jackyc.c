//Last Name: Chou First Name: Ka Seng EID: kc32547 CS429 HW7

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define NEWLINE '\n'
#define CARRIAGERETURN '\r'
#define MAXSTRINGSIZE 150
#define TRUE 1
#define FALSE 0

typedef short Bool;
typedef char *String;

//define point structure
struct Point
{
    double x;
    double y;
};
typedef struct Point Point;

//define line segments structure
struct LineSegments
{
    Point p1;
    Point p2;
    struct LineSegments *next;
};
typedef struct LineSegments LineSegments;

//define polygon structure
struct Polygons
{
    //the minimum and maximum values of the X ans Y coordinates that bounds the polygon
    double minX;
    double maxX;
    double minY;
    double maxY;

    int edgesSize; //the number of edges of the polygon
    LineSegments *edges; //a list of edges of the polygon
    struct Polygons *next;
};
typedef struct Polygons Polygons;

//define regions structure
struct Regions
{
    String name;
    Polygons *bounds;
    struct Regions *next;
};
typedef struct Regions Regions;

static int lineNo = 1; //keep track of line number

//functions declaraction
void issue_input_error();
void update_lineNo(int);
int skip_blanks(FILE*);

Regions *readRegions(FILE*);
Regions *readRegion(FILE*);
String get_region_name(FILE*);
Polygons *get_region_bound(FILE*);
LineSegments *get_region_edge(FILE*, Point*);
char get_region_point(FILE*, Point*);
double read_point_coordinate(FILE*);
Bool pointEqual(Point, Point);

void free_regions(Regions*);
void free_bound(Polygons*);
void free_edges(LineSegments*);

void process_regions(Regions*);
Bool is_adjacent_region(Polygons*, Polygons*);
Bool is_adjacents_bound(LineSegments*, LineSegments*);
Bool is_interect_line(LineSegments*, LineSegments*);
short get_direction(Point, Point, Point);
Bool on_segment(Point, Point, Point);
Bool is_parallel_line(LineSegments*, LineSegments*);

/***************************************************************************
*                                                                          *
*                                   main                                   *
*                                                                          *
***************************************************************************/

int main(int argc, String *argv)
{
    FILE* input;
    if(argc != 2 || ((input = fopen(argv[1], "r")) == NULL))
    {
        printf("Invalid usage! (usage: jackyc InputRegionsFile)\n");
        exit(1);
    }

    //read the input file to creat the regions
    Regions *regions = readRegions(input);

    //process the data the produce the output
    process_regions(regions);

    free_regions(regions); //free spaces
    fclose(input);
    return 0;
}

/***************************************************************************
*                                                                          *
*            functions for general processing of input file                *
*                                                                          *
***************************************************************************/

void issue_input_error()
{
    printf("Bad input at line %d\n", lineNo);
    exit(1);
}

void update_lineNo(int c)
{
    if(c == NEWLINE || c == CARRIAGERETURN)
        lineNo++;
}

//skips any space characters and update line number
int skip_blanks(FILE *file)
{
    int c;
    while (((c = getc(file)) != EOF) && isspace(c)) /* keep reading */
        update_lineNo(c);
    return(c);
}

/***************************************************************************
*                                                                          *
*       functions for reading in input file and creating structures        *
*                                                                          *
***************************************************************************/
//read in files and return the "head" of the list of regions created
Regions *readRegions(FILE *file)
{
    Regions *regions; //this is the head of the list

    //create the head of the regions
    if((regions = readRegion(file)) == NULL)
       {
           //empty file
           exit(0);
       }

    int c;
    if((c = skip_blanks(file)) != ',') //skip possible comma between regions
            ungetc(c, file);
    Regions *current = regions; //for "travelling" through the list of regions
    while((current->next = readRegion(file)) != NULL)
    {
        current = current->next;
        if((c = skip_blanks(file)) != ',') //skip possible comma between regions
            ungetc(c, file);
    }

    return regions;
}

//read in files and return the new region created
Regions *readRegion(FILE *file)
{

    int c;
    if((c = skip_blanks(file)) == EOF) //end of file
        return NULL;

    Regions *region = (Regions*)calloc(1, sizeof(Regions)); //allocate space for new region

    if(c != '{')
        issue_input_error();

    region->name = get_region_name(file); //get the name of the region

    if((c = skip_blanks(file)) != ',') //issue error if there is no comma between name and bounds
        {printf("%s  %c\n",region->name, c);issue_input_error();}

    //create the head of the bounds
    if((region->bounds = get_region_bound(file)) == NULL)
    {
        //the region has no bounds, issue error
        issue_input_error();
    }

    if((c = skip_blanks(file)) != ',') //ignore possible comma between bounds
        ungetc(c, file);

    Polygons *current = region->bounds; //for "travelling" through the list of bounds
    while((current->next = get_region_bound(file)) != NULL)
    {
        current = current->next;
        if((c = skip_blanks(file)) != ',') //ignore possible comma between bounds
            ungetc(c, file);
    }

    if(skip_blanks(file) != '}')
        issue_input_error();

    return region;
}

//read and return the name of a region
String get_region_name(FILE *file)
{
    String s = (String)calloc(MAXSTRINGSIZE, sizeof(char)); //allocate space for new name

    int c;
    int i = 0;

    //construct the name of the region
    if(!isalnum((c = skip_blanks(file))))
        issue_input_error();
    s[i++] = c;
    while((c = getc(file)) != ',')
            s[i++] = c;
    ungetc(c, file); //put back the last character read (should be a ',')
    s[i] = '\0'; //seal the string with a null character

    return s;

}

//read and return the bound of a region
Polygons *get_region_bound(FILE *file)
{
    int c;
    if((c = skip_blanks(file)) == '}')
    {
        ungetc(c, file);
        return NULL;
    }

    ungetc(c, file);

    Polygons *p = calloc(1, sizeof(Polygons)); //allocate space for new bound

    //initialize the bounding values
    p->minX = DBL_MAX;
    p->minY = DBL_MAX;
    p->maxX = DBL_MIN;
    p->maxY = DBL_MIN;

    if(skip_blanks(file) != '[')
        issue_input_error();

    Point *tempPoint = (Point*)calloc(1, sizeof(Point)); //for holding that point that has already been processed, use to construct the next edges

    if((get_region_point(file, tempPoint)) == 0)
    {
        //there is no point for the bound, issue error
        issue_input_error();
    }

    //create the first edge (head of a bound)
    if((p->edges = get_region_edge(file, tempPoint)) == NULL)
    {
        //no edges (only one point) for the bound, issue error
        issue_input_error();
    }
    p->edgesSize++; //update number of edges

    //update the bound
    if((p->edges->p1).x >= (p->maxX))
    {
        p->maxX = (p->edges->p1).x;
    }
    if((p->edges->p2).x >= (p->maxX))
    {
        p->maxX = (p->edges->p2).x;
    }
    if((p->edges->p1).x <= (p->minX))
    {
        p->minX = (p->edges->p1).x;
    }
    if((p->edges->p2).x <= (p->minX))
    {
        p->minX = (p->edges->p2).x;
    }
    if((p->edges->p1).y >= (p->maxY))
    {
        p->maxY = (p->edges->p1).y;
    }
    if((p->edges->p2).y >= (p->maxY))
    {
        p->maxY = (p->edges->p2).y;
    }
    if((p->edges->p1).y <= (p->minY))
    {
        p->minY = (p->edges->p1).y;
    }
    if((p->edges->p2).y <= (p->minY))
    {
        p->minY = (p->edges->p2).y;
    }

    LineSegments *current = p->edges; //for "travelling" through the list of bounds
    while((current->next = get_region_edge(file, tempPoint)) != NULL)
    {
        p->edgesSize++; //update number of edges
        current = current->next;
        //update the bounds
        if((current->p2).x >= (p->maxX))
        {
            p->maxX = (current->p2).x;
        }
        if((current->p2).x <= (p->minX))
        {
            p->minX = (current->p2).x;
        }
        if((current->p2).y >= (p->maxY))
        {
            p->maxY = (current->p2).y;
        }
        if((current->p2).y <= (p->minY))
        {
            p->minY = (current->p2).y;
        }
    }

    //check if the polygon just read is a valid polygon (issue error if not)
    if(p->edgesSize < 2 || !pointEqual(p->edges->p1, *tempPoint))
        issue_input_error();

    if(skip_blanks(file) != ']')
        issue_input_error();

    free(tempPoint);

    return p;
}

//read and return the edges of the bounds of a region, the first argument is the previous point that has already been read from input file
LineSegments *get_region_edge(FILE *file, Point *firstPt)
{
    LineSegments *ls = (LineSegments*)calloc(1, sizeof(LineSegments)); //allocate space for edge

    int c;
    if((c = skip_blanks(file)) == ']') //all points have been read
    {
        ungetc(c, file);
        return NULL;
    }

    ungetc(c, file);

    if(skip_blanks(file) != ',') //issue error if there is no comma between points
        issue_input_error();

    //assign first point of the edge, which is the second point of the previous edge
    (ls->p1).x = firstPt->x;
    (ls->p1).y = firstPt->y;

    Point *tempPt;//a point pointer for holding the new point read (it is needed because it is used to check for null)
    if(get_region_point(file, firstPt) == 0)
    {
        //no more points to read, so no new edge can be formed
        return NULL;
    }

    //assign the second point of the edge
    (ls->p2).x = firstPt->x;
    (ls->p2).y = firstPt->y;

    return ls;
}

//read and return the point of a bound of a region
char get_region_point(FILE *file, Point *pt)
{
    int c;
    if((c = skip_blanks(file)) == ']') //all points have been read
    {
        ungetc(c, file);
        return 0;
    }

    ungetc(c, file);

    if((c = skip_blanks(file)) != '(')
            issue_input_error();

    //read the x-coordinate of the point
    pt->x = read_point_coordinate(file);

    if((c = skip_blanks(file)) != ',') //issue error if there is no comma between two coordinates in a point
          issue_input_error();

    //read the y-coordinate of the point
    pt->y = read_point_coordinate(file);

    if((c = skip_blanks(file)) != ')')
            issue_input_error();

    return 1;
}

//read and return the value of a coordinate (a double) in a point
double read_point_coordinate(FILE *file)
{
    int i = 0;
    int c;
    String value = (String)calloc(MAXSTRINGSIZE, sizeof(char)); //allocate space for the string for holding the characters that forms a double

    if((!isdigit((c = skip_blanks(file)))) && (c != '+') && (c != '-') && (c != '.')) //if the first character of the double is not valid, issue error
        issue_input_error();

    value[i++] = c;

    //read the remaining possible characters to form the double value
    if(isdigit(c)) //the first character is a digit
    {
        while(isdigit(c = skip_blanks(file)))
            value[i++] = c;
        if(c == '.') //a possible dot
        {
            value[i++] = c;
            while(isdigit(c = skip_blanks(file)))
                value[i++] = c;
            ungetc(c, file); //put back the character to the file
        }
        else
            ungetc(c, file); //put back the character to the file

    }
    else if(c == '.') //the first character is a dot
    {
        if(!isdigit(c = skip_blanks(file))) //just a dot, no digit follows, issue error
            issue_input_error();
        value[i++] = c;
        while(isdigit(c = skip_blanks(file)))
            value[i++] = c;
        ungetc(c, file); //put back the character to the file
    }
    else //the first character is '+' or '-'
    {
        if((c = skip_blanks(file)) == '.') //follows by a dot
        {
            value[i++] = c;
            if(!isdigit(c = skip_blanks(file))) //just a dot, no digit follows, issue error
                issue_input_error();
            value[i++] = c;
            while(isdigit(c = skip_blanks(file)))
                value[i++] = c;
            ungetc(c, file); //put back the character to the file
        }
        else if(isdigit(c)) //follows by a digit
        {
            value[i++] = c;
            while(isdigit(c = skip_blanks(file)))
            value[i++] = c;
            if(c == '.') //a possible dot
            {
                value[i++] = c;
                while(isdigit(c = skip_blanks(file)))
                    value[i++] = c;
                ungetc(c, file); //put back the character to the file
            }
            else
                ungetc(c, file); //put back the character to the file
        }
        else //just a '+' or '-', issues error
            issue_input_error();
    }

    value[i] = '\0'; //seal the string with a null character
    double result = atof(value);
    //printf("here1\n");
    free(value); //free space for the string
    //printf("here2\n");
    return result;

}

//check if two points are equal
Bool pointEqual(Point p1, Point p2)
{
    return ((p1.x == p2.x) && (p1.y == p2.y));
}
/***************************************************************************
*                                                                          *
*                       functions for freeing spaces                       *
*                                                                          *
***************************************************************************/
//free space allocated to regions
void free_regions(Regions *rs)
{
    Regions *old;
    while(rs != NULL)
    {
        old = rs;
        rs = rs->next;
        free(old->name);
        free_bound(old->bounds);
        free(old);
    }
}

//free space allocated to the bounds of a region (polygons)
void free_bound(Polygons *ps)
{
    Polygons *old;
    while(ps != NULL)
    {
        old = ps;
        ps = ps->next;
        free_edges(old->edges);
        free(old);
    }
}

//free space allocated to the edges if a bound (edges of a polygon)
void free_edges(LineSegments *ls)
{
    LineSegments *old;
    while(ls != NULL)
    {
        old = ls;
        ls = ls->next;
        free(old);
    }
}
/***************************************************************************
*                                                                          *
*           functions for processing data (creating output)                *
*                                                                          *
***************************************************************************/
//list out all regions names with the name of the regions that are adjacent to them
void process_regions(Regions *regions)
{
    int firstTime;
    Regions *current = regions; //to "travel" through the list of regions to produce output
    Regions *current2; //to "travel" through the list of regions to compare with each region in the inner loop
    while(current != NULL)
    {
        firstTime = 1;
        printf("%s:", current->name);
        current2 = regions;
        while(current2 != NULL)
        {
            if(strcmp(current->name, current2->name)) //if the regions are not the same, compare them
            {
                if(is_adjacent_region(current->bounds, current2->bounds))
                {
                    if(firstTime)
                    {
                        firstTime = 0;
                        printf(" %s", current2->name);
                    }
                    else
                        printf(", %s", current2->name);
                }
            }
            current2 = current2->next;
        }
        printf("\n");
        current = current->next;
    }
}

//check if a region is adjacent to another
Bool is_adjacent_region(Polygons *bounds1, Polygons *bounds2)
{
    Polygons *current = bounds1; //to "travel" through the list of bounds 1
    Polygons *current2;//to "travel" through the list of bounds 2
    while(current != NULL)
    {
        current2 = bounds2;
        while(current2 != NULL)
        {
            if((current->minX <= current2->maxX) && (current->maxX >= current2->minX) && (current->minY <= current2->maxY) && (current->maxY >= current2->minY))
            {
                if(is_adjacents_bound(current->edges, current2->edges)) //the regions are adjacent
                {
                    return TRUE;
                }
            }
            current2 = current2->next;
        }
        current = current->next;
    }

    return FALSE; //the regions are not adjacent
}

//check if a polygon is adjacent to another
Bool is_adjacents_bound(LineSegments *edges1, LineSegments *edges2)
{
    int intersect_counter = 0; //counter the number of intersections of two polygons
    Bool intersect = 0; //Bool to indicate if the current lines intersect
    Bool parallel = 0; //Bool to indicate if the current lines parallel

    LineSegments *current = edges1; //to "travel" through the list of edges1
    LineSegments *current2; //to "travel" through the list of edges2

    while(current != NULL)
    {
        current2 = edges2;
        while(current2 != NULL)
        {
            intersect = is_interect_line(current, current2);
            parallel = is_parallel_line(current, current2);

            //ignore corner case
            if(!(pointEqual(current->p1, current2->p1) || pointEqual(current->p1, current2->p2) || pointEqual(current->p2, current2->p1) || pointEqual(current->p2, current2->p2)) || parallel)
            {
                if(intersect)
                    intersect_counter++; //update number of intersections

                //the polygons are adjacent if they have 2 single point intersect or if they are parallel and intersect (infinite number of intersection coz part of the line lays on other)
                if((intersect_counter == 2) || (intersect && parallel))
                    return TRUE;
            }

            current2 = current2->next;
        }
        current = current->next;
    }

    return FALSE; //the two polygons are not adjacent
}

//check if two lines intersect
Bool is_interect_line(LineSegments *l1, LineSegments *l2)
{
    short direction1 = get_direction(l2->p1, l2->p2, l1->p1);
    short direction2 = get_direction(l2->p1, l2->p2, l1->p2);
    short direction3 = get_direction(l1->p1, l1->p2, l2->p1);
    short direction4 = get_direction(l1->p1, l1->p2, l2->p2);

    return (((direction1 > 0 && direction2 < 0) || (direction1 < 0 && direction2 > 0)) && ((direction3 > 0 && direction4 < 0) || (direction3 < 0 && direction4 > 0))) ||
            (direction1 == 0 && on_segment(l2->p1, l2->p2, l1->p1)) || (direction2 == 0 && on_segment(l2->p1, l2->p2, l1->p2)) ||
            (direction3 == 0 && on_segment(l1->p1, l1->p2, l2->p1)) || (direction4 == 0 && on_segment(l1->p1, l1->p2, l2->p2));
}

short get_direction(Point p1, Point p2, Point p3)
{
  double d1 = (p3.x - p1.x) * (p2.y - p1.y);
  double d2 = (p2.x - p1.x) * (p3.y - p1.y);
  return d1 < d2 ? -1 : d1 > d2 ? 1 : 0;
}

Bool on_segment(Point p1, Point p2, Point p3)
{
  return (p1.x <= p3.x || p2.x <= p3.x) && (p3.x <= p1.x || p3.x <= p2.x) &&
         (p1.y <= p3.y || p2.y <= p3.y) && (p3.y <= p1.y || p3.y <= p2.y);
}

//check if two lines parallel
Bool is_parallel_line(LineSegments *l1, LineSegments *l2)
{
    if ((((l1->p2).y - (l1->p1).y) / ((l1->p2).x - (l1->p1).x)) == (((l2->p2).y - (l2->p1).y) / ((l2->p2).x - (l2->p1).x)))
        return TRUE;
    else
        return FALSE;
}

