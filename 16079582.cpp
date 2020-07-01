#include <vector> //only to be used to contain dispenser states and nothing else
#include <iostream>
#include <string>
//By 16079582
using namespace std;

enum state { Out_Of_Poptarts, No_Credit, Has_Credit, Dispenses_Poptart }; //states for our main functions
enum stateParameter { No_Of_Poptarts, Credit, Cost_Of_Poptart }; //tracks our parameters

class StateContext; //prototype for StateContext 

class State//Base class for our states
{
protected:

	StateContext * CurrentContext; //State context will be used to determine the state at which the vending machine is in and allow us to change states or access functions

public:

	State(StateContext* Context) 
	{
		CurrentContext = Context;
	}
	virtual ~State(void)
	{
	}

};

class StateContext //Manages information between states
{
protected:

	State * CurrentState = nullptr; // Holds the state of the current program
	int stateIndex = 0;
	vector<State*> availableStates;//A vector that holds all the states of the program.
	vector<int> stateParameters;// holds the important parameters.

public:

	virtual ~StateContext()//A destructer that will empty the vectors when it's finished.
	{
		for (int index = 0; index < this->availableStates.size(); index++) delete this->availableStates[index];
		this->availableStates.clear();
		this->stateParameters.clear();
	}

	virtual void setState(state newState) //Sets the state when the program is ready to change states
	{
		this->CurrentState = availableStates[newState];
		this->stateIndex = newState;
	}

	virtual int getStateIndex(void) //gets the current index for stateindex
	{
		return this->stateIndex;
	}

	virtual void setStateParam(stateParameter SP, int value) //sets the variable of the passed parameter
	{
		this->stateParameters[SP] = value;
	}

	virtual int getStateParam(stateParameter SP)//able to retrieve the variable of the passed parameter
	{
		return this->stateParameters[SP];
	}
};
//Base class for each state in the event.
class Transition 
{
public:

	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; }
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }

	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }

	virtual bool addPoptart(int) { cout << "Error!" << endl; return false; }

	virtual bool dispense(void) { cout << "Error!" << endl; return false; }

};
//Base class for the bases
//Holds all the information on the available poptarts
class Product
{
protected:
	string product_description;
	int itemCost = 0;
public:
	virtual void consume(void)
	{

	}
	virtual int cost(void) { return this->itemCost; }
	virtual string description(void) { return product_description; }
	virtual Product* ReturnHighestCostItem(void)
	{
		return nullptr;
	}
};
//Polymorphism is used so there's no need for classes to be used directly
class PoptartState : public State, public Transition
{
public:
	PoptartState(StateContext* Context) : State(Context) {}
};
// state for when there's no more poptarts
class OutOfPoptarts : public PoptartState
{
public:
	OutOfPoptarts(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};
//State when there's no credit left
class NoCredit : public PoptartState
{
public:
	NoCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};
//States when there is credit
class HasCredit : public PoptartState
{
private:
	Product* result = nullptr;
public:
	HasCredit(StateContext* Context) :PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};
//State to dispense poptarts
class DispensesPoptart : public PoptartState
{

public:
	DispensesPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};


//Classes for our product bases.
class Base : public Product
{
public:
	Base(void)
	{
		this->product_description = "Base";
		this->itemCost = 50;
	}
};
class Plain : public Base
{
public:
	Plain(void)
	{
		this->product_description = "Plain";
		this->itemCost = 100;
	}
};
class Spicy : public Base
{
public:
	Spicy(void)
	{
		this->product_description = "Spicy";
		this->itemCost = 150;
	}
};
class Chocolate : public Base
{
public:
	Chocolate(void)
	{
		this->product_description = "Chocolate";
		this->itemCost = 200;
	}
};
class Coconut : public Base
{
public:
	Coconut(void)
	{
		this->product_description = "Coconut";
		this->itemCost = 200;
	}
};
class Fruity : public Base
{
public:
	Fruity(void)
	{
		this->product_description = "Fruity ";
		this->itemCost = 200;
	}
};

class Filling : public Product
{
public:
	virtual void fillProduct(Product* NewBase);
	virtual int cost(void);
	virtual string description(void);
	Product* ReturnHighestCostItem(void) {}
};

//Poptart_Dispenser inhertis from statecontext and transition which allows for state changes.
class Poptart_Dispenser : public StateContext, public Transition
{
	//Connection to theses classes so they can have access to DispensedItem
	friend class DispensesPoptart;
	friend class HasCredit;
private:
	PoptartState * poptartCurrentState = nullptr;
	bool itemDispensed = false;
	Product* DispensedItem = nullptr;
	bool itemRetrieved = false;
public:
	Poptart_Dispenser(int inventory_count);

	~Poptart_Dispenser(void);

	bool insertMoney(int money);

	bool makeSelection(int option);

	bool moneyRejected(void);

	bool addPoptart(int number);

	bool dispense(void);

	Product* getProduct(void);

	virtual void setStateParam(stateParameter SP, int value);

	virtual int getStateParam(stateParameter SP);
};

//constructor for Poptart_Dispenser
Poptart_Dispenser::Poptart_Dispenser(int inventory_count)
{
	//Initializes states then pushes back to a vector
	this->availableStates.push_back(new OutOfPoptarts(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesPoptart(this));
	//initializes parameters and also pushes back to a vector
	this->stateParameters.push_back(0);//This is the number of poptarts
	this->stateParameters.push_back(0);//This is the number of credits

	this->setState(Out_Of_Poptarts);//setting the initial state to out of poptarts
	if (inventory_count > 0)
	{
		this->addPoptart(inventory_count);
	}
}
Poptart_Dispenser::~Poptart_Dispenser(void)
{
	//if the item is failes to retrieve, the dispensed poptart gets deleted.
	if (!this->itemRetrieved)
	{
		delete this->DispensedItem;
	}
}
bool Poptart_Dispenser::insertMoney(int money)
{
	poptartCurrentState = (PoptartState*)this->CurrentState;
	return this->poptartCurrentState->insertMoney(money);
}

bool Poptart_Dispenser::makeSelection(int option)
{
	poptartCurrentState = (PoptartState*)this->CurrentState;
	return this->poptartCurrentState->makeSelection(option);
}

bool Poptart_Dispenser::moneyRejected(void)
{
	poptartCurrentState = (PoptartState*)this->CurrentState;
	return this->poptartCurrentState->moneyRejected();
}
bool Poptart_Dispenser::addPoptart(int number)
{
	poptartCurrentState = (PoptartState*)this->CurrentState;
	return this->poptartCurrentState -> addPoptart(number);
}
bool Poptart_Dispenser::dispense(void)
{
	poptartCurrentState = (PoptartState*)this->CurrentState;
	return this->poptartCurrentState->dispense();
}

Product* Poptart_Dispenser::getProduct(void)
{
	if (this->itemDispensed)//returns the poptart if it has been dispensed
	{
		this->itemDispensed = false;
		this->itemRetrieved = true;
		return this->DispensedItem;
	}
	return nullptr;
}
void Poptart_Dispenser::setStateParam(stateParameter SP, int value)//Sets the passed parameter with the variable that was passed in 
{
	this->stateParameters[SP] = value;
}

int Poptart_Dispenser::getStateParam(stateParameter SP)//Retireves the variable from the passed in parameter
{
	return this->stateParameters[SP];
}
//Methods called that handle the event while there are no more poptarts
bool OutOfPoptarts::insertMoney(int money)
{
	cout << "Error" << endl;//because there are no poptarts, vending machine won't accept money.
	return false;
}
bool OutOfPoptarts::makeSelection(int option)
{
	cout << "There's no poptarts left" << endl;
	return false;
}
bool OutOfPoptarts::moneyRejected(void)
{
	cout << "DECLINED" << endl;
	this->CurrentContext->setStateParam(Credit, 0);//Sets credit to 0

	this->CurrentContext->setState(Out_Of_Poptarts);//sets the vending machine state to Out of poptarts
	return true;

}
bool OutOfPoptarts::addPoptart(int number)
{
	
	if (this->CurrentContext->getStateParam(Credit) > 0)//Credits must be 0, as the next state it enters is No_Credit.
	{
		cout << "Eject money before continueing." << endl;
		return false;
	}
	else if (number <= 0)//Returns a false value if no poptarts are added.
	{
		cout << "Out of Poptarts!" << endl;
		return false;
	}
	else
	{
		this->CurrentContext->setStateParam(No_Of_Poptarts, number);//uses the value passed in to set the number of poptarts
		this->CurrentContext->setState(No_Credit);//
		cout << "Adding " << number << " poptarts" << endl;//displays the amount of poptarts added
		return true;
	}
	
}
bool OutOfPoptarts::dispense(void)
{
	cout << "Sorry, looks like you ate all the Poptarts" << endl;
	return true;
}
//Methods that handle the NoCredit event.
bool NoCredit::insertMoney(int money)
{
	cout << "You inserted: " << money;
	this->CurrentContext->setStateParam(Credit, money);//uses the passed variable to set credit
	cout << " Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);//sets state to Has_Credit.
	return true;
}
bool NoCredit::makeSelection(int option)
{
	cout << "No money!" << endl;
	return false;
}
bool NoCredit::moneyRejected(void)
{
	cout << "No Money!" << endl;
	return false;
}
bool NoCredit::addPoptart(int number)
{
	cout << "Already got a poptart!" << endl;
	return false;
}
bool NoCredit::dispense(void)
{
	cout << "No Money!" << endl;
	return false;
}
//Methods that handle the HasCredit State.
bool HasCredit::insertMoney(int money)
{
	cout << "You inserted: " << money;
	this->CurrentContext->setStateParam(Credit, this->CurrentContext->getStateParam(Credit) + money);//Gets current credit, then adds the passed variable to create the new credit.
	cout << " Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);
	return true;
}
bool HasCredit::makeSelection(int option)
{
	if (option == 1)
	{
		cout <<"Plain Base Selected" << endl;
		this->result = new Plain;

	}
	else if (option == 2)
	{
		cout << "Spicy Base Seleced" << endl;
		this->result = new Spicy;
	}
	else if (option == 4)
	{
		cout << "Chocolate Base Selected" << endl;
		this->result = new Chocolate;
	}
	else if (option == 8)
	{
		cout << "Coconut Base Selected" << endl;
		this->result = new Coconut;

	}
	else if (option == 16)
	{
		cout << "Fruity Base Selected" << endl;
		this->result = new Fruity;
	}
	else
	{
		cout << "Wrong selection" << endl;
		return false;
	}
	((Poptart_Dispenser*)(CurrentContext))->DispensedItem = result;
	this->CurrentContext->setState(Dispenses_Poptart);
	return true;
	
	
}
bool HasCredit::moneyRejected(void)
{
	
	cout << "Money ejecting" << endl;
	this->CurrentContext->setStateParam(Credit, 0);
	this->CurrentContext->setState(No_Credit);
	return true;
}
bool HasCredit::addPoptart(int number)
{
	cout << "ERROR" << endl;
	return false;
}
bool HasCredit::dispense(void)
{
	cout << "Error" << endl;
	return true;
}
//Methods for handeling the DispensesPoptart State
bool DispensesPoptart::insertMoney(int money)
{
	cout << "Error" << endl;
	return false;
}
bool DispensesPoptart::addPoptart(int number)
{
	cout << "Error" << endl;
	return false;

}
bool DispensesPoptart::makeSelection(int option)
{
	cout << "Error" << endl;
	return false;
}
bool DispensesPoptart::moneyRejected(void)
{
	cout << "Error" << endl;
	return false;
}
bool DispensesPoptart::dispense(void)
{
	//Check if user has enopugh credit
	//deduct cost from credit
	//reduce no of poptarts by 1
	//checks for next state
	if (this->CurrentContext->getStateParam(Credit)>= ((Poptart_Dispenser*)(CurrentContext))->DispensedItem->cost())//Checks to see if there's enough credits to pay for the poptart
	{


		

		

		this->CurrentContext->setStateParam(Credit,  this->CurrentContext->getStateParam(Credit) -((Poptart_Dispenser*)(CurrentContext))->DispensedItem->cost());//Takes away the cost of the poptart away from your credit.

		this->CurrentContext->setStateParam(No_Of_Poptarts, (this->CurrentContext->getStateParam(No_Of_Poptarts)-1));//reduces the amount of poptarts by 1
		((Poptart_Dispenser*)(CurrentContext))->itemDispensed = true;
		cout << ((Poptart_Dispenser*)(CurrentContext))->DispensedItem->description() << "poptart dispensed" << endl;//Gets the description of the product
		if (this->CurrentContext->getStateParam(Credit)>0 && this->CurrentContext->getStateParam(No_Of_Poptarts)> 0)//If theres credit and theres poptarts, it will be set to Has_Credit.
		{
			cout << "Current Credit: " << this->CurrentContext->getStateParam(Credit);
			this->CurrentContext->setState(Has_Credit);
			
		}
		else if (this->CurrentContext->getStateParam(No_Of_Poptarts) == 0)//If there's no poptarts then state will be set to Out_Of_Poptarts
		{
			cout << "No Poptarts!" << endl;
			this->CurrentContext->setState(Out_Of_Poptarts);
			
		}
		else if (this->CurrentContext->getStateParam(Credit) == 0 && this->CurrentContext->getStateParam(No_Of_Poptarts) > 0)//If there's poptarts but no credit, state will be set to No_Credit.
		{
			cout << "No money!" << endl;
			this->CurrentContext->setState(No_Credit);

		}
		else
		{
			cout << "Error" << endl;
			return false;
		}
			


		return true;
	}
	else//if there's not enough credit output and error and set to No_Credit.
	{
		cout << "Not enough credit, try again later" << endl;
		this->CurrentContext->setState(No_Credit);
		return false;
	}
	
	
}
