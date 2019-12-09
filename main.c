//  main.c
//  Logic_Tableau
//
//  Created by Danny Toeun Kim on 2019/11/18.

#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>   /* malloc, free, rand */

const int Fsize=50;     /*maximum formula length*/
const int inputLines=10;/* number of formulas expected in input.txt*/
const int ThSize=100;   /* maximum size of set of formulas, if needed*/
const int TabSize=500;  /*maximum length of tableau queue, if needed*/
const char propositions[4] = "pqr";
const char binaryConnectives[4] = "v^>";

// For head and tape algorithm
// To access head variable globally in partone and parttwo
// These Global variables will change for each Tableau as for loop goes in the main function.
char *binaryPart1;
char *binaryPart2;
int globalHead = 0;
char topLevelBinaryConnective = '\0';

/*---------------------- Formula Validator -------------------------*/
// Input String may or may not be a valid formula.
// 1. Parse the input string. -> Classifies types
// 2. Get partone and parttwo for Binary formula


// check input has the same number of Lparen and Rparen -> return boolean
int checkParen(char *string){
    size_t head = 0;
    int Lparen = 0;
    int Rparen = 0;
    
    while (string[head] != '\0') {
        if (string[head]=='(') {
            Lparen++;
        }
        if (string[head]==')') {
            Rparen++;
        }
        head++;
    }
    
    return (Lparen == Rparen);
}

// Returns Boolean
int isInBC(char ch){
    return (ch=='v'|| ch=='>' || ch=='^');
}

// Returns Boolean
int isInProp(char ch){
    return (ch=='p'|| ch=='q' || ch=='r');
}

// return a sliced subString
char *sliced(char *source, int start, int end){
    char* subString = malloc((end-start+1) * sizeof(char));
    
    int i;
    for (i = 0; i < end-start; i++) {
        subString[i] = source[start+i];
    }
//    subString[end-start] = '\0';
    return subString;
}

/*
proposition    -> 1
negation       -> 2
binary         -> 3
not a formula  -> 0
*/
int parse(char *string){
    int length = (int)strlen(string);
    
    // check if the string is a proposition
    if (length == 1) {
        // check if string in propositions
        if (strstr(propositions, string)!= NULL) {
            return 1;
        }
        return 0;
    }
    
    // check if the string is a negation
    if (string[0]=='-') {
        if (length<2) {
            return 0;
        }
        if (length == 2 && isInProp(string[1]) ) {
            return 2;
        }
        // for the strings like '----p'
        if ( parse((string+1)) != 0 ) {
            return 2;
        }
        return 0;
    }
    
    // check if the string is a binary
    // check if they have same number of Lparen and Rparen
    // TODO: REMEMBER -(p^q^r) is not a negation. This is not a formula -> how are you going to check this?
            // Solution -> part1 and part2 should be also folmula
    if (string[0]=='(' && string[length-1]==')' && checkParen(string) && length>4) {
        // head and tape algorithm
        int head = 0;
        int Lparen = 0;
        int Rparen = 0;
        // look inside the outermost bracket
        for (head=1; head<length-1; head++) {
            if (string[head]=='(') {
                Lparen++;
            }
            if (string[head]==')') {
                Rparen++;
            }
            if ( (Lparen==Rparen) && isInBC(string[head]) ) {
                // currently head on BC
                break;
            }
        }
        // sanity check 1 -> Head is not on BC
        if (!isInBC(string[head])){
            return 0;
        }
        // sanity check 2 -> (p^q^r) : either or both part1 and part2 is not a formula
        
        if ( parse(sliced(string, 1, head)) && parse(sliced(string, head+1, (int)strlen(string)-1)) == 0) {
            return 0;
        }
        globalHead = head;
        topLevelBinaryConnective = string[head];
        return 3;
    }
    
    return 0;
}

// only if the string is in case 3, return first half
char *partone(char *binaryString){
    char *sliced1 = sliced(binaryString, 1, globalHead);
    return sliced1;
}

// only if the string is in case 3, return second half
char *parttwo(char *binaryString){
    char  *sliced2 = sliced(binaryString, globalHead+1, (int)strlen(binaryString)-1);
    return sliced2;
}


/*---------------------- Tableau Parser -------------------------*/
// We are dealing with only formula now
// 1. Build Tableau Tree.
// 2. Check if contradictions present.

/* A tableau will contain a formula and Tree. Use NULL for empty list.*/
struct tableau {
    char *formula;
    struct tableau *left;
    struct tableau *right;
    struct tableau *parent;
};
typedef struct tableau TABLEAU;

TABLEAU initialiseTableau(char *name){
    TABLEAU t = {
        name, NULL, NULL, NULL
    };
    return t;
}

// add ch to tab as a left child
void addLeft(TABLEAU *tab, char *child){
    // construct new tree
    TABLEAU *newTree = malloc(sizeof(TABLEAU));
    newTree -> formula = child;
    newTree -> left = NULL;
    newTree -> right = NULL;
    
    // add new tree on the left side of the origial tab
    tab -> left = newTree;
    newTree -> parent = tab;
}

// add ch to tab as a right child
void addRight(TABLEAU *tab, char *child){
    // construct new tree
    TABLEAU *newTree = malloc(sizeof(TABLEAU));
    newTree -> formula = child;
    newTree -> left = NULL;
    newTree -> right = NULL;
    
    // add new tree on the right side of the origial tab
    tab -> right = newTree;
    newTree -> parent = tab;
}

// Print Tableau Tree. For debugging and aesthetic :)
// getHeight, printLevel and printTreeByLevel are inspired from
// https://www.geeksforgeeks.org/level-order-tree-traversal/
int getHeight(TABLEAU *tableau){
    if (tableau == NULL) {
        return 0;
    }
    int leftMax = getHeight(tableau->left);
    int rightMax = getHeight(tableau->right);

    if (leftMax > rightMax) {
        return leftMax+2;
    }
    else return rightMax+2;
}

void printLevel(TABLEAU *tableau, int level){
    if (tableau == NULL) {
        return;
    }
    if (level == 1){
        printf("%s\n", tableau->formula);
    }
    else if (level > 1){
        printLevel(tableau->left, level-1);
        printLevel(tableau->right, level-1);
    }
}

void printTreeByLevel(TABLEAU *tableau){
    int level = getHeight(tableau);
    int i;
    for (i=0; i<level; i++) {
        printLevel(tableau, i);
    }
}

// returns negated string in a simplified format
char *negate(char* string){
    char *negated = malloc(Fsize);
    
    int count = 0;
    // remove all the negation sign
    while (string[0]=='-') {
        string++;
        count++;
    }
    
    if (count%2 == 0) {     // even number of '-' => prepend one '-'
        negated[0] = '-';
        return strcat(negated, string);
    }
    else{   // odd number of '-' => remove all '-'
        return string;
    }
}

// Does exactly opposite to negate function
char *simplifyNegation(char *string){
    char *simplified = malloc(sizeof(string)+1);
    
    int count = 0;
    // remove all the negation sign
    while (string[0]=='-') {
        string++;
        count++;
    }
    
    if (count%2==0) {   // even number of '-' => remove all '-'
        return string;
    }
    else{       // odd number of '-' => simplify to one '-'
        simplified[0] = '-';
        return strcat(simplified, string);
    }
}

// Returns Boolean
int isNegatedBinary(char *formula){
    return (parse(formula)==2 &&
            parse( sliced(formula, 1, (int)strlen(formula)) )==3
            );
}

// p or -p, which are Terminals in Tree
int isTerminal(char *formula){
    return (parse(formula)==1 ||
            parse( negate(formula) ) == 1
            );
}

/*
 Decides which expansion rule to use (returns one of strings below)
    proposition
    alpha
    beta
*/
char *rule(char *formula){
    formula = simplifyNegation(formula);
    
    // if proposition or negated proposition
    if ( isTerminal(formula) ) {
        return "proposition";
    }
    
    // negated binary
    if ( isNegatedBinary(formula) ) {
        if (topLevelBinaryConnective == '>' || topLevelBinaryConnective == 'v') {
            return "alpha";
        }
        else if (topLevelBinaryConnective == '^'){
            return "beta";
        }
        else{
            return "There is sth wrong with your topLevelBinaryConnective global variable DEBUG!! - from negated binary";
        }
    }
    
    // binary
    if (parse(formula)==3) {
        if (topLevelBinaryConnective=='>' || topLevelBinaryConnective=='v') {
            return "beta";
        }
        else if (topLevelBinaryConnective=='^'){
            return "alpha";
        }
        else{
            return "There is sth wrong with your topLevelBinaryConnective global variable DEBUG!! - from binary";
        }
    }
    
    return "Not a formula! You didn't expect this to be printed. DEBUG!";
}

void applyAlpha(TABLEAU *t, char *part1, char *part2){
//    char *part1 = partone(simplifyNegation(t->formula));
//    char *part2 = parttwo(simplifyNegation(t->formula));
    
    // if your are applying alpha at the last level node
    if (t->left == NULL) {
        addLeft(t, part1);
        addLeft(t->left, part2);
    }
    // t->left is not NULL, which means there was sth before you are doing this
    else{
        applyAlpha(t->left, part1, part2);
        if (t->right != NULL) {
            applyAlpha(t->right, part1, part2);
        }
    }
}

void applyBeta(TABLEAU *t, char *part1, char *part2){
//    char *part1 = partone(simplifyNegation(t->formula));
//    char *part2 = parttwo(simplifyNegation(t->formula));
    
    // if you are applying beta at the last level node
    if (t->left == NULL) {
        addLeft(t, part1);
        addRight(t, part2);
    }
    // t->left is not NULL, which means there was sth rule before you are doing this
    else{
        applyBeta(t->left, part1, part2);
        if (t->right != NULL) {
            applyBeta(t->right, part1, part2);
        }
    }
}


char *expandPartOne(char *formula){
    // Decide based on toplevelBinaryConnective
    if (isNegatedBinary(formula)) {
        char *partOneInside = partone(sliced(formula, 1, (int)strlen(formula)));
        
        switch (topLevelBinaryConnective) {
            case '>':
                return partOneInside;
                break;
            case 'v':
                return negate(partOneInside);
                break;
            case '^':
                return negate(partOneInside);
                break;
                
            default:
                printf("Debug - from expandPartOne/negatedBinary");
                break;
        }
    }
    // isBinary
    if (parse(formula)==3) {
        switch (topLevelBinaryConnective) {
            case '>':
                return negate(partone(formula));
                break;
            case 'v':
                return partone(formula);
                break;
            case '^':
                return partone(formula);
                break;
                
            default:
                printf("Debug - from expandPartOne/binary");
                break;
        }
    }
    // TODO: How can I raise error in C?
    return "sth is wrong - expandPartTwo";
}

char* expandPartTwo(char *formula){
    if (isNegatedBinary(formula)) {
        return negate(parttwo(sliced(formula, 1, (int)strlen(formula))));
    }
    if (parse(formula)==3) {
        return parttwo(formula);
    }
    
    return "sth is wrong - expandPartTwo";
}

// make tableau
void complete(TABLEAU *t){
    
    if (t != NULL) {
        char *fmla = simplifyNegation(t->formula);
        char *whichRule = rule(fmla);
        
        // Alpha rule
        if (strcmp(whichRule,"alpha")==0) {
            // simplifying Negation here is important
            applyAlpha( t, simplifyNegation(expandPartOne(fmla)), simplifyNegation(expandPartTwo(fmla)) );
        }
        
        // Beta rule
        if (strcmp(whichRule,"beta")==0) {
            applyBeta( t, simplifyNegation(expandPartOne(fmla)), simplifyNegation(expandPartTwo(fmla)) );
        }
        
        // Proposition
        if (strcmp(whichRule,"proposition")==0) {
            // Do nothing
        }
        
        // Recursively expand
        if (t->left != NULL) {
            complete(t->left);
        }
        if (t->right != NULL) {
            complete(t->right);
        }
    }
}

// For Debugging purpose.
// printTreeByDFS is inspired from
// https://www.geeksforgeeks.org/dfs-traversal-of-a-tree-using-recursion/
void printTreeByDFS(TABLEAU *tableau){
    if (tableau == NULL)
        return;
  
    /* first print data of node */
    printf("%s\n", tableau->formula);
  
    /* then recur on left sutree */
    printTreeByDFS(tableau->left);
  
    /* now recur on right subtree */
    printTreeByDFS(tableau->right);
}

// check if current node presents in parent nodes
// returns boolean
int swimUpandCheck(TABLEAU *t, char *string){
    if(t == NULL){
        return 0;
    }
    else{
        if(strcmp(t->formula, string)==0){
          return 1;
        }
        else{
          return swimUpandCheck(t->parent, string);
        }
    }
}

// check if there is a contradiction in one whole branch
int contradiction(TABLEAU *t){
    if (t==NULL){
        return 0;
    }
    else{
        return swimUpandCheck(t->parent, negate(t->formula));
  }
}
 
// return 0 -> satisfiable
// return 1 -> unsatisfiable
int closed(TABLEAU *t){
    if(contradiction(t)){
        return 1;
    }
    else{
        if(t==NULL){
            return 0;
        }
        if(t->right != NULL){
            return (closed(t->left) && closed(t->right));
        }
        else{
            return closed(t->left);
        }
    }
}

//main function that has to be submitted
int main(){
    /*input 10 strings from "input.txt" */
    char *name = malloc(Fsize);
    FILE *fp, *fpout;
    /*You should not need to alter the input and output parts of the program below.*/


    /* reads from input.txt, writes to output.txt*/
    if ( (fp=fopen("input.txt","r")) == NULL){
        printf("Error opening file");
        exit(1);
    }
    if ( (fpout=fopen("output.txt","w")) == NULL){
        printf("Error opening file");
        exit(1);
    }

    int j;

    for(j=0; j < inputLines; j++){
        /*read formula*/
        fscanf(fp, "%s",name);

        // parsing its type (not formula, prop, neg, bin)
        switch (parse(name)){
            case(0):
                fprintf(fpout, "%s is not a formula.  \n", name);
                break;
            case(1):
                fprintf(fpout, "%s is a proposition. \n ", name);
                break;
            case(2):
                fprintf(fpout, "%s is a negation.  \n", name);
                break;
            case(3):
                fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s  \n",
                        name, partone(name), parttwo(name));
                break;
            default:
                fprintf(fpout, "What the f***!  ");
        }

        // make a Tableau with formulae
        if (parse(name)!=0){
          /* here you should initialise a tableau t with theory in it*/
            TABLEAU t =  initialiseTableau(name);
          /* then you should call a function that completes the tableau t*/
            complete(&t);
            if (closed(&t)){
                fprintf(fpout, "%s is not satisfiable.\n", name);
            }
            else {
                fprintf(fpout, "%s is satisfiable.\n", name);
            }
        }
        else {
            fprintf(fpout, "I told you, %s is not a formula.\n", name);
        }
        //printf("----------------\n");
    }

    fclose(fp);
    fclose(fpout);
    free(name);
    free(binaryPart2);
    free(binaryPart1);
    // TODO: Why can't I free this??
    // free(t)
    return 0;
}
