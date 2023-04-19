

#include <iostream>
#include "uart_manager.hpp"
#include <fstream>
#include <ctime>
#include <list>

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h> /* do funkcji popen() */


using namespace std;



string Scp(string sign, string measurements)
{
	string value;
	int scp = measurements.find(sign)+3;
	while(measurements.at(scp) != '|' && measurements.at(scp) != '\n')
	{
		value+=measurements.at(scp++);
	}
	return value;
}




int main()
{

	class Data
	{
	public:
		string t0;
		string t1;
		string p0;
		string r0;
		Data(string t0,string t1,string p0, string r0)
		{
			this->t0 = t0;
			this->t1 = t1;
			this->p0 = p0;
			this->r0 = r0;
		}
	};


	list<Data> dataList;
	list<Data>::iterator it;
	list<string> recivedData;
	const char* dataSet = "dataSet.txt";
	const int noOfPoints = 50;
	bool first = true;



	FILE * gnuStream = popen( "gnuplot", "w" );
	FILE * gnuStream1 = popen( "gnuplot", "w" );
	FILE * gnuStream2 = popen( "gnuplot", "w" );

	UartManager um;
	fstream file;
	char byte;
	string measurements,t0,t1,p0,r0;
	//int scp;
	time_t now;
	if(um.Configure_uart() == 1)
		return 1;
	while(true)
	{
		byte = um.Recive_byte();
		if(byte == -1)
		{
			cout<<"Connection lost. Program terminated...\n";
			pclose( gnuStream );
			pclose( gnuStream1 );
			pclose( gnuStream2 );
			break;
		}

		measurements+=byte;
		if(measurements.find("\n")!=string::npos)
		{
			if(first == true)
			{
				first = false;
				measurements.clear();
				continue;
			}

			now = time(0);
			tm *ptm = localtime(&now);
			string time;
			time+=std::to_string(ptm->tm_year + 1900);
			time+=":";
			time+=std::to_string(1 + ptm->tm_mon);
			time+=":";
			time+=std::to_string(ptm->tm_mday );
			time+="|";
			time+=std::to_string(ptm->tm_hour );
			time+=":";
			time+=std::to_string(ptm->tm_min);
			time+=":";
			time+=std::to_string(ptm->tm_sec );
			time+="|";
			time+=measurements;

			file.open( "measurements.txt", ios::app);
			if( !file.good())
			{
				cout<<"can't open measurements file\n";
				break;
			}

			//cout<<measurements<<endl;
			file<<time;
			file.close();


			/*scp = measurements.find("T0:")+3;
			while(measurements.at(scp) != '|' && measurements.at(scp) != '\n')
			{
				t0+=measurements.at(scp++);
			}
			scp = measurements.find("T1:")+3;
			while(measurements.at(scp) != '|' && measurements.at(scp) != '\n')
			{
				t1+=measurements.at(scp++);
			}
			scp = measurements.find("P0:")+3;
			while(measurements.at(scp) != '|' && measurements.at(scp) != '\n')
			{
				p0+=measurements.at(scp++);
			}*/

			t0 = Scp("T0:", measurements);
			t1 = Scp("T1:", measurements);
			p0 = Scp("P0:", measurements);
			r0 = Scp("R0:", measurements);

			dataList.push_back(Data(t0,t1,p0,r0));
			if(dataList.size()>noOfPoints)
			{
				dataList.pop_front();
			}
			file.open(dataSet, ios::out|ios::trunc);
			int i=0;
			for(Data element : dataList)
			{
				file<<i++<<"\t"<<element.t0<<"\t"<<element.t1<<"\t"<<element.p0<<"\t"<<element.r0<<endl;
			}
			t0.clear();
			t1.clear();
			file.close();

			measurements.clear();


			fprintf( gnuStream, "plot [0:50][0:100] \"%s\" u 1:2 w lp title \"T0\", \"%s\" u 1:3 w lp title \"T1\"\n",dataSet,dataSet);
			//fprintf( gnuStream, "plot \"%s\" u 1:3 w lp title \"T1\"\n",dataSet);
			fprintf( gnuStream1, "plot [0:50][-0.2:2.5]\"%s\" u 1:4 w lp title \"P0\"\n",dataSet);
			fprintf( gnuStream2, "plot [0:50][0:3.3]\"%s\" u 1:5 w lp title \"R0\"\n",dataSet);
			fflush( gnuStream );
			fflush( gnuStream1 );
			fflush( gnuStream2 );


		}
	}

	return 0;
}
