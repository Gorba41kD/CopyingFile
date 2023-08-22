#include<iostream>
#include<fstream>
#include<string>
#include<thread>
#include<mutex>

std::mutex writingMtx;
std::condition_variable writingCv;
bool writing = true;
bool writing_predicate()
{
	return writing;
}

std::mutex mtx;
std::condition_variable cv;
bool reading  = false;
bool exitOfFile = false;
bool reading_predicate()
{
	return reading;
}
std::string getLastLine(std::ifstream& in)
{
	std::string line;
	while (in >> std::ws && std::getline(in, line));
	line += " \n";
	return line;
}
void readinigFile()
{
	std::ifstream fin;
	std::ofstream fout;
	std::string text;
	while (true)
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, reading_predicate);
		if (exitOfFile == true)
		{
			break;
		}
		fin.open("writingFile.txt");
		fout.open("copyingFile.txt",std::ios_base::app);
		if (fin.is_open())
		{
			fout << getLastLine(fin);
			fout.close();
			writing = true;
			writingCv.notify_one();
		}
		else
		{
			std::cout << "Error opening file" << std::endl;
		}
		fin.close();
		reading = false;
	}
}
void writingFile()
{
	std::ofstream fout;
	char symbol{};
	std::string text{};
	do
	{
		std::cout << "Do you want to enter a file (y/n)" << std::endl;
		std::cin >> symbol;
		std::cin.ignore(32767, '\n');
		std::unique_lock<std::mutex> wrLock(writingMtx);
		writingCv.wait(wrLock, writing_predicate);
		if (symbol == 'y')
		{
			writing = false;
			std::cout << "Enter a file" << std::endl;
			std::getline(std::cin, text);
			text += '\n';
			fout.open("writingFile.txt", std::ios_base::app);
			fout << text;
			fout.close();
			reading = true;
			cv.notify_one();
		}
		else if(symbol == 'n')
		{
			exitOfFile = true;
			reading = true;
			cv.notify_one();
			break;
		}
	} while (true);
}
int main()
{
	std::thread th1(writingFile);
	std::thread th2(readinigFile);
	th1.detach();
	th2.join();
}