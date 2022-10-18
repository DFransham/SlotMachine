/***********************************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand
(c) 2022 Media Design School
File Name : Main.cpp
Description : Mini project - slot machine mini game
Author : David Fransham
Mail : david.fransham@mds.ac.nz
**************************************************************************/

#include <iostream>
#include <windows.h>
#include <string>

using std::string;

//This class is to hold various variables relating to the user, to avoid using global variables
class SlotMachineUser
{
public:
	int LastSpin[3] = { 0,0,0 }; //stores the last spin values, for reprinting and keeping UI tidy

	void SetOutput(string _newOutput)
	{
		LastOutput = _newOutput;
	}

	string GetOutput()
	{
		return LastOutput;
	}

	void SetInput(string _newInput)
	{
		LastInput = _newInput;
	}

	string GetInput()
	{
		return LastInput;
	}

	void AddError()
	{
		CumulativeInputErrors++;
	}

	int GetErrors()
	{
		return CumulativeInputErrors;
	}

	void AddChips(int _iChipsToAdd)
	{
		CurrentChips += _iChipsToAdd;
	}

	int GetChips()
	{
		return CurrentChips;
	}

	//negative value to cash out, positive value to buy more
	void CashInOrOut(int _iAmountChanged)
	{
		CurrentChips += _iAmountChanged;
		CumulativeMoney -= _iAmountChanged;
	}

	int GetFinancialPosition()
	{
		return CumulativeMoney + CurrentChips;
	}

private:
	string LastInput; //used to store last input value, for reprinting and keeping UI tidy
	string LastOutput; //used to store last output value, for reprinting and keeping UI tidy
	int CumulativeInputErrors = 0; //counts input errors to allow casino security to warn user for repeated infractions
	int CurrentChips = 0; //stores current value of chips the user has on the table
	int CumulativeMoney = 0; //keeps track of how much money the user has spent or cashed out
};

//Constant definitions
enum ESpinResultCode
{
	LOSING_SPIN = 0, //no numbers match (0x bet multiplier)
	TWO_NUMS_MATCH = 3, //two numbers match (3x bet multiplier)
	THREE_NUMS_MATCH = 5, //triple but not 7s (5x bet multiplier)
	JACKPOT_THREE_SEVENS = 10, //triple 7s (10x bet multiplier)
};

enum class EExitCode
{
	OUT_OF_CHIPS,
	USER_CHOSE_QUIT,
	TOO_MANY_BAD_INPUTS,
};

enum class EInputErrors
{
	NOT_NUMBER,
	NOT_ON_MENU,
	INVALID_BET,
	NO_INPUT_GIVEN,
};

enum class EColour
{
	COLOUR_WHITE_ON_BLACK = 0, // White on Black.
	COLOUR_RED_ON_BLACK = 1, // Red on Black.
	COLOUR_GREEN_ON_BLACK = 2, // Green on Black.
	COLOUR_YELLOW_ON_BLACK = 3, // Yellow on Black.
	COLOUR_BLUE_ON_BLACK = 4, // Blue on Black.
	COLOUR_MAGENTA_ON_BLACK = 5, // Magenta on Black.
	COLOUR_CYAN_ON_BLACK = 6, // Cyan on Black.
	COLOUR_BLACK_ON_GRAY = 7, // Black on Gray.
	COLOUR_BLACK_ON_WHITE = 8, // Black on White.
	COLOUR_RED_ON_WHITE = 9, // Red on White.
	COLOUR_GREEN_ON_WHITE = 10, // Green on White.
	COLOUR_YELLOW_ON_WHITE = 11, // Yellow on White.
	COLOUR_BLUE_ON_WHITE = 12, // Blue on White.
	COLOUR_MAGENTA_ON_WHITE = 13,// Magenta on White.
	COLOUR_CYAN_ON_WHITE = 14, // Cyan on White.
	COLOUR_WHITE_ON_WHITE = 15 // White on White.
};

//user defined function prototypes
int GetMenuSelection(SlotMachineUser* _user);
int GetRandomNumber(int _iMinRand, int _iMaxRand);
int GetUserInput(SlotMachineUser* _user);
int GetScreenWidth();
int GetScreenHeight();
int GetSlotWinOrLose(SlotMachineUser* _user);
bool IsOnlyNumbers(const string& _str);
bool DoYouWishToContinue(SlotMachineUser* _user);
string DescribeCurrentPosition(bool _bStillPlaying, int _iMoney);

void RunSlots(SlotMachineUser* _user);
void CashOutChips(SlotMachineUser* _user);
void BuyMoreChips(SlotMachineUser* _user);
void GetUserBet(SlotMachineUser* _user);
void GoToXY(int _iX, int _iY);
void SetRgb(EColour _Colour);
void ExitSlots(EExitCode _ExitCode, SlotMachineUser* _user);
void PrintSlotUI(SlotMachineUser* _user, bool _bIncludeLast = true);
void CheckErrorCounter(int _iErrors, SlotMachineUser* _user);
void PrintLastSpin(SlotMachineUser* _user);
void ClearScreen();
void InvalidInput(EInputErrors _ErrCode, SlotMachineUser* _user);
void StartSlots(int _iPlayerBet, SlotMachineUser* _user);

int main()
{
	//initialising user object and attributes
	SlotMachineUser playerOne;
	playerOne.SetInput("Welcome to the GD1P01_22071 Mini Project: Slot Machine!");
	playerOne.SetOutput("Please gamble wisely.");
	playerOne.CashInOrOut(2000);

	for (int i = 0; i < 3; i++)
	{
		playerOne.LastSpin[i] = 7;
	}

	SlotMachineUser* pSlotUser = &playerOne;

	//seed "random" number generator to increase appearance of randomness
	srand((unsigned int)time(0));

	//keeps looping to the main menu while player has money left
	bool repeatBlock = true;
	do
	{
		while (playerOne.GetChips() > 0)
		{
			RunSlots(pSlotUser);
		}

		if (playerOne.GetChips() == 0)
		{
			PrintSlotUI(pSlotUser, true);

			repeatBlock = DoYouWishToContinue(pSlotUser);
		}
	} while (repeatBlock); //probably never gets reset to false, but program exits from within functions if this would be false.

	return 0;
}

//Asks user to deposit more money when they have run out.  If not, ends program via function call.
bool DoYouWishToContinue(SlotMachineUser* _user)
{
	bool inputInvalid = false; //used to loop until valid input is received
	do
	{
		std::cout << "You ran out of chips.  Would you like to buy more?\n  ";
		std::cout << "0) No\n  1) Yes\n  ";
		int userChoice = GetUserInput(_user);
		if (userChoice == 0)
		{
			ExitSlots(EExitCode::OUT_OF_CHIPS, _user);
		}
		else if (userChoice == 1)
		{
			BuyMoreChips(_user);

			if (_user->GetChips() > 0) //chips went to 0 but player bought more, so return true for "do you want to continue
			{
				return true;
			}
			else //chips went to 0, user said they want to buy more, but didn't actually buy any.  Assumes they chose to leave instead.
			{
				ExitSlots(EExitCode::OUT_OF_CHIPS, _user);
			}
		}
		else //triggers if input is negative (indicating input error), or outside range (item not on menu)
		{
			inputInvalid = true; //repeat loop for valid input
		}
	} while (inputInvalid);

	return false; //theoretically shouldn't get to here, as code above either returns true or exits via function call
}

//this function handles the calling of other functions to make the slot machine run
void RunSlots(SlotMachineUser* _user)
{
	PrintSlotUI(_user, true);

	int playerChoice = GetMenuSelection(_user);
	string currentProfitLoss;

	switch (playerChoice)
	{
	case -1: //input not a number - handled by other function, included here to exclude it from default
		break;
	case 1: //Play Slots - calls the slot function
		GetUserBet(_user);
		break;
	case 2: //Credits
		_user->SetOutput("This program was written by David Fransham, 2022\n\n  ");
		std::cout << _user->GetOutput();
		break;
	case 3: //Quit
		ExitSlots(EExitCode::USER_CHOSE_QUIT, _user);
		return; //probably not necessary as I know this function exits the program, but included for clarity of code
	case 4: //display profit tracker 
		currentProfitLoss = DescribeCurrentPosition(true, _user->GetFinancialPosition());
		_user->SetOutput(currentProfitLoss);
		std::cout << _user->GetOutput();
		break;
	case 5: //Cash Out
		CashOutChips(_user);
		break;
	case 6: //deposit more, only available if chips <= 500.
		if (_user->GetChips() <= 500)
		{
			BuyMoreChips(_user);
			break; //falls through to default if 5 is not on the menu
		}
	default: //should only be numbers that are not menu items
		InvalidInput(EInputErrors::NOT_ON_MENU, _user);
		break;
	}
	return;
}

//allows user to buy more chips, subtracting the amount they buy from the profit/loss tracking variable in the process
void BuyMoreChips(SlotMachineUser* _user)
{
	bool repeatBlock = false;
	do
	{
		std::cout << "How many more chips would you like to buy? (Max 5000)\n  ";
		int moreChips = GetUserInput(_user);
		if (moreChips == 0)
		{
			return;
		}
		else if (moreChips < 0)
		{
			repeatBlock = true;
		}
		else if (moreChips >= 5000)
		{
			_user->SetOutput("You have purchased the maximum number of chips allowed, 5000.\n  ");
			std::cout << _user->GetOutput();
			_user->CashInOrOut(5000);
			break;
		}
		else
		{
			string tempStr = "You purchased $";
			tempStr += std::to_string(moreChips);
			tempStr += " more chips.\n  ";
			_user->SetOutput(tempStr);
			std::cout << _user->GetOutput();

			_user->CashInOrOut(moreChips);
			break;
		}
	} while (repeatBlock);
	return;
}

//allows user to cash out some of their chips, adding the amount to the profit/loss tracker.
//this is to simulate the user taking some profit on their winnings to ensure they don't make a loss overall later
void CashOutChips(SlotMachineUser* _user)
{
	bool repeatBlock = false;
	do
	{
		int chipsNow = _user->GetChips();
		std::cout << "How much would you like to cash out?\n  ";
		int lessChips = GetUserInput(_user);
		if (lessChips == 0)
		{
			_user->SetOutput("You chose to return to the casino without cashing anything out.\n  ");
			std::cout << _user->GetOutput();
			return;
		}
		else if (lessChips < 0) //ignored because -1 means bad input
		{
			repeatBlock = true;
		}
		else if (lessChips >= chipsNow) //user chose to cash out all their chips
		{
			ExitSlots(EExitCode::USER_CHOSE_QUIT, _user);
		}
		else  //all that's left is: 0 < chip value entered < max chips held
		{
			string tempStr = "You cashed out ";
			tempStr += std::to_string(lessChips);
			tempStr += " and return to the casino.\n  ";

			_user->CashInOrOut(lessChips * -1);
			chipsNow = _user->GetChips();

			tempStr += "You still have ";
			tempStr += std::to_string(chipsNow);
			_user->SetOutput(tempStr);
			std::cout << _user->GetOutput();
			break;
		}
	} while (repeatBlock);

	return;
}

//print menu, calls for input, returns input value
int GetMenuSelection(SlotMachineUser* _user)
{
	GoToXY(2, 17);
	std::cout << "1) Play Slots!\n  ";
	std::cout << "2) Credits\n  ";
	std::cout << "3) Quit Slot Machine\n  ";
	std::cout << "4) Show Today's Winnings (or Losses)\n  ";
	std::cout << "5) Cash Out\n  ";
	if (_user->GetChips() <= 500) //only shows if user has 500 or fewer chips
	{
		std::cout << "6) Buy More Chips\n  ";
	}
	return GetUserInput(_user);
}

//rather than having multiple checks every time I want to cin, this function is called to take input and validate the data type
//the only input I need in this program is numbers, so this function takes a line of input from cin, checks it is all numbers and returns
int GetUserInput(SlotMachineUser* _user)
{
	string tempStr;
	std::getline(std::cin, tempStr);
	std::cout << "  ";

	_user->SetInput(tempStr);
	if (tempStr.empty())  //user pressed enter without any input first
	{
		InvalidInput(EInputErrors::NO_INPUT_GIVEN, _user);
		return -1;
	}
	else if (IsOnlyNumbers(tempStr))  //string to integer for valid numerical input
	{
		return stoi(tempStr);
	}
	else  //user entered non-numerical input value (includes . and - as non-valid)
	{
		InvalidInput(EInputErrors::NOT_NUMBER, _user);
		return -1;
	}
}

//Asks user for a value to bet on the slots.  Will handle invalid input.
void GetUserBet(SlotMachineUser* _user)
{
	int chipsNow = _user->GetChips();
	int inputBet;

	//infinite loop so function runs again after invalid input
	while (true)
	{
		std::cout << "How much would you like to bet? (0 to go back to main menu)\n  ";

		inputBet = GetUserInput(_user);
		if (inputBet == 0)  //user got cold feet and decided not to gamble just now
		{
			_user->SetOutput("You have chosen to return to the previous menu.\n  ");
			std::cout << _user->GetOutput();
			break;
		}
		else if (inputBet < 0)
		{	//deliberately runs no code as negative value only arises from bad input
			//this if statement catches the bad input and returns to the infite loop to try again
		}
		else if (inputBet > chipsNow) //User tried to bet more than they have available
		{
			InvalidInput(EInputErrors::INVALID_BET, _user);
		}
		else
		{
			StartSlots(inputBet, _user);
			break;
		}
	}
	return;
}

//function to spin the slots, print the numbers, and return a value showing the win code
int GetSlotWinOrLose(SlotMachineUser* _user)
{
	PrintSlotUI(_user, false);
	int slotNums[3];

	SetRgb(EColour::COLOUR_RED_ON_BLACK);

	//"clear" screen by printing space characters over existing numbers in slot machine
	for (int j = 0; j < 3; j++)
	{
		GoToXY((GetScreenWidth() / 2 - 5 + (5 * j)), 6);
		std::cout << " ";
	}

	//generates the three random numbers, stores the values in an array
	for (int j = 0; j < 3; j++)
	{
		slotNums[j] = GetRandomNumber(2, 7);
		Sleep(1001); //simulate the wheels spinning into place by taking time

		//print the newly generated value to the screen
		GoToXY((GetScreenWidth() / 2 - 5 + (5 * j)), 6);
		if (slotNums[j] == 7)
		{
			SetRgb(EColour::COLOUR_RED_ON_BLACK);
		}
		else
		{
			SetRgb(EColour::COLOUR_BLUE_ON_BLACK);
		}
		std::cout << slotNums[j];
		_user->LastSpin[j] = slotNums[j];
	}

	PrintSlotUI(_user);

	//checks the array to see if it is a winner or not
	//on relfection, this would be better as a separate function with a pointer to work with, but I ran out of time to implement this change
	if ((slotNums[0] == slotNums[1]) && (slotNums[1] == slotNums[2]))
	{
		if (slotNums[1] == 7)
		{
			return JACKPOT_THREE_SEVENS;
		}
		else
		{
			return THREE_NUMS_MATCH;
		}
	}
	else if ((slotNums[0] == slotNums[1]) || (slotNums[0] == slotNums[2]) || (slotNums[1] == slotNums[2]))
	{
		return TWO_NUMS_MATCH;
	}
	else
	{
		return LOSING_SPIN;
	}
}

//takes players bet as argument, calls the slot function and calculates result
void StartSlots(int playerBet, SlotMachineUser* _user)
{
	_user->AddChips(playerBet * -1); //subtract bet that was put in the slotmachine
	int spinResult;
	spinResult = GetSlotWinOrLose(_user);

	SetRgb(EColour::COLOUR_GREEN_ON_BLACK);
	std::cout << "\n\n  ";

	string tempStr = "";
	switch (spinResult)
	{
	case LOSING_SPIN:
		tempStr += "Sorry, you did not win this time.\n\n  ";
		break;
	case TWO_NUMS_MATCH:
		tempStr += "You matched two numbers, and won three times your bet!\n  ";
		break;
	case THREE_NUMS_MATCH:
		tempStr += "You matched three numbers, and won five times your bet!\n  ";
		break;
	case JACKPOT_THREE_SEVENS:
		tempStr += "You hit the jackpot and spun three 7s!  You won 10 times your bet!\n  ";
		break;
	default:
		std::cout << "Something unexpected happened, please contact the developer for more info\n  ";
		break;
	}

	if (spinResult != LOSING_SPIN)
	{
		int winnings = (playerBet * spinResult);
		tempStr += "You receive $";
		tempStr += std::to_string(winnings);

		_user->AddChips(winnings); //return any winnings to the player's pot
	}
	_user->SetOutput(tempStr);
	std::cout << _user->GetOutput();

	return;
}

//clears the screen, prints the border and the slots, and the most recent values in order to keep screen tidy.
void PrintSlotUI(SlotMachineUser* _user, bool _bIncludeLast)
{
	ClearScreen();

	//print border
	for (int i = 0; i < GetScreenWidth(); ++i)
	{
		SetRgb(EColour::COLOUR_BLACK_ON_GRAY);
		GoToXY(i, 0);
		std::cout << "+";
		GoToXY(i, GetScreenHeight() - 1);
		std::cout << "+";
	}
	for (int j = 1; j < GetScreenHeight() - 1; ++j)
	{
		GoToXY(0, j);
		std::cout << "|";
		GoToXY(GetScreenWidth() - 1, j);
		std::cout << "|";
	}

	//print current chips
	SetRgb(EColour::COLOUR_CYAN_ON_BLACK);
	GoToXY(2, 2);
	for (int i = 1; i < GetScreenWidth() - 2; ++i)
	{
		std::cout << " ";
	}
	GoToXY(2, 2);
	std::cout << " Your chips: $" << _user->GetChips();

	//print slot machine outline
	SetRgb(EColour::COLOUR_YELLOW_ON_BLACK);
	GoToXY((GetScreenWidth() / 2 - 8), 4);
	std::cout << "XXXXXXXXXXXXXXXXX";
	for (int i = 5; i < 8; i++)
	{
		GoToXY((GetScreenWidth() / 2 - 8), i);
		std::cout << "XX   XX   XX   XX";
	}
	GoToXY((GetScreenWidth() / 2 - 8), 8);
	std::cout << "XXXXXXXXXXXXXXXXX";

	//print the last slot numbers spun in the middle of the slot machine
	PrintLastSpin(_user);

	//prints last input, and last output if boolean argument is true
	SetRgb(EColour::COLOUR_BLUE_ON_BLACK);
	GoToXY(2, 12);
	std::cout << _user->GetInput() << "\n\n  ";
	if (_bIncludeLast)
	{
		std::cout << _user->GetOutput() << "\n\n  ";
	}
	SetRgb(EColour::COLOUR_GREEN_ON_BLACK);

	return;
}

//prints the most recently spun numbers on the slot machine
void PrintLastSpin(SlotMachineUser* _user)
{
	for (int i = 0; i < 3; ++i)
	{
		if (_user->LastSpin[i] == 7)
		{
			SetRgb(EColour::COLOUR_RED_ON_BLACK);
		}
		else
		{
			SetRgb(EColour::COLOUR_BLUE_ON_BLACK);
		}
		GoToXY((GetScreenWidth() / 2 - 5 + (5 * i)), 6);
		std::cout << _user->LastSpin[i];
	}
	SetRgb(EColour::COLOUR_GREEN_ON_BLACK);

	return;
}

//prints relevant error messages after input validation during code
void InvalidInput(EInputErrors ErrCode, SlotMachineUser* _user)
{
	SetRgb(EColour::COLOUR_MAGENTA_ON_BLACK);
	switch (ErrCode)
	{
	case EInputErrors::NOT_NUMBER:
		_user->SetOutput("-----Please enter a positive whole number with no other characters-----\n\n  ");
		std::cout << _user->GetOutput();
		break;
	case EInputErrors::NOT_ON_MENU:
		_user->SetOutput("-----Please enter a number that matches a menu option.-----\n\n  ");
		std::cout << _user->GetOutput();
		break;
	case EInputErrors::INVALID_BET:
		_user->SetOutput("-----You can't bet more than you have.-----\n\n  ");
		std::cout << _user->GetOutput();
		break;
	case EInputErrors::NO_INPUT_GIVEN:
		_user->SetOutput("-----You just hit enter without any input.-----\n\n  ");
		std::cout << _user->GetOutput();
		break;
	default: //shouldn't be called, but in case of changes in future this will be picked up.
		std::cout << "\n  Something unexpected happened.  See developer for more info.\n\n  ";
		break;
	}
	_user->AddError();
	CheckErrorCounter(_user->GetErrors(), _user);
	PrintSlotUI(_user);
	SetRgb(EColour::COLOUR_GREEN_ON_BLACK);
}

//prints a string to display how much money the user has made or lost today.
//Takes a boolean argument to assess whether they are still playing or whether they are leaving casino now
string DescribeCurrentPosition(bool _bStillPlaying, int _iMoney)
{
	string profitLossDescription = "You ";

	//int temp = g_CumulativeSpendOrWin;
	//temp += CurrentChips;

	if (_iMoney == 0)
	{
		if (_bStillPlaying)
		{
			profitLossDescription += "are breaking";
		}
		else //if !stillplaying
		{
			profitLossDescription += "broke";
		}

		profitLossDescription += " even today, not bad.\n  ";
		return profitLossDescription;  //should be "You are breaking even today, not bad." or "You broke even today, not bad."
	}
	else //if money != 0
	{
		if (_bStillPlaying)
		{
			profitLossDescription += "are making";
		}
		else //if !stillplaying
		{
			profitLossDescription += "made";
		}
	}

	if (_iMoney < 0)
	{
		profitLossDescription += " an overall loss today of ";
	}
	else //if (money > 0).  == 0 won't occur because it was in the separate if statement above and has already been returned.
	{
		profitLossDescription += " overall winnings today of ";
	}

	profitLossDescription += std::to_string(_iMoney);
	profitLossDescription += ".\n  ";
	return profitLossDescription; //Should be "You are making/made an overal loss today of xxxx" or "You are making/made overall winnings today of xxxx"
}

//function to handle the various exit messages when ending the program
void ExitSlots(EExitCode eCode, SlotMachineUser* _user)
{
	string madeOrLost = DescribeCurrentPosition(false, _user->GetFinancialPosition());

	PrintSlotUI(_user);
	SetRgb(EColour::COLOUR_RED_ON_BLACK);
	int chipsNow = _user->GetChips();

	switch (eCode)
	{
	case EExitCode::OUT_OF_CHIPS:
		std::cout << "You ran out of money.  Please come back another time!\n  ";
		std::cout << madeOrLost;
		break;
	case EExitCode::USER_CHOSE_QUIT:
		std::cout << "You have chosen to cash out and leave.\n  ";
		std::cout << "You cash out " << chipsNow << "\n  ";
		std::cout << madeOrLost;
		std::cout << "Come back soon!\n\n\n";
		break;
	case EExitCode::TOO_MANY_BAD_INPUTS:
		std::cout << "You were warned, but you didn't listen.\n  ";
		std::cout << "Security guards escort you to the cash out desk, and the door.\n  ";
		std::cout << "You cash out " << chipsNow << "\n  ";
		std::cout << madeOrLost;
		break;
	default:
		std::cout << "Your time on the slot machines is up.\n  ";
		std::cout << "Thanks for playing, please come again!";
		break;
	}

	//pauses to allow user to read screen and hit enter to close program
	do
	{
		std::cout << '\n' << "  Press the Enter key to exit.";
	} while (std::cin.get() != '\n');

	exit(0);
}

//function to replace global constant
int GetScreenWidth()
{
	return 25;
}

//function to replace global constant
int GetScreenHeight()
{
	return 11;
}

//function to put cursor at a particular location on screen
void GoToXY(int _iX, int _iY)
{
	COORD point;
	point.X = _iX;
	point.Y = _iY;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point);

	return;
}

//function to set the console text to a particular colour
void SetRgb(EColour colour)
{
	switch (colour)
	{
	case EColour::COLOUR_WHITE_ON_BLACK: // White on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_RED_ON_BLACK: // Red on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		break;
	case EColour::COLOUR_GREEN_ON_BLACK: // Green on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		break;
	case EColour::COLOUR_YELLOW_ON_BLACK: // Yellow on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case EColour::COLOUR_BLUE_ON_BLACK: // Blue on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_MAGENTA_ON_BLACK: // Magenta on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_CYAN_ON_BLACK: // Cyan on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_BLACK_ON_GRAY: // Black on Gray.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | BACKGROUND_INTENSITY);
		break;
	case EColour::COLOUR_BLACK_ON_WHITE: // Black on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
		break;
	case EColour::COLOUR_RED_ON_WHITE: // Red on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED);
		break;
	case EColour::COLOUR_GREEN_ON_WHITE: // Green on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_GREEN);
		break;
	case EColour::COLOUR_YELLOW_ON_WHITE: // Yellow on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
		break;
	case EColour::COLOUR_BLUE_ON_WHITE: // Blue on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_MAGENTA_ON_WHITE: // Magenta on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_CYAN_ON_WHITE: // Cyan on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	case EColour::COLOUR_WHITE_ON_WHITE: // White on White.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_INTENSITY | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	default: // White on Black.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
	return;
}

//Casino security don't like it if you keep trying to break the machines...
void CheckErrorCounter(int _iErrors, SlotMachineUser* _user)
{
	string temp; //placeholder to wait for user to hit enter to continue
	if (_iErrors == 4)
	{
		ClearScreen();
		SetRgb(EColour::COLOUR_RED_ON_BLACK);
		std::cout << "\n\n\n\tCasino Security have been notified of disruption in the casino.\n\n";
		std::cout << "\tA security guard approaches you and asks you politely to follow the directions.\n\n";
		std::cout << "\tPress Enter to continue.";
		std::getline(std::cin, temp);
		//PrintSlotUI();
	}
	else if (_iErrors == 8)
	{
		ClearScreen();
		SetRgb(EColour::COLOUR_RED_ON_BLACK);
		std::cout << "\n\n\n\tCasino Security take you aside and speak to you sternly for several minutes.\n\n";
		std::cout << "\tYou have been warned previously.  Continued breaking of the rules will result in expulsion.\n\n";
		std::cout << "\tPress Enter to continue, but behave yourself...";
		std::getline(std::cin, temp);
		//PrintSlotUI();
	}
	else if (_iErrors == 10)
	{
		ExitSlots(EExitCode::TOO_MANY_BAD_INPUTS, _user);
	}
	else
	{
		return;
	}
}

//returns a (pseudo)random number between user's chosen minimum and maximum values
int GetRandomNumber(int _iMinRand, int _iMaxRand)
{
	return _iMinRand + rand() % ((_iMaxRand - _iMinRand) + 1);
}

//Clears console screen. Copied from Lecture Slides. I don't understand it, but it works.
void ClearScreen()
{
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
}

//This function takes a string as input, uses std::isdigit to loop through the stringand check if any digits are not numbers.
bool IsOnlyNumbers(const string& str)
{
	for (char const& c : str)
	{
		if (std::isdigit(c) == 0) return false;
	}
	return true;
}
//onlyNumbers is borrowed and adapted from :
//https://www.delftstack.com/howto/cpp/how-to-determine-if-a-string-is-number-cpp/
//Limitations: checks each character for 0 - 9, so doesn't recognise decimal point or - as numbers.
//Therefore only works for positive whole numbers.