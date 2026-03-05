#include <iostream>
#include <windows.h> // for colors
#include <cstdlib> // for clearing screen
#include <conio.h> //for getch();
#include <ctime>
#include <iomanip>
#include <string>
#include <fstream>
#include <cmath>
using namespace std;

//For clearing Screen
void clearScreen() 
{
		system("CLS");
}
// Color codes
const int BLACK = 0;
const int BLUE = 1;
const int GREEN = 2;
const int CYAN = 3;
const int RED = 4;
const int MAGENTA = 5;
const int YELLOW = 6;
const int WHITE = 7;

const int COLOR = 4;
const int TYPE = 15;

int winner=0, p1Moves=0, p2Moves=0;
int colorString[5]={RED,BLUE,GREEN,YELLOW,WHITE};
string colorS[4]={"Red","Blue","Green","Yellow"};  //used to change the color of every card when printing hand
string typeString[15]={" 0 "," 1 "," 2 "," 3 "," 4 "," 5 "," 6 "," 7 "," 8 "," 9 "," R "," S ","+2 "," W ","W+4"}; 

int cardCount=0,discardCount=0,p1Count=7,p2Count=7,topCard,turn=1,turnNumber=1,player,playerCard,wildColorCall; //initializing some global variables to access in every function
int deck[4][15], shuffledDeck[108][2],discardPile[108][2];
int p1[108],p2[108], lastCard=107;
bool exitG = false; 

void initializeDeck(int deck[][15]) //Initialize the deck with all UNO cards.
{
		for(int color=0;color<COLOR;color++){
			deck[color][0]=1; //one '0' card
			for(int type=1;type<TYPE-2;type++)
				deck[color][type]=2; //2 cards each for 1-9, reverse[10], skip[11], draw 2[12]
			deck[color][13]=1;//1 'Wild' card[13]
			deck[color][14]=1;//1 'Wild Draw Four' card[14]
		}
}

void shuffleDeck(int deck[4][15]) //Shuffle the deck to randomize the card order
{
	int color,type,size=0;
	while(size<108) //while the new shuffled array isn't filled
	{
		color=rand()%4; //random color
		type=rand()%15; //random type
		if(deck[color][type]>0) //if there is any card of that specific type left in the main deck
		{
			shuffledDeck[size][0]=color;
			shuffledDeck[size][1]=type;
			size++;
			deck[color][type]--; //decreasing one card of that specific type in the main deck
		}	
	}	
}

void displayCards(int[][2],int);
void reShuffle()
{
	int j;
	for(int i=0;i<discardCount;i++)
	{
		j=rand()%discardCount;
		swap(discardPile[i][0],discardPile[j][0]);
		swap(discardPile[i][1],discardPile[j][1]);
	}
	cout<<"Discard Pile after Shuffling: "<<endl;
	displayCards(discardPile,discardCount);
	int cc=cardCount;
	cardCount=0;
	shuffledDeck[cardCount][0]=shuffledDeck[topCard][0];
	shuffledDeck[cardCount][1]=shuffledDeck[topCard][1];
	topCard=cardCount++;
	int p1temp[108][2],p2temp[108][2],temp[10][2],tempCount=0;
	for(int i=0;i<p1Count;i++)
	{
		p1temp[i][0]=shuffledDeck[p1[i]][0];
		p1temp[i][1]=shuffledDeck[p1[i]][1];
	}
	for(int i=0;i<p2Count;i++)
	{
		p2temp[i][0]=shuffledDeck[p2[i]][0];
		p2temp[i][1]=shuffledDeck[p2[i]][1];
	}
	cout<<"P1 Hand: "<<endl;
	displayCards(p1temp,p1Count);
	cout<<"P2 Hand: "<<endl;
	displayCards(p2temp,p2Count);
	for(int i=0;i<p1Count;i++)
	{
		shuffledDeck[cardCount][0] = p1temp[i][0];
		shuffledDeck[cardCount][1] = p1temp[i][1];
		p1[i]=cardCount++;
	}
	for(int i=0;i<p2Count;i++)
	{
		shuffledDeck[cardCount][0] = p2temp[i][0];
		shuffledDeck[cardCount][1] = p2temp[i][1];
		p2[i]=cardCount++;
	}

	for(int i=0;i<108-cc;i++)
	{
		temp[i][0] = shuffledDeck[cc + i][0];
		temp[i][1] = shuffledDeck[cc + i][1];
		tempCount++;
	}
	int counter = cardCount;
	for(int i=0;i<tempCount;i++)
	{
		shuffledDeck[counter][0]=temp[i][0];
		shuffledDeck[counter][1]=temp[i][1];
		counter++;
	}
	for(int i=0;i<discardCount;i++)
	{
		shuffledDeck[counter][0]=discardPile[i][0];
		shuffledDeck[counter][1]=discardPile[i][1];
		counter++;
	}
	discardCount = 0;
}

int callColor();
void setColor(int textColor, int bgColor){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

bool exitToMenu() //confirmation to end the game for good
{
	cout<<"Press 'E' again to confirm exit to Main Menu, or Press 'B' to return back to the game"<<endl; 
	char input;
	do
	{
		input=getch();
		if(input!='B'&&input!='b'&&input!='E'&&input!='e') //validating the options 
			cout<<"Invalid Command!"<<endl; //else invalid 
	}
	while(input!='B'&&input!='b'&&input!='E'&&input!='e'); //it loops untill a valid option is entered
	if(input=='e' || input=='E')
	{
		cout << "Game ended successfully!!\n";
		return true; //signifies ending game
	}	
	else
		return false; //plauyer changed his mind
}

void print(int index){
	int color,cardNumber;
	color=shuffledDeck[index][0]; //takes color out of that index
	cardNumber=shuffledDeck[index][1]; //stores the type of card
	if(cardNumber==13||cardNumber==14) //special cards without a color having special color
		color=4;
	setColor(BLACK,colorString[color]); //setting color
	cout << "---------" <<endl;
	cout << "-       -" <<endl;
	cout << "-  "<<typeString[cardNumber]<<"  -"<<endl;
	cout << "-       -" <<endl;
	cout << "---------" <<endl;
	setColor(WHITE, BLACK); //resetting the color back to normal
	cout<<endl;
}

void printPlayerHand(int hand[],int handSize){ //taking hand and its size
	int cardNumber,color; 
	for(int i=0;i<handSize;i++) //outputting the first line of every card at once horizontally
	{
		color=shuffledDeck[hand[i]][0]; //color of each individual color
		cardNumber=shuffledDeck[hand[i]][1]; //type of each individual color
		if(cardNumber==13||cardNumber==14) //speacial color for wild4 and wild
			color=4;
		setColor(BLACK,colorString[color]); //setting color
		cout << "---"<<"--"<<"----"; //printing lines
		setColor(WHITE,BLACK); //resetting the color
		cout<<" "; //adding a little space between the cards
	}
	cout<<endl;//next line of the cards
	for(int i=0;i<handSize;i++){
		color=shuffledDeck[hand[i]][0];
		cardNumber=shuffledDeck[hand[i]][1];
		if(cardNumber==13||cardNumber==14)
			color=4;
		setColor(BLACK,colorString[color]);
		cout << "-  "<<"  "<<"   -";
		setColor(WHITE,BLACK);
		cout<<" ";
	}
	cout<<endl;
	for(int i=0;i<handSize;i++)	{
		color=shuffledDeck[hand[i]][0];
		cardNumber=shuffledDeck[hand[i]][1];
		if(cardNumber==13||cardNumber==14)
			color=4;
		setColor(BLACK,colorString[color]);
		cout << "-  "<<typeString[cardNumber]<<"  -"; //printing individual name of the card in line, extracting from the array initially created
		setColor(WHITE,BLACK);
		cout<<" ";
	}
	cout<<endl;
	for(int i=0;i<handSize;i++)	{
		color=shuffledDeck[hand[i]][0];
		cardNumber=shuffledDeck[hand[i]][1];
		if(cardNumber==13||cardNumber==14)
			color=4;
		setColor(BLACK,colorString[color]);
		cout << "-  "<<"  "<<"   -";
		setColor(WHITE,BLACK);
		cout<<" ";
	}
	cout<<endl;
	for(int i=0;i<handSize;i++)	{
		color=shuffledDeck[hand[i]][0];
		cardNumber=shuffledDeck[hand[i]][1];
		if(cardNumber==13||cardNumber==14)
			color=4;
		setColor(BLACK,colorString[color]);
		cout << "---"<<"--"<<"----";
		setColor(WHITE,BLACK);
		cout<<" ";
	}
	cout<<endl;
	cout<<"    "; //space accordingly
	for(int i=0;i<handSize;i++)	{
//		cout << "    "<<i+1<<"     ";
		cout<<left<<setw(10)<<i+1; //using setw to carefully print the numbers below each card
	}
	setColor(WHITE,BLACK);
	cout<<endl;
}

void printBoard(int hand[],int handSize,int player){	//prints board
	cout<<"Top Card: "<<endl;
	print(topCard); //printing topcard
	if((shuffledDeck[topCard][1]==13 || shuffledDeck[topCard][1]==14)&&turnNumber>1) //if there is wild or wild+4
		cout<<"Wild Call: "<<colorS[wildColorCall]<<endl; //print the color chosen by the player so that the color is known
	cout<<"Player "<<player<<" Hand: "<<endl; //print the player who is playing at he moment
	printPlayerHand(hand,handSize); //print the hand of the player
}

void draw(int count, int hand[], int &handSize){ //draws a card to the player hand
	for(int i=0;i<count;i++){  //number of cards to be drawm
		hand[handSize]=cardCount; //assign card
		handSize++; //increment handSize
		cardCount++; //increment the shuffled Deck index
	}
}

void dealCards(){
	cardCount=0; //initially zero
	topCard=cardCount++; //use the first card as the top card
//	shuffledDeck[topCard][1]=14;
	for(int i=0;i<7;i++){ //assign cards to player hands alternatively from shuffled deck
		p1[i]=cardCount;
		cardCount++; //increment index
		p2[i]=cardCount;
		cardCount++;
	}
	if(shuffledDeck[topCard][1]==10 || shuffledDeck[topCard][1]==11) //if by any chance the topCard == special skip or reverse
		turn=2; //skip to next player
	else if(shuffledDeck[topCard][1]==12) //incase of draw 2
	{
		draw(2,p1,p1Count); //make player 1 draaw 2 cards
		turn=2; //skips the turn
	}
	else if(shuffledDeck[topCard][1]==14) //draw 4
	{
		draw(4,p1,p1Count); //make player 1 draaw 4 cards
		turn=2; //skip the turn
	}
	
}

int selectCard(int hand[], int handSize){ //deciding the card to play
	bool isValid;
	int index;
	string b;
	int j;
	cout<<"Enter a card (1-"<<handSize<<") or Press 'E' to exit: "; //show the options
	do{
		isValid=true;
		getline(cin,b); //get input as string
		j=0,index=0; //index is the number of card chosen by player 
		for(int i=b.length()-1;i>=0;i--) //loop the code below till the length of the string to find if a valid number
		{
			if((b[0]=='e' || b[0]=='E') && b.length()==1) //if e is entered as string, treat it as pause sequence, and if sing e is entered
			{
				exitG=exitToMenu(); //take confirmation
				if(exitG) //if confirmedd
					return -1; //break function, and don't take input
				else
					isValid = false; //incase if player changed his mind, invert the bool to take input again
			}
			else if(b[i]<48 || b[i]>57) //if the extracted char value from string is not a valid digit
			{
				isValid=false; //invert the bool to take input again
				break; 
			}
			else
				index+=(b[i]-48)*pow(10,j++);// if extracted character is valid digit, add it to index after multiplying with its scope (whether it is at ones place, tenths place, etc)
		}
		if(index<1 || index>handSize || !isValid) //if invalid input
			cout<<"Enter a valid card (1-"<<handSize<<") or Press 'E' to exit: "; //display message and ask for input again
	}
	while(index<1 || index>handSize || !isValid); //loop again till right input

	index--; //decrementing it to use it as index
	playerCard=hand[index]; //putting the index of the card of shuffled deck chosen by the player from hand
	return index; // return the index
}

bool isValidPlay(int playerCard, int topCard)
{
//	cout<<shuffledDeck[playerCard][0]<<" "<<shuffledDeck[playerCard][1]<<endl;
//	cout<<shuffledDeck[topCard][0]<<" "<<shuffledDeck[topCard][1]<<endl;
	if(shuffledDeck[playerCard][0]==shuffledDeck[topCard][0] || shuffledDeck[playerCard][1]==shuffledDeck[topCard][1] || shuffledDeck[playerCard][1]==13 || shuffledDeck[playerCard][1]==14)
	{ //if the color of the card chosene by the user = to the color of the topcard or if the type of both the cards is same or if the player plays wild card or draw 4
		return 1; //return true
	}
	return 0; //else return false
}

char in() //function to validate input by the user
{
	char input;
	do
	{
		input=getch();
		if(input!='P'&&input!='p'&&input!='D'&&input!='d'&&input!='E'&&input!='e'&&input!='Y'&&input!='y') //if the input is anything but these, output invalid
			cout<<"Invalid Command!"<<endl;
	}
	while(input!='P'&&input!='p'&&input!='D'&&input!='d'&&input!='E'&&input!='e'&&input!='Y'&&input!='y'); //loop if invalid
	return input; //return valid character
}

int strToNum(string str){ //function to do strToNum
	int scope =1, numtemp, num=0;
	for(int i=1;i<str.length();i++)
		scope *= 10; //finds the maximum scope of the number, (if it is in hundreds, ones or thousansands
	for(int i=0;i<str.length();i++){ //traversing through the length of the str
		for(int j=48;j<=57;j++){ //to check ASCII of the digits
			if((str[i])==j){ //if the ASCII equals the char which is extracted from the string
				numtemp = (j-48) * scope; //store the digit equalent to the char after multiplying with it's scope
				scope /=10; //decrease the scope
				break; //break in case the ASCII matches
			}
		}
		num +=numtemp; //add the numbers into one
	}
	return num;
}

string numToStr(int num){
	int exit =1, digit;
	string str = "";
	while(1){
		digit = num %10; //string the last digit in the number
		switch(digit){ //apending the string with appropriate str based on the digit
			case 0 : str = "0" + str;	break;
			case 1 : str = "1" + str;	break;
			case 2 : str = "2" + str;	break;
			case 3 : str = "3" + str;	break;
			case 4 : str = "4" + str;	break;
			case 5 : str = "5" + str;	break;
			case 6 : str = "6" + str;	break;
			case 7 : str = "7" + str;	break;
			case 8 : str = "8" + str;	break;
			case 9 : str = "9" + str;			
		}
		num /= 10; //decreasing the number 
		if(exit==2) //breaking the loop
			break;
		if(num/10==0) //if this is true, the loop should work only one more time
			exit++;	
	}
	return str;
}

int callUno(int hand[],int handSize){
	pause:
	char uno;
	cout << "Enter 'U' confirm Uno call or 'B' to go back or 'E' to open Pause Menu";
	uno = getch();
	cout<<endl;
	while(uno!='U' && uno!='u' && uno!='B' && uno!='b' && uno!='E' && uno!='e'){ //loop if invalid input
		cout << "Invalid Input, Re-Enter";
		uno = getch(); //take in input
	}
	if(uno=='u' || uno=='U'){ //if uno called
		if (handSize!=2) // and if handSize is 2, return 2
			return 2;
		else //else wrong uno call
			return 1;
	}
	else if(uno=='E'||uno=='e') //if player wants to enter pause menu
	{
		exitG = exitToMenu(); //confirmation to end game
		if(!exitG) //if change of mind, take input again
			goto pause;
		else //else break the function as the game has ended
			return 3;
	}
	return 0; //go back if b pressed as the player had a change of mind
}

void saveLoadGameResult(int winner, int p1Moves, int p2Moves){ 
	string line;
	int lineCount = 0;
	ifstream inFile("records.txt"); //opens the file to read
    if(inFile.is_open()){ //file error handling
        while (getline(inFile, line)) { //counting the lines in the file
            lineCount++;
        }
        inFile.close(); //close file
    } 
	else
        cout << "Unable to open file for reading.\n"; //enable to open the file, error message
	if(winner==0){ //if game not won yet
		if(lineCount!=0){ //and length of the file is not zero
			ifstream inFile("records.txt"); //opening file
			if(inFile.is_open()){ //output the first three lines of the file used to store the record
				getline(inFile, line);
				cout << "The total games played: "<<line<<endl;
				getline(inFile, line);
				cout << "Games won by Player 1: "<<line<<endl;
				getline(inFile, line);
				cout << "Games won by Player 2: "<<line<<endl;
				while (getline(inFile, line)) { //prints rest of the lines in the file
	           		cout << line<<endl; //outputting
	        	}
			}
			else{
				cout << "Unable to open the file!\n";
			}
		}
		else{ //if the line count is zero, no games played and all value zero printed
			cout << "Total games played: 0\n";
			cout << "Games won by Player 1: 0\n";
			cout << "Games won by Player 2: 0\n";
		}
	}
	else{ //winner is non zero
	    if(lineCount==0) //if there is some winner and line count =zero
	    	lineCount = 3; //initialize it to 3
	    string arr[lineCount + 3]; //declare an array of line count + the new record of the game
		if (lineCount == 3){ //if linecount == 3
			arr[0] = "0"; //initilize the first three elements to zero
			arr[1] = "0"; 
			arr[2] = "0"; 
		}
	    inFile.open("records.txt"); //file opened
	    if(inFile.is_open()){ //error handline
	    	int count=0;
	        while (getline(inFile, line)) { //reading 
	            arr[count++] = line; //storing the lines of the file in the array
	        }
	        inFile.close(); //close the file
	    } 
		else
	        cout << "Unable to open file for reading.\n";
	    arr[0] = numToStr(strToNum(arr[0])+1); //increasing the matches by one
	    arr[lineCount] = "Player "+ numToStr(winner)+" won the game"; //stroring the new game log
	    arr[lineCount+1] = "Player1 moves = "+ numToStr(p1Moves); //game log
	    arr[lineCount+2] = "Player2 moves = "+ numToStr(p2Moves); //game log
	    if (winner ==1) //incremeting the wins of individula players
	    	arr[1] = numToStr(strToNum(arr[1])+1); 
	    else
	    	arr[2] = numToStr(strToNum(arr[2])+1);
	    ofstream outFile("records.txt"); //open file for write
		if(outFile.is_open()){
	        for(int i=0;i<(lineCount+3);i++){ //wrinting the file for the length of the array
	        	outFile << arr[i]; //writing line by line using array
	        	if(i < lineCount + 2) //moving on the next lne in file except for the last line of the fi
					outFile << '\n';
			}
	        outFile.close();
	    } 
		else
	        cout << "Unable to open file for writing.\n";
	}	        
}

int callColor()
{
	bool isValid;
	int index;
	string b;
	int j;
	cout<<"Choose the color for next move:"<<endl;
	for(int i=0;i<4;i++) //printing the color options
		cout<<i+1<<". "<<colorS[i]<<endl;
	do
	{
		isValid=true;
		getline(cin,b); //taking input as string
		j=0,index=0;
		for(int i=b.length()-1;i>=0;i--) //for looping through the the length of the string 
		{
			if((b[0]=='e' || b[0]=='E') && b.length()==1) //checking if the user wants to enter the pause menu
			{
				exitG=exitToMenu(); //confiemattion of the exit
				if(exitG)
					return -1; //incase the user wants exit
				else
					isValid = false; //for taking the input again from the user
			}
			else if(b[i]<48 || b[i]>57) //if the digit extracted is not a valid digit
			{
				isValid=false; //invert the bool for taking the input again
				break;
			}
			else
				index+=(b[i]-48)*pow(10,j++); //making a digit by using its ASCII value and multiplying it by it's scope (hundreds, ones, or thousands)	
		}
		if(index<1 || index>4 || !isValid) //error message printed
			cout<<"Enter a valid color: ";
	}
	while(index<1 || index>4 || !isValid); //loop till you find the correct color chosen by the user
	index--; //changing it to index form from the number form
	return index; //returning index of the color selected
}

void handleSpecialCard(int specialCardType)
{
	if(specialCardType>9 && specialCardType<15) //if it is a speacial card
	{
//		cout<<"Card Number of Special Card: "<<specialCardType<<endl; 
		switch(specialCardType)
		{
			case 10: case 11: //Reverse //Skip
				turn=player;
				cout<<"Player: "<<player<<endl;
				break;
			case 12: //Draw Two
				if(player==1)
					draw(2,p2,p2Count);
				else if(player==2)
					draw(2,p1,p1Count);//give 2 cards to the player  who isn't playing
				turn=player; //skip the turn
				break;
			case 13: //Wild
				wildColorCall=callColor();
				shuffledDeck[topCard][0]=wildColorCall; //let the player choose a color
				break;	
			case 14: //Wild Draw Four
				if(player==1) //give 4 cards to the player  who isn't playing
					draw(4,p2,p2Count);
				else if(player==2)
					draw(4,p1,p1Count);
				wildColorCall=callColor(); //color choosing
				shuffledDeck[topCard][0]=wildColorCall; //giving it a color
				turn=player;
				break;
		}	
	}
}

void updateDiscardPile()
{
	discardPile[discardCount][0]=shuffledDeck[topCard][0]; //sending the color of the topcard to discard pile
	discardPile[discardCount][1]=shuffledDeck[topCard][1]; //sending the type of the topcard to discard pile
	discardCount++;	//incrementing the index of the shuffled deck
}

void swap(int &a,int &b)
{
    int temp;
    temp = a;
    a = b;
    b = temp;
}

void playTurn(int hand[], int &handSize,int player) //handles a turn
{
	if(shuffledDeck[topCard][1]==13 && turnNumber==1) //if the first ever card is wild, let player 1 choose the color
	{
		wildColorCall=callColor();
		shuffledDeck[topCard][0]=wildColorCall;
	}
	else if(shuffledDeck[topCard][1]==14 && turnNumber==1) //if the first ever card is draw 4 let player 2 choose the color and play
	{
		wildColorCall=callColor();
		shuffledDeck[topCard][0]=wildColorCall;
		turn=2;	
	}
	else
	{
		char input;
		int choice=-1;
		flag1:
		cout<<"Press 'P' to Play a Card or 'D' to Draw a Card or 'E' to open the Pause Menu or 'Y' to call UNO"<<endl; //menu
		input=in();
		if(input=='E'||input=='e') //enter exit menu
		{
			exitG = exitToMenu(); //confirmation
			if(!exitG)
				goto flag1; //change of mind, take input again
		}
		else if (input=='Y'||input=='y'){ //willing to call uno
			choice = callUno(hand, handSize); //numbers based on conditions thrown into choice
			if(choice==0) //change of heart and not declaring uno
				goto flag1;
			else if(choice==1) //uno called at the right time
				cout << "Uno successfully called!!\n";
			else if(choice==2) //uno called at the wrong time
				cout << "Wrong Uno call!!\n";
			else if(choice==3) //breaking the function and ending th game session
				return;
			goto flagUno1; //choosing a card after saying uno, whether right or wrong
		}
		if(input=='P'||input=='p')
		{
			flagUno1:
			int playerCardIndex=selectCard(hand,handSize); //player card index chosen by the user stored in the variable 
			if(playerCardIndex!=-1) //in case exit fuunction called and game ended successfully
			{
				if(choice == 2||(handSize==2 && choice==-1)) //if after playing a card, wrong uno was called or uno was not called at the right time
					draw(2,hand,handSize); //giving two cards to the user
//			playerCard=hand[playerCardIndex];
				if(isValidPlay(playerCard,topCard)) //check whether the card selected by the user is playable or not
				{
					updateDiscardPile(); //updating discard pile if card is valid
					topCard=playerCard;
					for(int i=playerCardIndex;i<handSize-1;i++) //removing the card from the hand
					{
						hand[i]=hand[i+1]; //hand array traversed
					}
					handSize--; //handSie decremented
					handleSpecialCard(shuffledDeck[playerCard][1]); //check if the card played by the user was a special card
				}
				else
				{
					cout<<"Invalid Move"<<endl; //error statement in case in valid card
					goto flag1; //retaking the input from the user if invalid input
				}
			}
		}
		else if(input=='D'||input=='d') //if draw card was selected
		{
			choice = -1; //re-initializing the choice variable to check again
			draw(1,hand,handSize); //make the player draw 1 card
			clearScreen(); //clear screen
			printBoard(hand,handSize,player); // print the board again as the user might throw the card drawn
			flag2:
			cout<<"Press 'P' to Play a Card or 'D' to Pass Turn without discarding a Card or 'E' to open the Pause Menu or 'Y' to call UNO"<<endl;
			input=in();
			if(input=='E'||input=='e')//enter exit menu
			{
				exitG = exitToMenu(); //confirmation
				if(!exitG)
					goto flag2; //change of mind, take input again
			}
			else if (input=='Y'||input=='y'){//willing to call uno
				choice = callUno(hand, handSize); //numbers based on conditions thrown into choice
				if(choice==0) //change of heart and not declaring uno
					goto flag2;
				else if(choice==1) //uno called at the right time
					cout << "Uno successfully called!!\n";
				else if(choice==2) //uno called at the wrong time
					cout << "Wrong Uno call!!\n";
				else if(choice==3) //breaking the function and ending th game session
					return;
				goto flagUno2; //choosing a card after saying uno, whether right or wrong
			}
			else if(input=='P'||input=='p')
			{
				flagUno2:
				int playerCardIndex=selectCard(hand,handSize); //player card index chosen by the user stored in the variable

				if(playerCardIndex!=-1) //in case exit fuunction called and game ended successfully
				{
					if(choice ==2 ||(handSize==2 && choice==-1)) //if after playing a card, wrong uno was called or uno was not called at the right time
						draw(2,hand,handSize);  //giving two cards to the user
					playerCard=hand[playerCardIndex];
					if(isValidPlay(playerCard,topCard)) //check whether the card selected by the user is playable or not
					{
						updateDiscardPile(); //updating discard pile if card is valid
						topCard=playerCard; 
						for(int i=playerCardIndex;i<handSize-1;i++) //removing the card from the hand
						{
							hand[i]=hand[i+1]; //hand array traversed
						}
						handSize--; //handSie decremented
						handleSpecialCard(shuffledDeck[playerCard][1]); //check if the card played by the user was a special card
					}
					else
					{
						cout<<"Invalid Move"<<endl; //error statement in case in valid card
						goto flag2; //retaking the input from the user if invalid input
					}
				}
			}
		}
	}
}
void printDeck(int deck[][15]){
	for(int i=0;i<4;i++){
		for(int j=0;j<15;j++){
			cout<<deck[i][j]<<" ";
		}
		cout<<endl;
	}
}

void displayCards(int arr[][2],int size){
	for(int i=0;i<size;i++){
		cout<<i<<". "<<arr[i][0]<<" "<<arr[i][1]<<endl;
	}
}

void displayCards(int hand[],int handSize){
	for(int i=0;i<handSize;i++){
		cout<<i<<". "<<shuffledDeck[hand[i]][0]<<" "<<shuffledDeck[hand[i]][1]<<endl;
	}
}

char mainMenu()
{
	cout << "MAIN MENU"<<endl; //printing main menu
	cout << "1. Play The Game"<<endl;
	cout << "2. Load Game History"<<endl;
	cout << "3. Exit The Game"<<endl;
	char o;
	do
	{
		o=getch(); //taking input
	}
	while(o!='1' && o!='2' && o!='3'); //loop the code, untill correct input
	return o; //return choice
}

void playGame()
{
	cout << endl;
//		cout<<"Turn: "<<turn<<endl;
		if(turn==1)
		{
			player=1; //player =1 when turn =1 
			turn=2; //changing the turn so that next player plays the game
			printBoard(p1,p1Count,player); //print board
			playTurn(p1,p1Count,player); //handling a turn
			p1Moves++; //incrementing the p1Moves
			clearScreen();
		}
		else if(turn==2)
		{
			player=2; //player =2 when turn =2
			turn=1; //changing the turn so that next player plays the game
			printBoard(p2,p2Count,player);
			playTurn(p2,p2Count,player); //handling a turn
			p2Moves++; //incrementing the p2Moves
			clearScreen();
		}
		if(cardCount>102) //when cardCOunt increases from 102, repopulate the deck with discardPile
			reShuffle();
		if(p1Count==0 || p2Count==0) //When the hand of a player ends, end game
			exitG=true;
		if(!exitG) //if exitG is false, game the game hasn't ended
		{
			turnNumber++; //increase the total number of turns
//			displayCards(discardPile,discardCount);
		}
		if(p1Count==0)
		{
			cout<<"Player 1 has won the game!"<<endl; //wining output
			winner =1;
			saveLoadGameResult(winner,p1Moves,p2Moves); //calling function to store game result
			winner=0; //winner reinitialized for a new game
		}
		else if(p2Count==0)
		{
			cout<<"Player 2 has won the game!"<<endl;
			winner =2;
			saveLoadGameResult(winner,p1Moves,p2Moves);
			winner=0; //winner reinitialized for a new game
		}
}

void resetGame() //resets the index pointers of the arrays used in th progam
{
	discardCount=0;
	p1Count=7;
	p2Count=7;
	turn=1;
	turnNumber=1;
}

int main(){
	srand(time(0));
//	cout<<"P1 Hand: "<<endl;
//	displayCards(p1,p1Count);
//	cout<<"P2 Hand: "<<endl;
//	displayCards(p2,p2Count);
//	for(int i=0;i<p1Count;i++)
//		cout<<p1[i]<<" ";
//	cout<<endl;
	char menu;
	p1Count=0;
	bool exitMenu=false; //making the loop work for first time
	while(!exitMenu)
	{
		clearScreen();
		menu = mainMenu(); //menu choice
		switch(menu)
		{
			case '1': //when user enter main menu after starting the game, the game ends and record is stored if it should be stored, and a new game starts after user enters 1
				resetGame();
				initializeDeck(deck); //reinitializing deck
				shuffleDeck(deck); //shuffling deck
				dealCards(); //dealing cards
				while(!exitG) //loop the code while the user doesn't exists the game by ending it
				{
//					cout<<"Shuffled Deck: "<<endl;
//					displayCards(shuffledDeck,108);
//					cout<<"P1 Hand: "<<endl;
//					displayCards(p1,p1Count);
//					cout<<"P2 Hand: "<<endl;
//					displayCards(p2,p2Count);
//					cout<<"Card Count: "<<endl;
//					cout<<cardCount<<endl;
//					cout<<"Discard Count: "<<endl;
//					cout<<discardCount<<endl;
//					cout<<"Discard Pile: "<<endl;
//					displayCards(discardPile,discardCount);
					playGame();
				}
				if(p1Count==0||p2Count==0)  //if a game is successfully concluded, wait for a buffer before entering main menu
				{
					cout<<endl<<"Press any key to continue to main menu...";
					getch();
				}
				exitG=false; //reassign exitG to be used for the next game
				break;
			case '2': //outputting the game records
				cout<<"\nGame Results: "<<endl;
				saveLoadGameResult(winner,p1Moves,p2Moves);
				cout<<endl<<"Press any key to return to Main Menu..."<<endl; //if game records successfully shown then wait for the buffer to enter in to the main menu
				getch();
				break;
			case '3':
				cout<<"Game exited successfully. Thank you for playing UNO!"; //terminate the program
				exitMenu=true; //exit condition the loop set for successfully exiting
		}	
	}
	return 0;
}
