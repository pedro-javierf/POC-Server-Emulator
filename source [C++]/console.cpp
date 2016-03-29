int pfail(const char *text)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 12);
	std::cout << "[X]" << text << "\n";
	SetConsoleTextAttribute(hConsole, 7);
	return 0;
}

int padvert(const char *text)
{

	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 14);
	std::cout << "[!]" << text << "\n";
	SetConsoleTextAttribute(hConsole, 7);
	return 0;



}
