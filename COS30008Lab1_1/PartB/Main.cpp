#include <iostream>
using namespace std;

class Entity
{
protected:
	string ID;
public:
	Entity()
	{
		ID = "Default";
	}
	void SetID(string ID)
	{
		this->ID = ID;
	}
	string GetID()
	{
		return ID;
	}
};


class Character : public Entity
{
private:
	string Profession;

public:
	Character(string ID, string Prof)
	{
		Entity::ID = ID;
		this->Profession = Prof;
	}
	void SetProfession(string Proffesion)
	{
		this->Profession = Profession;
	}
	string GetProfession()
	{
		return ID;
	}

	friend istream& operator >>(istream& aIstream, Character& aCharacter)
	{
		aIstream >> aCharacter.ID >> aCharacter.Profession;
		return aIstream;
	}
	friend ostream& operator<<(ostream& aOstream, Character& aCharacter)
	{
		aOstream << aCharacter.ID << "Prof:" << aCharacter.Profession;
		return aOstream;
	}
};

int main(int argc, char* argv[])
{
	Entity Something;

	string Name;
	cout << "Enter a name for your Something: ";
	cin >> Name;
	Something.SetID(Name);

	Character Guy("Sam", "Hobbit");
	cout << Something.GetID() << endl;
	cout << Guy.GetProfession() << endl;
	return 0;

}
